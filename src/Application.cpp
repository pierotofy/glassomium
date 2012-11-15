/*
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

   Copyright 2012 The Glassomium Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

// Fix a compilation issue on windows
#include <SFML/Graphics/RenderStates.hpp>
//const sf::RenderStates sf::RenderStates::Default;
//const sf::Time sf::Time::Zero;

#include "Application.h"
#include "Globals.h"
#include "Appinfo.h"
#include "Utils.h"
#include "Launcher.h"
#include "Globals.h"

bool g_debug = false;
float Application::windowWidth = 0;
float Application::windowHeight = 0;

using namespace pt;

Application::Application(int argc, char* argv[]){
	// Default window dimension unless specified otherwise
	Application::windowWidth = 800;
	Application::windowHeight = 600;
	fullscreen = false;
	mouseEnabled = true;

	// If there are no command line arguments, launch the launcher
	if (argc < 2){
		Launcher launcher;

		Application::windowWidth = (float)launcher.getResolutionWidth();
		Application::windowHeight = (float)launcher.getResolutionHeight();
		fullscreen = launcher.isFullscreen();
		g_debug = launcher.isDebug();
	}

	// skip program name argv[0] if present
	argc -= argc > 0; 
	argv += argc > 0;

	for (int i = 0; i < argc; i++){
		string arg(argv[i]);
		if (arg == "-f") fullscreen = true;
		else if ((arg == "-w" || arg == "--width") && i+1 < argc){
			int width = str_to_int(argv[i+1]);
			if (width != -1) Application::windowWidth = (float)width;
		}
		else if ((arg == "-h" || arg == "--height") && i+1 < argc){
			int height = str_to_int(argv[i+1]);
			if (height != -1) Application::windowHeight = (float)height;
		}
		else if (arg == "-d" || arg == "--debug"){
			g_debug = true;
		}	
		else if (arg == "-n" || arg == "--nomouse"){
			mouseEnabled = false;
		}
		else if (arg == "-?" || arg == "--help"){
			printUsage();
			exit(EXIT_SUCCESS);
		}			
	}

	cout << APP_NAME << " " << APP_VERSION << endl << "====================" << endl;
	cout << "Fullscreen: " << (fullscreen ? "yes" : "no" ) << endl;
	cout << "Width: " << Application::windowWidth << endl;
	cout << "Height: " << Application::windowHeight << endl;
	cout << "Debug mode: " << (g_debug ? "yes" : "no") << endl;
	cout << "Mouse enabled: " << (mouseEnabled ? "yes" : "no") << endl;
	cout << endl;
	cout << "Starting up..." << endl;
}

void Application::printUsage(){
  cout << "USAGE: "APP_NAME" [options]" << endl << endl << "Options:" << endl
	   << "  --help  \tPrint usage and exit." << endl
	   << "  --fullscreen, -f  \tRun in fullscreen." << endl
	   << "  --width  \tSpecify width of the window in pixels" << endl
	   << "  --height  \tSpecify height of the window in pixels" << endl
	   << "  --debug, -d  \tEnable debug information and display touch blobs on the screen to troubleshoot resolution issues" << endl
	   << "  --nomouse, -n  \tDisable mouse events. Only UDP TUIO events will allow the user to interact with the UI" << endl
	   << endl << "Examples:" << endl
	   << "  "APP_NAME" --width 800 --height 600 --nomouse" << endl
	   << "  "APP_NAME" -f -w 1600 -h 900 --debug -n" << endl;
   exit(EXIT_SUCCESS);
}

void Application::go(){

	// Check the video mode
	sf::VideoMode videoMode((int)Application::windowWidth, (int)Application::windowHeight, 32);
	if (fullscreen && !videoMode.isValid()){
		cerr << "The specified width and height are not supported by your video card, please choose from: " << endl;

		std::vector<sf::VideoMode> videoModes = sf::VideoMode::getFullscreenModes();
		for (unsigned int i = 0; i < videoModes.size(); i++){
			if (videoModes[i].bitsPerPixel != 32) continue; // Skip duplicates

			cerr << videoModes[i].width << "x" << videoModes[i].height << (i+1 < videoModes.size() ? ", " : "");
		}

		cerr << endl << "Could not initialize window" << endl;

		exit(EXIT_FAILURE);
	}

	// Create the main rendering window
	renderWindow = new sf::RenderWindow(videoMode, 
										APP_NAME" "APP_VERSION, 
										(fullscreen ? sf::Style::Fullscreen : sf::Style::Default));
	renderWindow->setFramerateLimit(60);

	// Set icon
	sf::Image icon;
	icon.loadFromFile("icon.png");
	renderWindow->setIcon(32, 32, icon.getPixelsPtr());

	// Hide cursor?
	if (!mouseEnabled){
		renderWindow->setMouseCursorVisible(false);
	}

	if (!Berkelium::init(Berkelium::FileString::empty(), 0, NULL)){
		std::cout << "Failed to initialize berkelium!" << std::endl;
        return;
	}

	ServerManager::initialize("localhost", 5555);

	FileManager::initialize();

	PhysicsManager::initialize();

	// Initialize window manager
	UIManager::initialize();

	tuioManager = new TuioManager(3333);
	
	// Fetch resources, setup layout
	try{
		UIManager::getSingleton()->updateServerResources();
	}catch(exception &e){
		// UI Server unrecheable, close window and exit
		cerr << e.what() << endl;
		return;
	}

	// Create the layout
	UIManager::getSingleton()->setupSystemLayout();

	// Start loop
	while (renderWindow->isOpen())
    {
        // Process events
        sf::Event e;
		while (renderWindow->pollEvent(e))
        {
            // Close window : exit
			if (e.type == sf::Event::Closed){
				renderWindow->close();
				return;
			}
		}

		// Esc will exit
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
			return;
		}

		if (mouseEnabled){
			handleMouseEvents();
		}

		UIManager::getSingleton()->update();

		Berkelium::update();

		// Clear screen
		renderWindow->clear(sf::Color(76, 76, 76));

		// Display our stuff
		UIManager::getSingleton()->draw(renderWindow);

		// Display window contents on screen
		renderWindow->display();
    }
}

// Helps distinguish between mousedown, mousemove and mouseup
bool g_mouseDown = false;
void Application::handleMouseEvents(){

	// Catch mouse events
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
		sf::Vector2i position = sf::Mouse::getPosition(*renderWindow);
		TouchEvent touchEvent(position.x, position.y, -1);

		// Mouse down?
		if (!g_mouseDown){
			UIManager::getSingleton()->onTouchDown(touchEvent);
			UIManager::getSingleton()->onTrackTouchDown(touchEvent);
			g_mouseDown = true;
		}else{
			UIManager::getSingleton()->onTouchMove(touchEvent);
			UIManager::getSingleton()->onTrackTouchMove(touchEvent);
		}
	}else{

		// Mouse up?
		if (g_mouseDown){
			sf::Vector2i position = sf::Mouse::getPosition(*renderWindow);
			TouchEvent touchEvent(position.x, position.y, -1);

			UIManager::getSingleton()->onTouchUp(touchEvent);
			UIManager::getSingleton()->onTrackTouchUp(touchEvent);
			g_mouseDown = false;
		}
	}
}

/** Return the ratio of the dimension of the render window */
float Application::getScreenRatio(){
	assert(Application::windowHeight > 0);

	return Application::windowWidth / Application::windowHeight;
}

/**
 * @param point point in percentage values (0..1),(0..1) as received from TUIO
 * @param cornerSize percentage value that indicates the size of a corner relative to the size of the screen
      ex. a value of 0.3 indicates that each corner spawns 30% of the screen height and 30% of the screen width
	  values bigger than 0.5 will be truncated to 0.5 (and every point on the screen will be considered a corner) 
 * @return Whether the point provided as an argument is touching one of the corners of the screen */ 
bool Application::isPointOnScreenCorner(const sf::Vector2f &point, float cornerSize){
	if (cornerSize > 0.5f) return true;
	else if (cornerSize < 0.0f) return false;

	return (point.x < cornerSize && point.y < cornerSize) || // Top left
			(point.x > (1.0f - cornerSize) && point.y < cornerSize) || // Top right
			(point.x < cornerSize && point.y > (1.0f - cornerSize)) || // bottom left
			(point.x > (1.0f - cornerSize) && point.y > (1.0f - cornerSize)); // bottom right
}

Application::~Application(){
	RELEASE_SAFELY(tuioManager);
	UIManager::destroy();
	PhysicsManager::destroy();
	FileManager::destroy();
	ServerManager::destroy();
	Berkelium::destroy();
	RELEASE_SAFELY(renderWindow);
}

int main(int argc, char** argv)
{
    // Create application object
    Application app(argc, argv);
	app.go();

    return 0;
}

