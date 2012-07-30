/*
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

   Copyright 2012 Piero Toffanin

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

#include "Launcher.h"
#include "Appinfo.h"
#include "Utils.h"

Launcher::Launcher(){
	// Find available resolutions
	std::vector<sf::VideoMode> videoModes = sf::VideoMode::getFullscreenModes();
	for (unsigned int i = 0; i < videoModes.size(); i++){
		if (videoModes[i].bitsPerPixel != 32) continue; // Avoid duplicates

		stringstream ss;

		Resolution res;
		res.width = videoModes[i].width;
		res.height = videoModes[i].height;
		ss << res.width << "x" << res.height;
		res.printable = ss.str();

		availableResolutions.push_back(res);
	}

	if (availableResolutions.size() > 0){
		run();
	}else{
		cerr << "No screen resolutions are available; make sure your video card driver is properly installed and enabled." << endl;
		exit(EXIT_FAILURE);
	}
}


void Launcher::run() {
	const float WINDOW_WIDTH = 480.0f;
	const float WINDOW_HEIGHT = 320.0f;

    // Create SFML's window.
	renderWindow = new sf::RenderWindow( sf::VideoMode( (int)WINDOW_WIDTH, (int)WINDOW_HEIGHT ), APP_NAME " "  APP_VERSION, sf::Style::Titlebar );

	// Load icon
	sf::Image icon;
	icon.loadFromFile("icon.png");
	renderWindow->setIcon(32, 32, icon.getPixelsPtr());

	// Load splash screen
	sf::Image splashImage;
	splashImage.loadFromFile("splash.png");

	// OK and exit buttons
    sfg::Button::Ptr launchButton( sfg::Button::Create( "Launch!" ) );
    launchButton->GetSignal( sfg::Widget::OnLeftClick ).Connect( &Launcher::LaunchButton_Click, this );
 
    sfg::Button::Ptr exitButton( sfg::Button::Create( "Exit" ) );
    exitButton->GetSignal( sfg::Widget::OnLeftClick ).Connect( &Launcher::ExitButton_Click, this );

	// Create layout
	sfg::Table::Ptr table( sfg::Table::Create() );
	table->SetRequisition(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));

	// Splash image
	table->Attach( sfg::Image::Create(splashImage), sf::Rect<sf::Uint32>( 0, 0, 3, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	// Options
	
	// Resolution

	resolutionComboBox = sfg::ComboBox::Create();
	for (unsigned int i = 0; i < availableResolutions.size(); i++){
		resolutionComboBox->AppendItem(availableResolutions[i].printable); 
	}
	resolutionComboBox->SelectItem(0);

	table->Attach( sfg::Label::Create( "Resolution:" ), sf::Rect<sf::Uint32>( 0, 1, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	table->Attach( resolutionComboBox, sf::Rect<sf::Uint32>( 1, 1, 2, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	// Aspect ratio
	/*
	aspectRatioComboBox = sfg::ComboBox::Create();
	aspectRatioComboBox->AppendItem( "4:3" );
	aspectRatioComboBox->AppendItem( "16:9" );

	Resolution selectedRes = availableResolutions[0];
	if (selectedRes.width % 4 == 0 && selectedRes.height % 3 == 0){
		aspectRatioComboBox->SelectItem(0);
	}else{
		aspectRatioComboBox->SelectItem(1);
	}

	table->Attach( sfg::Label::Create( "Aspect ratio:" ), sf::Rect<sf::Uint32>( 0, 2, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	table->Attach( aspectRatioComboBox, sf::Rect<sf::Uint32>( 1, 2, 2, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	*/

	// Debug
	debugChkBox = sfg::CheckButton::Create("");
	debugChkBox->SetActive(true);
	
	table->Attach( sfg::Label::Create( "Debug:" ), sf::Rect<sf::Uint32>( 0, 2, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	table->Attach( debugChkBox, sf::Rect<sf::Uint32>( 1, 2, 2, 1 ), sfg::Table::FILL, sfg::Table::FILL );


	// Full screen
	fullscreenChkBox = sfg::CheckButton::Create("");
	fullscreenChkBox->SetActive(true);
	
	table->Attach( sfg::Label::Create( "Fullscreen:" ), sf::Rect<sf::Uint32>( 0, 3, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	table->Attach( fullscreenChkBox, sf::Rect<sf::Uint32>( 1, 3, 2, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	// Notice
	table->Attach( sfg::Label::Create("To access more options launch the program from a console: glassomium --help"), sf::Rect<sf::Uint32>( 0, 4, 3, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	
	// Launch/exit buttons
	table->Attach( exitButton, sf::Rect<sf::Uint32>( 0, 5, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );
	table->Attach( launchButton, sf::Rect<sf::Uint32>( 2, 5, 1, 1 ), sfg::Table::FILL, sfg::Table::FILL );

	table->SetRowSpacing(0, 20.0f);
	table->SetRowSpacing(1, 10.0f);
	table->SetRowSpacing(2, 10.0f);
	table->SetRowSpacing(3, 20.0f);
 	table->SetRowSpacing(4, 20.0f);

    // Create a window and add the box layouter to it. Also set the window's title.
    sfg::Window::Ptr window( sfg::Window::Create(sfg::Window::BACKGROUND) );
    window->Add( table );

    // Create a desktop and add the window to it.
    sfg::Desktop desktop;
    desktop.Add( window );
	desktop.SetProperty<sf::Color>("Window", "BackgroundColor", sf::Color(0xb9, 0xd6, 0xf7));
	desktop.SetProperty<sf::Color>("Label", "Color", sf::Color::Black);
	desktop.SetProperty<sf::Color>("Button", "BackgroundColor", sf::Color(0x7a, 0xaf, 0xef));
	desktop.SetProperty<float>("Button", "BorderWidth", 1.0f);
	desktop.SetProperty<sf::Color>("Button", "BorderColor", sf::Color(0x10, 0x10, 0x10));	
	desktop.SetProperty<sf::Color>("Button", "Color", sf::Color::Black);

	desktop.SetProperty<sf::Color>("ComboBox", "BackgroundColor", sf::Color(0x7a, 0xaf, 0xef));
	desktop.SetProperty<sf::Color>("ComboBox", "HighlightedColor", sf::Color::White);
	desktop.SetProperty<sf::Color>("ComboBox", "ArrowColor", sf::Color::Black);
	desktop.SetProperty<sf::Color>("ComboBox", "Color", sf::Color::Black);
	desktop.SetProperty<sf::Color>("ComboBox", "BorderColor", sf::Color::Black);

	desktop.SetProperty<sf::Color>("CheckButton", "BackgroundColor", sf::Color(0x7a, 0xaf, 0xef));
	desktop.SetProperty<sf::Color>("CheckButton", "CheckColor", sf::Color::Black);
	desktop.SetProperty<sf::Color>("CheckButton", "Color", sf::Color::Black);
	desktop.SetProperty<sf::Color>("CheckButton", "BorderColor", sf::Color::Black);

    // We're not using SFML to render anything in this program, so reset OpenGL
    // states. Otherwise we wouldn't see anything.
    renderWindow->resetGLStates();
 
    // Main loop!
    sf::Event event;
    sf::Clock clock;
 
    while( renderWindow->isOpen() ) {
        // Event processing.
        while( renderWindow->pollEvent( event ) ) {
            desktop.HandleEvent( event );
 
            // If window is about to be closed, leave program.
            if( event.type == sf::Event::Closed ) {
                renderWindow->close();
            }

			if ( event.type == sf::Event::MouseWheelMoved){
				resolutionComboBox->SelectItem(resolutionComboBox->GetSelectedItem() - event.mouseWheel.delta);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
				resolutionComboBox->SelectItem(resolutionComboBox->GetSelectedItem() - 1);
			}else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
				resolutionComboBox->SelectItem(resolutionComboBox->GetSelectedItem() + 1);
			}else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
				ExitButton_Click();
			}else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)){
				LaunchButton_Click();
			}
        }
 
        // Update SFGUI with elapsed seconds since last call.
        desktop.Update( clock.restart().asSeconds() );
 
        // Rendering.
		renderWindow->clear();
        sfgui.Display( *renderWindow );
        renderWindow->display();
    }
}

void Launcher::LaunchButton_Click() {
	renderWindow->close();
}

void Launcher::ExitButton_Click() {
	exit(EXIT_SUCCESS);
}

Launcher::~Launcher(){
	RELEASE_SAFELY(renderWindow);
}

