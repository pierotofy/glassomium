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
   
#ifndef WINDOW_H
#define WINDOW_H

#include "stdafx.h"
#include "WebView.h"
#include "Rectangle.h"
#include "TouchGroup.h"

using namespace std;

enum WindowType{
	User, System, Keyboard, Browser, Screensaver
};

enum WindowOrientation{
	Bottom, Left, Top, Right
};

namespace pt{

/** This class displays the actual web content to the user and can be dragged, resized, moved, etc. */
class Window {
public:
	static unsigned int windowCount;

	Window(float normalizedWidth, float normalizedHeight);
    virtual ~Window();

	void loadURL(const string &);
	void loadFile(const string &);
	void loadInternal(const string &);
	void reload();

	void translate(float screen_delta_x, float screen_delta_y);
	void rotate(Degrees angle);
	void scale(float scale);
	void scale(float scale_x, float scale_y);

	void setScale(const sf::Vector2f &);
	void setRotation(Degrees angle);

	void setZOrder(int z);
	int getZOrder();
	bool isZStatic();

	void setTransparent(bool);
	bool isTransparent();

	void show();
	void hide();
	bool isVisible();

	float getWidth() const;
	float getHeight() const;

	float getTextureWidth() const;
	float getTextureHeight() const;

	float getNormalizedWidth() const;
	float getNormalizedHeight() const;

	Degrees getRotation() const;
	Degrees getNormalizedRotation() const;
	void normalizeRotation();
	WindowOrientation getOrientation() const;
	sf::Vector2f getScale() const;

	void setPosition(const sf::Vector2f &screenPosition);
	void setPosition(float screen_x, float screen_y);
	sf::Vector2f getPosition() const; // screen coordinates

	void resizeSprite(float width, float height);

	void onMouseDown(int cursor_id, int screen_x, int screen_y);
	void onMouseUp(int cursor_id, int screen_x, int screen_y);
	void onMouseMove(int cursor_id, int screen_x, int screen_y);

	void onKeyDown(int modifiers, int vkCode);
	void onKeyUp(int modifiers, int vkCode);
	void injectText(const std::string &utf8);

	void setZoom(float zoom);
	void zoomIn();
	void zoomOut();

	void scroll(int dx, int dy);
	void setScrollOnMouseMove(bool flag);
	void setScrollOnPinch(bool flag);

	void executeJavascript(const string &code);

	void setTuioEnabled(bool flag);
	bool isTuioEnabled(){ return tuioEnabled; }
	void fireJsTuioEvent(const string &name, TouchGroup *group, int blob_id, const sf::Vector2f &webviewCoords);

	pt::Rectangle getRectangle();
	pt::Rectangle getClientRectangle();

	bool coordsInsideWindow(float screen_x, float screen_y);
	bool coordsInsideWindow(float screen_x, float screen_y, sf::Vector2f &webviewCoords);
	bool coordsInsideWindow(sf::Vector2f screenCoords[], int numCoords);

	sf::Vector2f screenToWebViewCoords(const sf::Vector2f &screenCoords);

	static sf::Vector2f getPointRotatedAround(Radians angle, const sf::Vector2f &pivot, const sf::Vector2f &point);
	
	bool hasWebView(WebView *webView);
	virtual void injectJavascriptResources();

	/** In this method you specify which javascript functions should be binded by berkelium
	 * at page load. Only those function returned in this list will be listened and callbacked */
	virtual std::vector<std::wstring> getJavascriptBindings();

	/** This method is called by the webview whenever a javascript callback event is detected 
	 * javascript callbacks can only happen if they are registered in getJavascriptBindings */
	virtual void onJavascriptCallback(std::wstring functionName, std::vector<std::string> params);

	inline unsigned int getID(){ return id; }
	sf::Sprite *getSprite(){ return sprite; }

	void updateScrolling(const sf::Vector2f &);

	void startDragging(const sf::Vector2f &);
	void updateDragging(const sf::Vector2f &);
	void stopDragging(const sf::Vector2f &);

	void startTransforming(const sf::Vector2f &, float, const sf::Vector2f &, const sf::Vector2f &);
	void updateTransform(const sf::Vector2f &, float transformDistanceFromCenter, const sf::Vector2f &, const sf::Vector2f &);
	void stopTransforming();
	void blockTransforms();

	void setDraggable(bool);
	void setTransformable(bool);
	void setScrollable(bool);

	void setPinchableOutOfFullscreen(bool);
	void setPinchableToFullscreen(bool);
	void setFullscreen(bool);
	bool isFullscreen();

	Degrees popRotation();
	sf::Vector2f popScale();
	sf::Vector2f popPosition();

	void pushRotation();
	void pushScale();
	void pushPosition();

	sf::Uint8 getAlpha();
	void setAlpha(sf::Uint8 alpha);
	
	bool rotationSameAs(Window *otherWindow, Degrees threshold);

	void copyAttributesTo(Window *otherWindow);

	virtual void update();  // To be called from the main loop (or UIManager)

	/** Called whenever a new page is starting to load */
	virtual void onStartLoading();

	/** Called after we know that the DOM of the window has loaded */
	virtual void onDOMLoaded();

	void repaint();

	/** Called after a crash occured and a new window has been created */
	virtual void onCrash(const string &description);
	bool isCrashed(){ return crashed; }

	virtual WindowType getType() = 0;
protected:
	float normalizedWidth; // The width of the window in 0..1 range passed to the constructor
	float normalizedHeight;  // The height of the window in 0..1 range passed to the constructor

	bool fullscreen; // Is the window running in full screen?
	bool pinchableOutOfFullscreen; // Can this window be pinched out of full screen?
	bool pinchableToFullscreen; // Can this window be pinched to full screen?

	bool dragging; // Flag that tells whether this window is being dragged

	bool draggable; // Is this window draggable?
	bool transformable; // Can this window be resized/rotated?
	bool scrollable; // Can we do scrolling?

	bool crashed; // Has the window crashed?
	std::map<std::string, std::string> crashReport;

	void setBlendColor(const sf::Color &);

	bool isTouchingCorner(const sf::Vector2f &, float);

	virtual void handleMouseDown(int cursor_id, const sf::Vector2f &webviewCoords);
	virtual void handleMouseUp(int cursor_id, const sf::Vector2f &webviewCoords);
	virtual void handleMouseMove(int cursor_id, const sf::Vector2f &webviewCoords);

	// Current zoom level
	float currentZoom;

	bool scrollOnPinch; // Whether a pinch during full screen will cause a scroll event

	bool scrollOnMouseMove; // This flag tells whether a mouse move will cause a scroll event instead of a mouse move
	bool scrolledOnMouseMove; // Whether we have scrolled in the last mouse move
	bool mouseDown; // Whether the mouse is down on this window
	sf::Vector2f lastMouseDownLocation; // webview coords
	sf::Vector2f lastMouseMoveLocation; // webview coords
	sf::Vector2f lastDeltaMouseMove; // amount of space moved during the last mouse move

	bool tuioEnabled; // Whether javascript tuio events will be fired on mousedown, mousemove and mouseup

	sf::Vector2f transformCoordinatesForWebView(pt::Rectangle &windowRect, const sf::Vector2f &rotatedScreenCoords);
	sf::Vector2f getRotatedScreenCoords(pt::Rectangle &windowRect, float screen_x, float screen_y);

	bool coordsInsideWindow(float screen_x, float screen_y, pt::Rectangle &windowRect, sf::Vector2f &webviewCoords);

	WebView *webView;
	sf::Sprite *sprite;
	int zOrder; // current z-order

	unsigned int id; // Unique for each window
private:

#define SMOOTH_DRAG 1
#ifdef SMOOTH_DRAG
	sf::Vector2f beginningDragTouchPosition; // Used to compute the window dragging algorithm
	sf::Vector2f windowCenterOnDragBegin; // Same
#endif

	bool blockTransformsFlag; // After we detect a scroll, we want to avoid receiving accidental transform gestures
	float transformDistanceFromCenterOnTransformBegin;
	sf::Vector2f windowScaleOnTransformBegin;
	sf::Vector2f previousWindowScale;
	int previousDy;
	Radians previousWindowRotation;
	bool deltaScaleBigEnough;
	bool deltaRotationBigEnough;
	sf::Vector2f previousFirstTouchLocation;
	sf::Vector2f previousSecondTouchLocation;
	sf::Vector2f firstTouchLocationOnTransformBegin;
	sf::Vector2f secondTouchLocationOnTransformBegin;
	sf::Vector2f centerLocationOnTransformBegin;
	bool pinchedOutOfFullscreen; // Has the window just returned to windowed mode?

	// Sprite sizes before a fullscreen (to exit out of fullscreen)
	int widthBeforeFullscreen;
	int heightBeforeFullscreen;

	// And other properties
	bool draggableBeforeFullscreen;
	bool transparentBeforeFullscreen;

	std::stack<Degrees> rotationStack;
	std::stack<sf::Vector2f> positionStack;
	std::stack<sf::Vector2f> scaleStack;
};


}
#endif
