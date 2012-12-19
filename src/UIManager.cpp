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
   
#include "Application.h"
#include "UIManager.h"
#include "Utils.h"
#include "Globals.h"
//#include "KeyboardMap.h"
#include "TwoFingerGesture.h"
#include "DragGesture.h"
#include "TouchGesture.h"
#include "ColorChangeAnimation.h"
#include "TransformAnimation.h"
#include "UserWindow.h"
#include "SystemWindow.h"
#include "KeyboardWindow.h"
#include "BrowserWindow.h"
#include "ScreensaverWindow.h"

// Z order configuration
#define FOREGROUND_Z_ORDER_START 1000000000
#define KEYBOARD_Z_ORDER_START   2000000000
#define BACKGROUND_Z_ORDER_START -2140000000
#define NUM_APPLICATIONS 200000000
#define USER_Z_ORDER_START	-(NUM_APPLICATIONS)
#define SCREENSAVER_Z_ORDER 2140000000
#define FULLSCREEN_Z_ORDER  1999999999

UIManager *UIManager::singleton = 0;

void UIManager::initialize(){
	UIManager::singleton = new UIManager();
}

void UIManager::destroy(){
	RELEASE_SAFELY(UIManager::singleton);
}

UIManager *UIManager::getSingleton(){
	return singleton;
}

UIManager::UIManager(){
	assert(UIManager::getSingleton() == 0); // Don't allow multiple instantiations

    // Instantiate gesture manager
	gestureManager = new GestureManager();

	// Instantiate animation manager
	animationManager = new AnimationManager();

	overlaySprite = new OverlaySprite((unsigned int)Application::windowWidth, (unsigned int)Application::windowHeight);

	screensaverShowing = false;

	appConfigs = 0;
}

/** Requests from the UI server data that the UI manager will utilize to 
 * do its job. You need to call this method at startup and whenever the UI server
 * has changed the data (UI reload?). The program will throw an excetion of it fails. */
void UIManager::updateServerResources(){
	ServerManager::getSingleton()->retrieveJsResources();

	appConfigs = ServerManager::getSingleton()->retrieveAppConfigurations();
	themeConfig = ServerManager::getSingleton()->retrieveThemeConfiguration();

	//themeConfig->dump();

	screensaverWait = max(0, themeConfig->getInt("screensaver.wait"));

	PhysicsManager::getSingleton()->setEnabled(themeConfig->getBool("physics.enabled"));
	PhysicsManager::getSingleton()->setFriction(themeConfig->getFloat("physics.drag-friction"));
	PhysicsManager::getSingleton()->setRestitution(themeConfig->getFloat("physics.drag-restitution"));
	
	overlaySprite->setColor(intToColor(themeConfig->getInt("desktop.fade-transition-color")));
}


/** Setup the system windows, menus, keyboards and widgets of the theme received
 * from the UI Server. There must be no windows in the windows list before calling this method */
void UIManager::setupSystemLayout(){
	std::vector<std::string> components = themeConfig->getComponentsList();
	for (unsigned int i = 0; i < components.size(); i++){
		
		// What is each component?
		WindowType windowType;

		// What is the actual Z-order?
		int zOrder;

		string componentType = themeConfig->getString(components[i], "window.type");
		if (componentType == "foreground"){
			windowType = System;

			// Z-order starts at 0 in the configuration, but we need to translate it to 
			// a value that is suitable for our UI manager
			// for foreground system windows, they should never be on top of keyboards

			// -2 (-1-1) to allow for full screen applications to run on top of system windows + 1 space
			zOrder = FOREGROUND_Z_ORDER_START + max(0, min(KEYBOARD_Z_ORDER_START - FOREGROUND_Z_ORDER_START - 2, themeConfig->getInt(components[i], "window.z-order")));
		}else if (componentType == "background"){
			windowType = System;

			// Make sure it's not more than the number of applications
			zOrder = BACKGROUND_Z_ORDER_START + max(0, min(-(BACKGROUND_Z_ORDER_START) - NUM_APPLICATIONS, themeConfig->getInt(components[i], "window.z-order")));
		}else if (componentType == "keyboard"){
			windowType = Keyboard;

			zOrder = KEYBOARD_Z_ORDER_START + max(0, themeConfig->getInt(components[i], "window.z-order"));
		}else if (componentType == "user"){
			windowType = User;

			zOrder = USER_Z_ORDER_START + max(0, min(NUM_APPLICATIONS - 1, themeConfig->getInt(components[i], "window.z-order")));
		}else{
			cout << "FATAL! Invalid window type '" << componentType << "', check your theme configuration!" << endl;
			exit(1);
		}

		Degrees rotation = themeConfig->getFloat(components[i], "window.rotation");

		// Cap values between 0 and 1.0
		float windowWidth = max(0.0f, min(1.0f, themeConfig->getFloat(components[i], "window.width")));
		float windowHeight = max(0.0f, min(1.0f, themeConfig->getFloat(components[i], "window.height")));
		float windowPosPercentageX = max(0.0f, min(1.0f, themeConfig->getFloat(components[i], "window.position.x")));
		float windowPosPercentageY = max(0.0f, min(1.0f, themeConfig->getFloat(components[i], "window.position.y")));

		// Convert position percentages to screen coordinates
		float windowPosX = windowPosPercentageX * Application::windowWidth;
		float windowPosY = windowPosPercentageY * Application::windowHeight;

		// Find URL to load
		string url = themeConfig->getString(components[i], "window.URL");
		if (url == ""){
			cout << "FATAL! Empty URL in window component [" << components[i] << "], check your theme configuration!" << endl;
			exit(1);
		}

		// Add prefix if the URL starts with /
		if (url[0] == '/'){
			url = "http://localhost:5555" + url;
		}

		// Create window
		Window *w = createWindow(windowWidth, windowHeight, windowType);
		w->loadURL(url);
		w->setPosition(windowPosX, windowPosY);
		w->setZOrder(zOrder);	
		w->setRotation(rotation);
	}

	// Extract drag color components
	int color = themeConfig->getInt("windows.drag-color");
	dragColor = intToColor(color);

	onSystemLayoutChanged();
}

/** Helper to converts a 4 bytes integer to a SFML color object */
sf::Color UIManager::intToColor(int color){
	return sf::Color((sf::Uint8)(color >> 16), 
						  (sf::Uint8)(color >> 8), 
						  (sf::Uint8)(color));
}

/** This is called by the main loop, updates resources connected to the UI Manager */
void UIManager::update(){
	for (unsigned int i = 0; i < windows.size(); i++){
		windows[i]->update();
	}

 	getGestureManager()->processQueue();
	getAnimationManager()->processQueue();
	PhysicsManager::getSingleton()->update();

	// Check screensaver
	if (screensaverWait > 0 && screensaverClock.getElapsedTime().asSeconds() > screensaverWait && !screensaverShowing){
		showScreensaver();
	}

	// Check garbage bin
	if (!garbageBin.empty() && garbageClock.getElapsedTime().asSeconds() > 10){
		for (unsigned int i = 0; i < garbageBin.size(); i++){
			if (g_debug){
				cout << "Disposed window " << garbageBin[i]->getID() << endl;
			}
			RELEASE_SAFELY(garbageBin[i]);
		}
		garbageBin.clear();
	}
}

/** Creates a screensaver window and displays it to the user, provided that a screensaver
 * is not already displayed */
void UIManager::showScreensaver(){
	if (!screensaverShowing){
		Window *screensaverWindow = createWindow(1.0f, 1.0f, Screensaver);
		screensaverWindow->setZOrder(SCREENSAVER_Z_ORDER); // On top of everything
		sortWindowsByZOrder();
		screensaverWindow->hide();

		Animation *fadeIn = new FadeInAnimation(200, screensaverWindow);
		fadeIn->start();

		screensaverShowing = true;
		putToSleepAllWindowsExcept(screensaverWindow);
	}
}

/** Takes care of drawing our stuff in the rendering window. The order of drawing
 * will decide the Z ordering of the elements */
void UIManager::draw(sf::RenderWindow *renderWindow){
	// Windows are ordered by Z value (top most window = beginning of the list)
	// so by iterating in reverse order we will get the correct rendering
	unsigned int size = windows.size();
	for (unsigned int i = 0; i < size; i++){
		renderWindow->draw(*windows[size - 1 - i]->getSprite());
	}	

	// Draw the overlay (when needed)
	if (overlaySprite->isVisible()) renderWindow->draw(*overlaySprite->getSprite());

	// Pointer sprites last
	std::map<int, PointerSprite *>::iterator iter;
	for (iter = visiblePointers.begin(); iter != visiblePointers.end(); iter++){
		renderWindow->draw(*iter->second);
	}
}

/** Display a keyboard window for the window that has made a request for it */
void UIManager::showKeyboard(Window *window){
	// Are there any keyboards available?
	if (keyboardsCount == 0) return;

	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i]->getType() == Keyboard){
			KeyboardWindow *kw = (KeyboardWindow *)windows[i];

			// Available and rotation is close within certain degrees?
			// The closest keyboard will be opened
			if (kw->available() && kw->rotationSameAs(window, 180.0f / (float)keyboardsCount)){
				// Yes, link and show!

				kw->linkTo(window);
				kw->show();
				break;
			}
		}
	}
}

/** Hide (remove) the keyboard from the window specified as an argument */
void UIManager::hideKeyboard(Window *window){
	// Which keyboard is our window using?

	for (unsigned int i = 0; i < windows.size(); i++){
		if (dynamic_cast<KeyboardWindow*>(windows[i]) != NULL){
			KeyboardWindow *kw = (KeyboardWindow *)windows[i];

			if (kw->isLinkedTo(window)){
				kw->unlink();
				kw->hide();
				kw->reload(); // So that the user starts with a "fresh" keyboard next time 
				break;
			}
		}
	}
}

/** Based on the location of the parent's center, chooses a new center for the new child window */
void UIManager::setNewWindowCenter(Window *parent, Window *newWindow){
	sf::Vector2f parentCenter = parent->getPosition();

	// If the parent window crashed, simply set the new position to the same
	if (parent->isCrashed()){
		newWindow->setPosition(parentCenter);
		return;
	}

	// In what corner of the screen is the parent?
	float halfWidth = Application::windowWidth / 2.0f;
	float halfHeight = Application::windowHeight / 2.0f;

	float displacement = (parent->getHeight() / 2.0f) + (newWindow->getHeight() / 2.0f) + 10.0f;

	// Upper left?
	if (parentCenter.x < halfWidth && parentCenter.y < halfHeight){
		newWindow->setPosition(parentCenter.x + displacement, parentCenter.y + displacement);
	}

	// Upper right?
	else if (parentCenter.x >= halfWidth && parentCenter.y < halfHeight){
		newWindow->setPosition(parentCenter.x - displacement, parentCenter.y + displacement);
	}

	// Lower left?
	else if (parentCenter.x < halfWidth && parentCenter.y >= halfHeight){
		newWindow->setPosition(parentCenter.x + displacement, parentCenter.y - displacement);
	}

	// Lower right?
	else if (parentCenter.x >= halfWidth && parentCenter.y >= halfHeight){
		newWindow->setPosition(parentCenter.x - displacement, parentCenter.y - displacement);
	}
}

/** Creates a new window of the specified type and adds it to the list of available windows. 
 * @param width percentage between 0 and 1 that specifies the width of the window (relative to the screen)
 * @param height percentage between 0 and 1 that specifies the height of the window (relative to the screen)
 * @param type type of the window to create
 * @return a pointer to the newly created window or NULL on error */
Window *UIManager::createWindow(float width, float height, WindowType type){
	if (width > 1.0f) width = 1.0f;
	if (height > 1.0f) height = 1.0f;

	if (g_debug){
		cout << "Creating window " << width << "x" << height << endl;
	}

	Window *w = NULL;

	// What type of window are we creating?
	if (type == User){
		w = new UserWindow(width, height);
	}else if (type == System){
		w = new SystemWindow(width, height);
	}else if (type == Keyboard){
		w = new KeyboardWindow(width, height);
	}else if (type == Browser){
		w = new BrowserWindow(width, height);
	}else if (type == Screensaver){
		w = new ScreensaverWindow(width, height);
	}

	// Created?
	if (w != NULL){
		windows.push_back(w);

		if (type == User || type == Browser){
			// Modify Z-order of existing applications
			setTopMostWindow(w);
		}else{
			// Simply sort
			sortWindowsByZOrder();
		}
	}

	return w;
}

/** To be called after the initial layout of the UI has been changed */
void UIManager::onSystemLayoutChanged(){
	sortWindowsByZOrder();
	updateKeyboardsCount();
}

/** Updates the count of the number of keyboards available */
void UIManager::updateKeyboardsCount(){
	int count = 0;
	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i]->getType() == Keyboard){
			count++;
		}
	}

	cout << "Found " << count << " keyboards" << endl;
	keyboardsCount = count;
}

/** Windows with higher Z order will be first in the list */
void UIManager::sortWindowsByZOrder(){
	sort (windows.begin(), windows.end(), UIManager::compareWindowsZOrder); 
}

/** Dumps a list of the current windows along with some information
 * Useful for debugging */
void UIManager::dumpWindows(){
	cout << "Dumping windows: " << endl;
	for (unsigned int i = 0; i < windows.size(); i++){
		cout << "  Window #" << windows[i]->getID() << 
			    " type " << windows[i]->getType() << 
				" z-Order " << windows[i]->getZOrder() << endl;
	}
}

/** Returns the window with the highest visible Z order that is being hit by the coordinates provided
 * @return window pointer if one is found, NULL otherwise */
Window* UIManager::findFirstWindow(float screen_x, float screen_y){
	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i]->isVisible()){
			if (windows[i]->coordsInsideWindow(screen_x, screen_y)){
				return windows[i];
			}
		}
	}

	return NULL;
}

/** Iterates through the list of windows searching for a window with the given ID.
 * @return the window with the given ID, or NULL if none is found. */
Window* UIManager::findWindowById(int windowId){
	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i]->getID() == windowId){
			return windows[i];
		}
	}

	return NULL;
}

/** Returns the window with the highest visible Z order that is also TUIO-enabled hit by the coordinates provided
 * @return window pointer if one is found, NULL otherwise */
Window* UIManager::findFirstTuioEnabledWindow(float screen_x, float screen_y, sf::Vector2f &webviewCoords){
	// Will need to scan our windows, find one (if any) and convert to webview coordinates
	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i]->isVisible() && windows[i]->isTuioEnabled()){
			if (windows[i]->coordsInsideWindow(screen_x, screen_y, webviewCoords)){
				return windows[i];
			}
		}
	}

	return NULL;
}

/** Returns the window with the highest Z order that is being hit by all the coordinates provided
 * @return window pointer if one is found, NULL otherwise */
Window* UIManager::findFirstWindow(sf::Vector2f screenCoords[], int numCoords){
	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i]->isVisible()){
			bool allInside = true;
			for (int j = 0; j < numCoords; j++){
				if (!windows[i]->coordsInsideWindow(screenCoords[j].x, screenCoords[j].y)){
					allInside = false;
					break;
				}
			}

			if (allInside) return windows[i];
		}
	}
	
	return NULL;
}

void UIManager::addPointer(int screen_x, int screen_y, int pointer_id, PointerSprite::Color color = PointerSprite::Red){
	#define POINTER_RADIUS 30.0f

	// Make sure we do not have already added the same pointer
	if (visiblePointers.count(pointer_id) == 0){
		PointerSprite *pointerSprite = new PointerSprite(POINTER_RADIUS/800.0f * Application::windowWidth, color);
		pointerSprite->setPosition((float)screen_x, (float)screen_y);
		visiblePointers[pointer_id] = pointerSprite;
	}
}

void UIManager::movePointer(int screen_x, int screen_y, int pointer_id){
	PointerSprite *pointerSprite = visiblePointers[pointer_id];
	pointerSprite->setPosition((float)screen_x, (float)screen_y);
}

void UIManager::removePointer(int screen_x, int screen_y, int pointer_id){
	PointerSprite *pointerSprite = visiblePointers[pointer_id];
	RELEASE_SAFELY(pointerSprite);
	visiblePointers.erase(pointer_id);
}

/** This is called by webviews whenever a new window is asked to be opened 
  * @param parent the Window where the request came from */
void UIManager::onNewWindowRequested(const string &url, Window *parent, WindowType type){
	// Does this URL have a configuration? If not, generate a temporary one to get the default values
	bool removeTemporaryConfiguration = false;
	if (appConfigs->count(url) == 0){
		(*appConfigs)[url] = new AppConfiguration();
		removeTemporaryConfiguration = true;
	}

	Window *newWindow;

	// Is the parent is a user window? (Was this window spawned as a popup from an existing user window?)
	if (parent->getType() == User){
		newWindow = UIManager::getSingleton()->createWindow(parent->getNormalizedWidth(), parent->getNormalizedHeight(), type);
		parent->copyAttributesTo(newWindow);
	}else{
		// This was spawned by a system window (probably an application launch)
		float width = (*appConfigs)[url]->getFloat("window.width");
		float height = (*appConfigs)[url]->getFloat("window.height");

		// Is there an aspect ratio to override the height?
		float aspectRatio = (*appConfigs)[url]->getFloat("window.aspectratio");
		if (aspectRatio != 0.0f){
			float heightPixels = (width * Application::windowWidth) / aspectRatio;
			height = heightPixels / Application::windowHeight;
		}

		newWindow = UIManager::getSingleton()->createWindow(width, height, type);

		// User windows might need to change some application configs
		if (type == User){
			newWindow->setTransparent((*appConfigs)[url]->getBool("window.transparent"));
		}
	}

	newWindow->hide();

	// Start loading
	newWindow->loadURL(url);

	// Set center position
	setNewWindowCenter(parent, newWindow);

	// Rotation is inherited from parent
	newWindow->setRotation(parent->getRotation());

	setTopMostWindow(newWindow);

	// Start full screen
	if ((*appConfigs)[url]->getBool("window.fullscreen")){
		UIManager::getSingleton()->setFullscreen(newWindow);
	}

	// Fade in
	Animation *fadeIn = new FadeInAnimation(200, newWindow);
	fadeIn->start();

	// User windows will dispose this object
	if (removeTemporaryConfiguration){
		RELEASE_SAFELY((*appConfigs)[url]);
		appConfigs->erase(url);
	}
   
}

/** This is called when a webviews is asked to be closed */
void UIManager::onCloseWindowRequested(Window *sender){
	//closeWindow(sender);
	fadeAndCloseWindow(sender);
}

/** Called whenever a window has requested to be set to fullscreen */
void UIManager::onWindowEnterFullscreenRequested(Window *sender){
	if (!sender->isFullscreen()){
		
		// Save position/rotation/scale for exiting fullscreen later
		sender->pushPosition();

		// Normalize first
		sender->normalizeRotation();
		sender->pushRotation();

		sender->pushScale();

		animateFadeAndSetFullscreen(sender);
		//animateScaleAndSetFullscreen(sender);
	}
}

/** Sets a window in full screen mode */
void UIManager::setFullscreen(Window *window){
	window->setFullscreen(true);
	window->setZOrder(FULLSCREEN_Z_ORDER);
	sortWindowsByZOrder();

	// Put all other windows asleep
	putToSleepAllWindowsExcept(window);

	//dumpWindows();
}

void UIManager::animateFadeAndSetFullscreen(Window *window){
	// Fade overlay in
	overlaySprite->setData((void *)window);
	Animation *a = new FadeInAnimation(250, overlaySprite, animateFadeAndSetFullscreenCallback);
	a->start();
}

void UIManager::animateFadeAndSetFullscreenCallback(AnimatedObject *o){
	// Set window to fullscreen, then fade overlay out
	Window *w = (Window *)((OverlaySprite *)o)->getData();
	UIManager::getSingleton()->setFullscreen(w);

	Animation *a = new FadeOutAnimation(250, o);
	a->start();
}

void UIManager::animateScaleAndSetFullscreen(Window *window){
	sf::Vector2f targetPosition(Application::windowWidth / 2.0f, Application::windowHeight / 2.0f);

	// Calculate rotation offset to reach fullscreen
	window->normalizeRotation(); // We are now in a 0..360 range
	Degrees currentRotation = window->getRotation();
	Degrees targetRotation = 0.0f;

	WindowOrientation orientation = window->getOrientation();

	if (orientation == Bottom){
		if (currentRotation >= 0.0f && currentRotation <= 45.0f){
			targetRotation = 0.0f;
		}else{
			targetRotation = 360.0f;
		}
	}else{
		switch(orientation){
		case Left:
			targetRotation = 90.0f;
			break;
		case Top:
			targetRotation = 180.0f;
			break;
		case Right:
			targetRotation = 270.0f;
			break;
		case Bottom: // Never executed, but compiler complains if it's not here
			targetRotation = 0.0f;
			break;
		}
	}

	sf::Vector2f targetScale;
	if (orientation == Bottom || orientation == Top){
		targetScale = sf::Vector2f(Application::windowWidth / window->getTextureWidth(), 
							Application::windowHeight / window->getTextureHeight());
	}else{
		targetScale = sf::Vector2f(Application::windowHeight / window->getTextureWidth(), 
							Application::windowWidth / window->getTextureHeight());
	}

	Animation *a = new TransformAnimation(250, targetScale, targetPosition, targetRotation, TransformAnimation::Linear, window, animateScaleAndSetFullscreenCallback);
	a->start();
}

void UIManager::animateScaleAndSetFullscreenCallback(AnimatedObject *w){
	UIManager::getSingleton()->setFullscreen((Window *)w);
}


/** Called whenever a window has requested to exit out of fullscreen.
 * No animation/scaling/rotating is performed. */
void UIManager::onWindowExitFullscreenRequested(Window *sender){
	if (sender->isFullscreen()){
		sender->setFullscreen(false);
		sender->setZOrder(-1);
		sortWindowsByZOrder();

		wakeUpAllWindowsExcept(sender);
	}
}

/** Called whenever a window wants to return to its previous rotation/dimension and scale
 * before going to fullscreen. */
void UIManager::onWindowAnimatedExitFullscreenRequested(Window *sender){
	if (sender->isFullscreen()){
		onWindowExitFullscreenRequested(sender);
		Degrees targetRotation = sender->popRotation();
		sf::Vector2f targetScale = sender->popScale();
		sf::Vector2f targetPosition = sender->popPosition();

		// Clock-wise or counter clock-wise faster?
		if (fabs(sender->getRotation() - targetRotation) <= 180.0f){
			targetRotation = targetRotation;
		}else{
			targetRotation = targetRotation - 360.0f;
		}

		Animation *a = new TransformAnimation(250, targetScale, targetPosition, targetRotation, TransformAnimation::Linear, sender);
		a->start();
	}
}

/** Closes a window, frees up any resource that it might be using */
void UIManager::closeWindow(Window *window){
	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i] == window){
			
			// Unlink the keyboard
			hideKeyboard(window);

			// Remove from list
			windows.erase(windows.begin() + i);

			// Stop physics
			PhysicsManager::getSingleton()->stopAllPhysics(window);

			// If the window was in fullscreen, we need to remove the
			// reference to the fullscreen window and do other stuff
			if (window->isFullscreen()){
				wakeUpAllWindowsExcept(window); // Others might be asleep
			}

			// Dispose later. This lets asynchronous events to be dispatched
			// without making a mess with disposed memory areas
			window->prepareForDisposal();
			garbageBin.push_back(window);
			garbageClock.restart();
			break;
		}
	}
}

/** Fades and closes a window */
void UIManager::fadeAndCloseWindow(Window *window){
	Animation *a = new FadeOutAnimation(100, window, fadeAndCloseWindowCallback);
	a->start();
}

void UIManager::fadeAndCloseWindowCallback(AnimatedObject *w){
	UIManager::getSingleton()->closeWindow((Window *)w);
}

/** Performs an animation on an existing screensaver window and closes it at the end */
void UIManager::onExitScreensaverRequested(Window *screensaver, ScreensaverAnimation animation, int animationMsTime){
	Animation *a = 0;

	switch(animation){
	case FadeOut:
		a = new FadeOutAnimation((float)animationMsTime, screensaver, exitScreensaverCallback);
		break;
	case SlideUp:
		a = new MoveAnimation((float)animationMsTime, sf::Vector2f(Application::windowWidth / 2.0f, -(Application::windowHeight / 2.0f)), TransformAnimation::Linear, screensaver, exitScreensaverCallback);
		break;
	case SlideDown:
		a = new MoveAnimation((float)animationMsTime, sf::Vector2f(Application::windowWidth / 2.0f, Application::windowHeight * 1.5f), TransformAnimation::Linear, screensaver, exitScreensaverCallback);
		break;
	case SlideLeft:
		a = new MoveAnimation((float)animationMsTime, sf::Vector2f(-(Application::windowWidth / 2.0f), Application::windowHeight / 2.0f), TransformAnimation::Linear, screensaver, exitScreensaverCallback);
		break;
	case SlideRight:
		a = new MoveAnimation((float)animationMsTime, sf::Vector2f(Application::windowWidth * 1.5f, Application::windowHeight / 2.0f), TransformAnimation::Linear, screensaver, exitScreensaverCallback);
		break;
	}

	a->start();

	screensaverClock.restart();
	screensaverShowing = false;
}

void UIManager::exitScreensaverCallback(AnimatedObject *screensaver){
	UIManager::getSingleton()->wakeUpAllWindowsExcept((Window *)screensaver); // Others might be asleep
	UIManager::getSingleton()->closeWindow((Window *)screensaver);
	
	if (g_debug){
		cout << "Closed screensaver!" << endl;
	}
}

void UIManager::onTouchGesture(const GestureEvent &gestureEvent){

	// TODO: use gestureEvent.windowId instead of finding the window using the location of the touch!

	TouchGesture *touch = static_cast<TouchGesture *>(gestureEvent.gesture);

	TouchEvent touchEvent = touch->getTouchEvent();
	if (touch->getPhase() == Gesture::BEGINNING){
		onTouchDown(touchEvent);
	}

	else if (touch->getPhase() == Gesture::UPDATING){
		onTouchMove(touchEvent);
	}

	else if (touch->getPhase() == Gesture::ENDING){
		onTouchUp(touchEvent);
	}
}

/** Handles a touch down event */
void UIManager::onTouchDown(const TouchEvent &touchEvent){
	Window *window = findFirstWindow((float)touchEvent.screen_x, (float)touchEvent.screen_y);

	if (window != NULL){
		setTopMostWindow(window);
		window->onMouseDown(touchEvent.touch_id, touchEvent.screen_x, touchEvent.screen_y);
	}	
}

/** Handles a touch move event */
void UIManager::onTouchMove(const TouchEvent &touchEvent){
	Window *window = findFirstWindow((float)touchEvent.screen_x, (float)touchEvent.screen_y);

	if (window != NULL){
		window->onMouseMove(touchEvent.touch_id, touchEvent.screen_x, touchEvent.screen_y);
	}
}

/** Handles a touch up event */
void UIManager::onTouchUp(const TouchEvent &touchEvent){
	Window *window = findFirstWindow((float)touchEvent.screen_x, (float)touchEvent.screen_y);
	if (window != NULL){
		window->onMouseUp(touchEvent.touch_id, touchEvent.screen_x, touchEvent.screen_y);
	}
}

void UIManager::onDragGesture(const GestureEvent &gestureEvent){
	DragGesture *drag = static_cast<DragGesture *>(gestureEvent.gesture);
	
	//Window *window = findFirstWindow(gestureEvent.location.x, gestureEvent.location.y);
	Window *window = findWindowById(gestureEvent.windowId);
	if (window != NULL){
		if (drag->getPhase() == Gesture::BEGINNING){
			window->startDragging(gestureEvent.location);
		}

		if (drag->getPhase() == Gesture::UPDATING){
			window->updateDragging(gestureEvent.location);
		}

		if (drag->getPhase() == Gesture::ENDING){
			window->stopDragging(gestureEvent.location, drag->getSpeedOnDragEnd());
		}
	}
}

/** Transform handles scale and rotate */
void UIManager::onTransformGesture(const GestureEvent &gestureEvent){
	TwoFingerGesture *twoFingerGesture = static_cast<TwoFingerGesture *>(gestureEvent.gesture);
	
	// Are the fingers inside the window?
	sf::Vector2f touches[2];
	touches[0] = twoFingerGesture->getFirstTouchLocation();
	touches[1] = twoFingerGesture->getSecondTouchLocation();

	touches[0].x *= Application::windowWidth;
	touches[0].y *= Application::windowHeight;

	touches[1].x *= Application::windowWidth;
	touches[1].y *= Application::windowHeight;

	//Window *window = findFirstWindow(touches, 2);
	Window *window = findWindowById(gestureEvent.windowId);
	if (window != NULL){
		if (twoFingerGesture->getPhase() == Gesture::BEGINNING){
			window->startTransforming(twoFingerGesture->getFirstTouchLocation(), 
										twoFingerGesture->getSecondTouchLocation());
		}

		if (twoFingerGesture->getPhase() == Gesture::UPDATING){
			window->updateTransform(twoFingerGesture->getFirstTouchLocation(), 
										twoFingerGesture->getSecondTouchLocation());

		}

		if (twoFingerGesture->getPhase() == Gesture::ENDING){
			window->stopTransforming();
		}
	}
}

/** Tracks the reception of a touch down event direcly from TUIO without being analyzed by the GestureManager
 * Must be called from the rendering thread */
void UIManager::onTrackTouchDown(const TouchEvent &touchEvent){
	// Reset screensaver timer
	screensaverClock.restart();

	if (g_debug){
		// We color the pointer based on the touchgroup to which it belongs to
		PointerSprite::Color pointerColor;

		// Mouse events are never part of a touchgroup, so just color a default value
		if (touchEvent.mouseSimulated){
			pointerColor = PointerSprite::Red;
		}else{
			int touchGroup = getGestureManager()->findTouchGroup(touchEvent.blob);

			pointerColor = (PointerSprite::Color)(touchGroup % 3); // Red (0), blue (1), green (2)
		}

		addPointer(touchEvent.screen_x, touchEvent.screen_y, touchEvent.touch_id, pointerColor);
	}

	// Handle TUIO enabled windows events
	// TUIO-enabled windows are special in the sense that they receive events directly from TUIO events
	// which are not filtered by the gesture manager (a two finger gesture will not be sent to the window
	// as a scale gesture, but as two distinct "touchDown" events). These windows will also receive the other
	// gesture messages, so it is responsability of the developer to disable certain gestures as not to create
	// collisions when the user interacts with the window (for example, do not activate TUIO and allow transforms at the same time!)

	sf::Vector2f webviewCoords;
	Window *w = findFirstTuioEnabledWindow((float)touchEvent.screen_x, (float)touchEvent.screen_y, webviewCoords);
	if (w != NULL){
		w->fireJsTuioEvent("touchstart", touchEvent.group, touchEvent.touch_id, webviewCoords); 
	}
}

/** Tracks the reception of a touch up event direcly from TUIO without being analyzed by the GestureManager 
 * Must be called from the rendering thread */
void UIManager::onTrackTouchUp(const TouchEvent &touchEvent){
	// Reset screensaver timer
	screensaverClock.restart();

	if (g_debug){
		removePointer(touchEvent.screen_x, touchEvent.screen_y, touchEvent.touch_id);
	}

	// Handle TUIO enabled windows events
	sf::Vector2f webviewCoords;
	Window *w = findFirstTuioEnabledWindow((float)touchEvent.screen_x, (float)touchEvent.screen_y, webviewCoords);
	if (w != NULL){
		w->fireJsTuioEvent("touchend", touchEvent.group, touchEvent.touch_id, webviewCoords); 
	}
}

/** Tracks the reception of a touch move event direcly from TUIO without being analyzed by the GestureManager
 * Must be called from the rendering thread  */
void UIManager::onTrackTouchMove(const TouchEvent &touchEvent){
	// Move the pointer sprite only when there's a down event
	if (g_debug && visiblePointers.count(touchEvent.touch_id) != 0){
		movePointer(touchEvent.screen_x, touchEvent.screen_y, touchEvent.touch_id);
	}

	// Handle TUIO enabled windows events
	sf::Vector2f webviewCoords;
	Window *w = findFirstTuioEnabledWindow((float)touchEvent.screen_x, (float)touchEvent.screen_y, webviewCoords);
	if (w != NULL){
		w->fireJsTuioEvent("touchmove", touchEvent.group, touchEvent.touch_id, webviewCoords); 
	}
}

/** Puts w as the topmost window and lowers the Z order of any other window
 * windows that have a Z order greater than 0 are not affected */
void UIManager::setTopMostWindow(Window *w){
	// Ignore static z-order windows (we never change their Z-order)
	if (w->isZStatic()) return;

	int previousZ = w->getZOrder();

	w->setZOrder(-1); // -1 is the topmost window following our convention

	// Avoid pushing down windows if w was already the topmost window
	if (previousZ != -1){
		for (unsigned int i = 0; i < windows.size(); i++){
			if (windows[i] != w && !windows[i]->isZStatic()){
				int currentZ = windows[i]->getZOrder();
				if (currentZ <= 0) windows[i]->setZOrder(currentZ - 1);
			}
		}
	}

	sortWindowsByZOrder();
}

/** increases the Z order of any other user window (with z order less than 0)
 *  system windows are not affected */
void UIManager::pushUpZOrdering(Window *w){
	for (unsigned int i = 0; i < windows.size(); i++){
		int currentZ = windows[i]->getZOrder();
		if (currentZ <= 0 && !windows[i]->isZStatic()) windows[i]->setZOrder(currentZ + 1);
	}
}

/** Wakes up all windows */
void UIManager::wakeUpAllWindowsExcept(Window *w){
	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i] != w) windows[i]->wakeUp();
	}
}

/** Put asleep all windows exept the one passed as an argument */
void UIManager::putToSleepAllWindowsExcept(Window *w){
	for (unsigned int i = 0; i < windows.size(); i++){
		if (windows[i] != w) windows[i]->setToSleep();
	}
}

UIManager::~UIManager(){
	for (unsigned int i = 0; i<windows.size(); i++){
		RELEASE_SAFELY(windows[i]);
	}
	windows.clear();

	if (appConfigs != 0){
		map<std::string, AppConfiguration *>::iterator it;
		for (it = appConfigs->begin(); it != appConfigs->end(); it++){
			RELEASE_SAFELY(it->second);
		}
		appConfigs->clear();
		RELEASE_SAFELY(appConfigs);
	}

	RELEASE_SAFELY(overlaySprite);
	RELEASE_SAFELY(gestureManager);
	RELEASE_SAFELY(animationManager);
}