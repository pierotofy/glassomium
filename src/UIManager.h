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
   
#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "stdafx.h"
#include "Window.h"
#include "PointerSprite.h"
#include "GestureManager.h"
#include "PhysicsManager.h"
#include "AnimationManager.h"
#include "AppConfiguration.h"
#include "ThemeConfiguration.h"

using namespace std;
using namespace pt;
class WebView;

enum ScreensaverAnimation{
	SlideUp, SlideDown, SlideLeft, SlideRight, FadeOut
};

class UIManager {
public:
	UIManager();
    ~UIManager();

	static void initialize();
	static void destroy();
	static UIManager *getSingleton();

	Window *createWindow(float width, float height, WindowType type);
	void updateServerResources();
	void setupSystemLayout();
	void onSystemLayoutChanged();

	void showKeyboard(Window *window);
	void hideKeyboard(Window *window);

	void closeWindow(Window *window);
	void fadeAndCloseWindow(Window *window);
	static void fadeAndCloseWindowCallback(Window *w);
	//void onKeyDown(const OIS::KeyEvent &e);
	//void onKeyUp(const OIS::KeyEvent &e);	

	void setFullscreen(Window *window);
	void animateScaleAndSetFullscreen(Window *window);
	static void animateScaleAndSetFullscreenCallback(Window *w);


	void onTouchGesture(const GestureEvent &);
	void onDragGesture(const GestureEvent &);
	void onTransformGesture(const GestureEvent &);

	void onTrackTouchDown(const TouchEvent &);
	void onTrackTouchUp(const TouchEvent &);
	void onTrackTouchMove(const TouchEvent &);

	void onTouchDown(const TouchEvent &);
	void onTouchUp(const TouchEvent &);
	void onTouchMove(const TouchEvent &);

	void onNewWindowRequested(const string &url, Window *parent, WindowType type);
	void onCloseWindowRequested(Window *sender);
	void onWindowEnterFullscreenRequested(Window *sender);
	void onWindowExitFullscreenRequested(Window *sender);
	void onWindowAnimatedExitFullscreenRequested(Window *sender);

	void onExitScreensaverRequested(Window *screensaver, ScreensaverAnimation animation, int animationMsTime);
	static void exitScreensaverCallback(Window *screensaver);

	void showScreensaver();

	// Window operations
	Window* findFirstWindow(float screen_x, float screen_y);
	Window* findFirstTuioEnabledWindow(float screen_x, float screen_y, sf::Vector2f &webviewCoords);
	Window* findFirstWindow(sf::Vector2f screenCoords[], int numCoords);
	Window* findWindowById(int windowId);

	void update(); // To be called from the main loop
	void draw(sf::RenderWindow *renderWindow); // Takes care of drawing the UI components in the rendering window

	GestureManager *getGestureManager(){ return gestureManager; }
	AnimationManager *getAnimationManager() { return animationManager; }
	sf::Color getDragColor() const{ return dragColor; } 
private:
	static UIManager *singleton;

	// Takes care of recognizing gestures
	GestureManager *gestureManager;

	// Takes care of animating windows
	AnimationManager *animationManager;

	// Screensaver clock
	sf::Clock screensaverClock;
	bool screensaverShowing;

	// Keeps a count of all the keyboard windows
	int keyboardsCount;

	// Pointer sprite (useful for debugging tuio messages)
	std::map<int, PointerSprite *> visiblePointers;
	void addPointer(int screen_x, int screen_y, int pointer_id, PointerSprite::Color color);
	void removePointer(int screen_x, int screen_y, int pointer_id);
	void movePointer(int screen_x, int screen_y, int pointer_id);

	// Handles Z ordering operations
	void setTopMostWindow(Window *w);
	void pushUpZOrdering(Window *w);
	void sortWindowsByZOrder();
	static bool compareWindowsZOrder (Window *i, Window *j) { return (i->getZOrder() > j->getZOrder()); }
	void dumpWindows();

	// Generic create window procedure
	Window *createWindow(float windowRatio);
	Window *createWindow(float width, float height);
	void adjustWindowScale(Window *w, float width, float height);
	void setNewWindowCenter(Window *parent, Window *newWindow);

	void updateKeyboardsCount();

	// Keeps track of all the windows
	std::vector<Window *> windows;

	// Keeps the configuration information about the applications that will be launched
	std::map<std::string, AppConfiguration *> *appConfigs;

	// Keeps the configuration of the current theme
	ThemeConfiguration *themeConfig;

	// The color of the windows during dragging
	sf::Color dragColor;

	// Seconds before showing the screensaver as read from the configuration
	int screensaverWait;
};

#endif
