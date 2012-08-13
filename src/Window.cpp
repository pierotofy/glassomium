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
#include "Window.h"
#include "Utils.h"
#include "Globals.h"

namespace pt{

unsigned int Window::windowCount = 0;

Window::Window(float normalizedWidth, float normalizedHeight){
	id = Window::windowCount;
	Window::windowCount++;

	this->normalizedWidth = normalizedWidth;
	this->normalizedHeight = normalizedHeight;

	// Instantiate web view
	webView = new WebView(normalizedWidth/normalizedHeight, this);

	// Create sprite to hold the webviews texture
	sprite = new sf::Sprite(*webView->getTexture());
	sprite->setOrigin(webView->getTextureWidth() / 2.0f, webView->getTextureHeight() / 2.0f);

	// By default a mouse move is a mouse move
	scrollOnMouseMove = false;
	mouseDown = false;
	scrolledOnMouseMove = false;

	// Don't fire tuio events unless specified otherwise
	tuioEnabled = false;

	crashed = false; 

	currentZoom = 1.0f; // Normal
	zOrder = 0;

    dragging = blockTransformsFlag = false;
	scrollable = draggable = transformable = true;

	widthBeforeFullscreen = heightBeforeFullscreen = 0;
	fullscreen = false;
	draggableBeforeFullscreen = false;
	pinchedOutOfFullscreen = false;
	pinchableOutOfFullscreen = true;
}

/** Sets a color that will blend the content of the window */
void Window::setBlendColor(const sf::Color &color){
	sprite->setColor(color);
}

/** Set whether this window is draggable */
void Window::setDraggable(bool draggable){
	this->draggable = draggable;
}

/** Set whether this window is resizable/scalable */
void Window::setTransformable(bool transformable){
	this->transformable = transformable;
}

/** Set whether this window can be scrolled */
void Window::setScrollable(bool scrollable){
	this->scrollable = scrollable;
}

/** Converts a scroll direction to the proper values and scrolls the window */
void Window::updateScrolling(const sf::Vector2f &scrollDirection){
	if (scrollable && !scrollOnMouseMove){
		#define SCROLL_SENSITIVITY 4 // Higher values will make the scroll more sensitive

		scroll((int)(scrollDirection.x * cos(getRotation()) + scrollDirection.y * sin(getRotation())) * SCROLL_SENSITIVITY, 
			   (int)(scrollDirection.y * cos(getRotation()) + scrollDirection.x * -sin(getRotation())) * SCROLL_SENSITIVITY);

		// Do not allow transforms to be done while scrolling
		blockTransforms();
	}
}

/** When set to true, a fullscreen window can be pinched back to windowed mode by pinching
 * near the corners of the screen (pinching in the middle will cause no resizing when in fullscreen)
 * When set to false, the only way to bring an application back to windowed mode is by system call */
void Window::setPinchableOutOfFullscreen(bool flag){
	pinchableOutOfFullscreen = flag;
}

void Window::setFullscreen(bool flag){
	// Going into fullscreen?
	if (!fullscreen && flag){

		// Save dimensions to later restore to normal
		widthBeforeFullscreen = webView->getTextureWidth();
		heightBeforeFullscreen = webView->getTextureHeight();

		setPosition(Application::windowWidth / 2.0f, Application::windowHeight / 2.0f);
		setScale(sf::Vector2f(1.0f, 1.0f));

		// Rotate to closest 90 degree angle
		WindowOrientation orientation = getOrientation();
		switch (orientation){
			case Left:
				setRotation(90.0f);
				break;
			case Right:
				setRotation(270.0f);
				break;
			case Top:
				setRotation(180.0f);
				break;
			case Bottom:
				setRotation(0.0f);
				break;
		}

		// Resize the sprite
		if (orientation == Top || orientation == Bottom){
			resizeSprite(Application::windowWidth, Application::windowHeight);
		}else{
			resizeSprite(Application::windowHeight, Application::windowWidth);
		}

		// Cannot drag a fullscreen window (doesn't matter what)
		draggableBeforeFullscreen = draggable;
		setDraggable(false);
		
		// Fullscreen windows should not be transparent (otherwise content from
		// the bottom windows might show up, yet the user cannot interact with it)
		transparentBeforeFullscreen = isTransparent();
		setTransparent(false);
		
		fullscreen = true;

		repaint();
	// Going out?
	}else if (fullscreen && !flag){
		if (widthBeforeFullscreen != 0 && heightBeforeFullscreen != 0){
			resizeSprite((float)widthBeforeFullscreen, (float)heightBeforeFullscreen);

			// Restore properties
			setDraggable(draggableBeforeFullscreen);
			setTransparent(transparentBeforeFullscreen);

			fullscreen = false;
		}else{
			cerr << "WARNING: Tried to exit fullscreen, but original dimensions are not available" << endl; 
		}

		repaint();
	}
}

bool Window::isFullscreen(){
	return fullscreen;
}

void Window::onStartLoading(){
	// Make sure that we have unlinked any active keyboard
	UIManager::getSingleton()->hideKeyboard(this);
}

/** Changes the color of the window and begins the drag */
void Window::startDragging(const sf::Vector2f &dragTouchPosition){
     if (!dragging && draggable){

#ifdef SMOOTH_DRAG
		 beginningDragTouchPosition = dragTouchPosition;
		 windowCenterOnDragBegin = getPosition();
#endif
		 // Color
		 setBlendColor(UIManager::getSingleton()->getDragColor());

         dragging = true;               
     }                        
}

/** Moves the window if the window is being dragged */
void Window::updateDragging(const sf::Vector2f &dragTouchPosition){
	if (dragging && draggable){
#ifdef SMOOTH_DRAG
		sf::Vector2f newPosition = windowCenterOnDragBegin + (dragTouchPosition - beginningDragTouchPosition);
		setPosition(newPosition);
#else
		setPosition(dragTouchPosition); // Temporary solution that doesn't lag, but no smooth movements
#endif
	}
}

/** Terminates the drag gesture */
void Window::stopDragging(const sf::Vector2f &dragTouchPosition){
     if (dragging && draggable){

		// Uncolor
		setBlendColor(sf::Color(255, 255, 255)); // No blend

        dragging = false;               
     }         
}

/** All vector3 are points in range 0..1 (as received from the gesture manager) */
void Window::startTransforming(const sf::Vector2f &centerLocation, float transformDistanceFromCenter, const sf::Vector2f &firstTouchLocation, const sf::Vector2f &secondTouchLocation){
	if (transformable){
		blockTransformsFlag = false;
		centerLocationOnTransformBegin = centerLocation;
		
		transformDistanceFromCenterOnTransformBegin = transformDistanceFromCenter;
		
		windowScaleOnTransformBegin = this->getScale();
		previousWindowScale = this->getScale();
		previousWindowRotation = this->getRotation() * DEGREES_TO_RADIANS;

		deltaScaleBigEnough = false;
		deltaRotationBigEnough = false;

		pinchedOutOfFullscreen = false;

		previousFirstTouchLocation = firstTouchLocationOnTransformBegin = firstTouchLocation;
		previousSecondTouchLocation = secondTouchLocationOnTransformBegin = secondTouchLocation;
	}
}


void Window::stopTransforming(){
	// Have we reached full screen threshold?
	if (transformable && !fullscreen && !pinchedOutOfFullscreen){
		const float fullscreenThreshold = 0.9f;
		WindowOrientation orientation = getOrientation();
		float sizeX = this->getScale().x * webView->getTextureWidth();
		float sizeY = this->getScale().y * webView->getTextureHeight();
		if (((orientation == Top || orientation == Bottom) && 
			(sizeX >= Application::windowWidth * fullscreenThreshold || sizeY >= Application::windowHeight * fullscreenThreshold))
			||
			((orientation == Left || orientation == Right) && 
			(sizeX >= Application::windowHeight * fullscreenThreshold || sizeY >= Application::windowWidth * fullscreenThreshold))
			){
			// Yes

			UIManager::getSingleton()->onWindowEnterFullscreenRequested(this);
		}
	}
}

/** Handles the transformations on this window. This includes scaling and rotating.
 */
void Window::updateTransform(const sf::Vector2f &centerLocation, float transformDistanceFromCenter, const sf::Vector2f &firstTouchLocation, 
							const sf::Vector2f &secondTouchLocation){
	if (blockTransformsFlag || !transformable) return;
	if (fullscreen && !pinchableOutOfFullscreen) return;

	// In fullscreen, only pinching one of the corners of the window will allow resizing
	if (fullscreen){
		#define CORNER_SIZE 0.10f

		if (!(Application::isPointOnScreenCorner(firstTouchLocationOnTransformBegin, CORNER_SIZE) ||
			  Application::isPointOnScreenCorner(secondTouchLocationOnTransformBegin, CORNER_SIZE))) return;
	}

	#define SCALE_SENSITIVITY 2
	#define DELTA_SCALE_THRESHOLD 0.010f

	float deltaScale = (transformDistanceFromCenter - transformDistanceFromCenterOnTransformBegin) * SCALE_SENSITIVITY;

	if (fabs(deltaScale) > DELTA_SCALE_THRESHOLD && !deltaScaleBigEnough){
		deltaScaleBigEnough = true;
		
		// Setting this value avoids sudden scaling at the beginning
		previousWindowScale = getScale();
	}

	// Don't start scaling if delta is not big enough (the user could be doing another gesture)
	// Also don't scale if the scale is too big (it could have been a sudden bad input from TUIO)
	if (deltaScaleBigEnough){

		// If we are in fullscreen, before resizing we need to get back to windowed mode
		if (fullscreen){
			UIManager::getSingleton()->onWindowExitFullscreenRequested(this);
			pinchedOutOfFullscreen = true;
		}
		
		this->setScale(sf::Vector2f(previousWindowScale.x + deltaScale, 
							previousWindowScale.y + deltaScale));
	}

	// Handle rotation
	#define DELTA_ROTATION_DEGREES_THRESHOLD 2.5f
	#define ROTATION_SENSIBILITY 1

	// Cannot rotate in fullscreen mode
	if (fullscreen) return;

	sf::Vector2f previousVec1 = previousFirstTouchLocation - centerLocation;
	sf::Vector2f currentVec1 = firstTouchLocation - centerLocation;

	sf::Vector2f previousVec2 = previousSecondTouchLocation - centerLocation;
	sf::Vector2f currentVec2 = secondTouchLocation - centerLocation;

	// These thetas do not give us the sign (+ or -), only an absolute value

	Radians firstTheta = angleBetween(previousVec1, currentVec1);
	Radians secondTheta = angleBetween(previousVec2, currentVec2);
	
	// Use the maximum angle to do the rotation between the two touches
	Radians deltaTheta;
	float cross; // calculating the cross product allows us to decide the sign of the angle
	if (firstTheta > secondTheta){
		deltaTheta = firstTheta;
		cross = crossProduct(previousVec1, currentVec1);
	}else{
		deltaTheta = secondTheta;
		cross = crossProduct(previousVec2, currentVec2);	
	}

	if (deltaTheta * RADIANS_TO_DEGREES > DELTA_ROTATION_DEGREES_THRESHOLD && !deltaRotationBigEnough){
		deltaRotationBigEnough = true;

		// Setting this value avoids sudden rotating at the beginning
		previousWindowRotation = getRotation() * DEGREES_TO_RADIANS;
	}

	if (deltaRotationBigEnough){
		// Flip the sign if needed
		if (cross < 0.0f){
			deltaTheta = -deltaTheta;
		}

		Radians rotation = deltaTheta * ROTATION_SENSIBILITY + previousWindowRotation;
		this->setRotation(rotation * RADIANS_TO_DEGREES);

		previousFirstTouchLocation = firstTouchLocation;
		previousSecondTouchLocation = secondTouchLocation;
		previousWindowRotation = rotation;
	}
}


void Window::blockTransforms(){
	blockTransformsFlag = true;
}

std::vector<std::wstring> Window::getJavascriptBindings(){
	std::vector<std::wstring> result;
	result.push_back(L"_GLAOpenNewWindow");
	result.push_back(L"_GLACloseWindow");
	result.push_back(L"_GLANotifyZoomChanged");
	result.push_back(L"_GLASetTuioEnabled");
	result.push_back(L"_GLASetTransparent");
	result.push_back(L"_GLAEnterFullscreen");
	result.push_back(L"_GLAExitFullscreen");
	result.push_back(L"_GLAShowKeyboard");
	result.push_back(L"_GLAHideKeyboard");
	result.push_back(L"_GLAOpenNewWebBrowserWindow");
	result.push_back(L"_GLALog");
	result.push_back(L"_GLAGoBack");
	result.push_back(L"_GLAGoForward");
	result.push_back(L"_GLANotifyDomLoaded");
	result.push_back(L"_GLASetScrollOnTouchMove");
	result.push_back(L"_GLASetTransformable");
	result.push_back(L"_GLASetDraggable");
	result.push_back(L"_GLASetPinchableOutOfFullscreen");	
	result.push_back(L"_GLAShowScreensaver");
	return result;
}

void Window::onJavascriptCallback(std::wstring functionName, std::vector<std::string> params){
	if (functionName == L"_GLAOpenNewWindow"){
		if (params.size() == 1){
			UIManager::getSingleton()->onNewWindowRequested(params[0], this, User);
		}
    }else if (functionName == L"_GLACloseWindow"){
        UIManager::getSingleton()->onCloseWindowRequested(this);
	}else if (functionName == L"_GLASetTuioEnabled"){
        if (params.size() == 1){
			int enabled = abs(str_to_int(params[0].c_str()));
			setTuioEnabled(enabled == 1);
		}
	}else if (functionName == L"_GLASetTransparent"){
        if (params.size() == 1){
			int flag = abs(str_to_int(params[0].c_str()));
			setTransparent(flag == 1);
		}
	}else if (functionName == L"_GLASetScrollOnTouchMove"){
        if (params.size() == 1){
			int flag = abs(str_to_int(params[0].c_str()));
			setScrollOnMouseMove(flag == 1);
		}
	}else if (functionName == L"_GLASetDraggable"){
        if (params.size() == 1){
			int flag = abs(str_to_int(params[0].c_str()));
			setDraggable(flag == 1);
		}
	}else if (functionName == L"_GLASetPinchableOutOfFullscreen"){
		if (params.size() == 1){
			int flag = abs(str_to_int(params[0].c_str()));
			setPinchableOutOfFullscreen(flag == 1);
		}
	}else if (functionName == L"_GLASetTransformable"){
        if (params.size() == 1){
			int flag = abs(str_to_int(params[0].c_str()));
			setTransformable(flag == 1);
		}
	}else if (functionName == L"_GLAShowScreensaver"){
		UIManager::getSingleton()->showScreensaver();
	}else if (functionName == L"_GLAEnterFullscreen"){
		UIManager::getSingleton()->onWindowEnterFullscreenRequested(this);
	}else if (functionName == L"_GLAExitFullscreen"){
		UIManager::getSingleton()->onWindowAnimatedExitFullscreenRequested(this);		
	}else if (functionName == L"_GLAShowKeyboard"){
		UIManager::getSingleton()->showKeyboard(this);
	}else if (functionName == L"_GLAHideKeyboard"){
		UIManager::getSingleton()->hideKeyboard(this);
	}else if (functionName == L"_GLAOpenNewWebBrowserWindow"){
		UIManager::getSingleton()->onNewWindowRequested(params[0], this, Browser);
	}else if (functionName == L"_GLAGoBack"){
		webView->goBack();
	}else if (functionName == L"_GLAGoForward"){
		webView->goForward();
	}else if (functionName == L"_GLANotifyDomLoaded"){
		webView->notifyDomLoaded();
		onDOMLoaded();
	}else if (functionName == L"_GLANotifyZoomChanged"){
		if (params.size() == 1){
			currentZoom = str_to_float(params[0].c_str());
		}
	}else if (functionName == L"_GLALog"){
		if (params.size() == 1){
			cout << params[0] << endl;
		}
    }else{
        std::wcout << "Unregistered callback for " << functionName;
    }
}

void Window::onDOMLoaded(){
	// Make sure we set the zoom to the current level
	// it gets lost at every page load (since it's CSS based)
	setZoom(currentZoom);

	// Crash information to send?
	if (crashed){
		// Push information via JS
		stringstream ss;
		map<std::string, std::string>::iterator it;
		for (it = crashReport.begin(); it != crashReport.end(); it++){
			ss << "GLA._addCrashInformation('" << it->first << "', '" << it->second << "');";
		}

		webView->executeJavascript(ss.str());
		ss.flush();

		ss << "GLA._notifyCrashInformationChanged();";
		webView->executeJavascript(ss.str());
	}
}

/** Copies the settings of the current window over to otherWindow
 * (ex. is it transparent? scroll on mouse move? zoom level? etc) */
void Window::copyAttributesTo(Window *otherWindow){
	otherWindow->setDraggable(this->draggable);
	otherWindow->setScrollable(this->scrollable);
	otherWindow->setTransformable(this->transformable);
	otherWindow->setTransparent(this->isTransparent());
	otherWindow->setZoom(this->currentZoom);
	otherWindow->setScrollOnMouseMove(this->scrollOnMouseMove);
	otherWindow->setFullscreen(this->fullscreen);
	otherWindow->setPinchableOutOfFullscreen(this->pinchableOutOfFullscreen);
	otherWindow->setTuioEnabled(this->tuioEnabled);
}

sf::Uint8 Window::getAlpha(){
	return sprite->getColor().a;
}

void Window::setAlpha(sf::Uint8 alpha){
	sf::Color color = sprite->getColor();
	color.a = alpha;
	sprite->setColor(color);
}

/** Compares the rotation of the current window and otherWindow within a defined threshold
 @param otherWindow the window to compare to
 @param threshold amount in degrees where the comparison is true
 @return true when the rotation of the two windows are the same within a certain threshold */
bool Window::rotationSameAs(Window *otherWindow, Degrees threshold){
	Degrees currentRotation = this->getNormalizedRotation();
	Degrees otherRotation = otherWindow->getNormalizedRotation();

	Degrees bigger = max(currentRotation, otherRotation);
	Degrees smaller = min(currentRotation, otherRotation);

	return (bigger - smaller <= threshold ||
		fabs(smaller - (bigger - 360.0f)) <= threshold);
}

void Window::show(){
	sf::Color currentColor = sprite->getColor();
	currentColor.a = 255;
	sprite->setColor(currentColor);
}

void Window::hide(){
	sf::Color currentColor = sprite->getColor();
	currentColor.a = 0;
	sprite->setColor(currentColor);
}

bool Window::isVisible(){
	sf::Color currentColor = sprite->getColor();
	return currentColor.a > 0;
}

void Window::loadURL(const string &url){
	webView->loadURI(url);
}

void Window::loadFile(const string &file){
	loadURL("file://" + FileManager::getSingleton()->getCurrentWorkingDirectory() + file);
}

void Window::loadInternal(const string &file){
	loadURL("chrome://" + file);
}

void Window::reload(){
	webView->reload();
}

void Window::translate(float screen_delta_x, float screen_delta_y){
	sprite->move(screen_delta_x, screen_delta_y);
}

void Window::setPosition(const sf::Vector2f &screenPosition){
	sprite->setPosition(screenPosition);
}

void Window::setPosition(float screen_x, float screen_y){
	setPosition(sf::Vector2f(screen_x, screen_y));
}

/** @return screen position coordinates of the window */
sf::Vector2f Window::getPosition() const{
	return sprite->getPosition();
}

void Window::rotate(Degrees angle){
	sprite->rotate(angle);
}

void Window::scale(float scale){
	sprite->scale(scale, scale);
}

void Window::scale(float scale_x, float scale_y){
	sprite->scale(scale_x, scale_y);
}

void Window::setScale(const sf::Vector2f &scale){
	sprite->setScale(scale);
}

void Window::setRotation(Degrees angle){
	sprite->setRotation(angle);
}

/** Modifies the Z axis to define the order of the window
 * a window with higher Z value will be rendered on top of windows with lower Z */
void Window::setZOrder(int z){
	zOrder = z;
}

/** Retrieve the Z order of the window */
int Window::getZOrder(){
	return zOrder;
}

/** Whether this window's Z-order should not be changed dynamically
 * during user interaction. System windows, keyboards, etc. for example should not have their
 * Z-order changed during user interaction */
bool Window::isZStatic(){
	return (!(getType() == User || getType() == Browser)) || fullscreen;
}

Degrees Window::getRotation() const{
	return sprite->getRotation();
}

/** @return the rotation of this window within a (0 .. 360) range */
Degrees Window::getNormalizedRotation() const{
	Degrees rotation = getRotation();
	if (fabs(rotation) >= 360){
		rotation = rotation - (360.0f * (float)((int)rotation / 360));
	}
	if (rotation < 0) rotation += 360.0f;

	return rotation;
}

/** Normalizes the rotation of this window (for example if the rotation is set at 370.0f degrees,
 * after this call the window's rotation will be 10.0f) */
void Window::normalizeRotation(){
	this->setRotation(this->getNormalizedRotation());
}

/** @return the orientation of the window (if it's facing the user to the left, right, top or bottom) */
WindowOrientation Window::getOrientation() const{
	Degrees rotation = getNormalizedRotation();
	if (rotation <= 45.0f || rotation >= 315.0f){
		return Bottom;
	}else if (rotation > 45.0f && rotation <= 135.0f){
		return Left;
	}else if (rotation > 135.0f && rotation < 225.0f){
		return Top;
	}else{
		return Right;
	}
}

sf::Vector2f Window::getScale() const{
	return sprite->getScale();
}

/** @return the width of the window in screen coordinates */
float Window::getWidth() const{
	return webView->getTextureWidth() * getScale().x;
}

/** @return the height of the window in screen coordinates */
float Window::getHeight() const{
	return webView->getTextureHeight() * getScale().y;
}

/** @return the normalized width (0..1) of the window as passed to the constructor */
float Window::getNormalizedWidth() const{
	return normalizedWidth;
}

/** @return the normalized height (0..1) of the window as passed to the constructor */
float Window::getNormalizedHeight() const{
	return normalizedHeight;
}

/** @return the width of the window's texture before any scaling */
float Window::getTextureWidth() const{
	return (float)webView->getTextureWidth();
}

/** @return the height of the window's texture before any scaling */
float Window::getTextureHeight() const{
	return (float)webView->getTextureHeight();
}

/** Resize the window's sprite to the specified size in pixels
 * this is not the same as scaling, it will destroy the current texture and create a new one
 * this should be used only when modifying the dimensions of the window (not the scale) or when
 * switching to full screen */
void Window::resizeSprite(float width, float height){
	webView->resize((int)width, (int)height);

	sprite->setTexture(*webView->getTexture(), true);
	sprite->setOrigin(webView->getTextureWidth() / 2.0f, webView->getTextureHeight() / 2.0f);
}

/** Turns transparency on or off. When transparent, every browser page
 * that has an element color set as "transparent" will be rendered with whatever
 * is behind the window */
void Window::setTransparent(bool transparent){
	webView->setTransparent(transparent);
}

bool Window::isTransparent(){
	return webView->isTransparent();
}

sf::Vector2f Window::getPointRotatedAround(Radians angle, const sf::Vector2f &pivot, const sf::Vector2f &point){
	return sf::Vector2f(pivot.x + (point.x - pivot.x) * cos(angle) - (point.y - pivot.y) * sin(angle),
				   pivot.y + (point.y - pivot.y) * cos(angle) + (point.x - pivot.x) * sin(angle));
}


/** Rotates global screen coordinates to align them with the provided windowRect (which is never rotated)
 * @param screen_x, screen_y: global screen coordinates (as received from the input listener) */
sf::Vector2f Window::getRotatedScreenCoords(pt::Rectangle &windowRect, float screen_x, float screen_y){
	sf::Vector2f screenCoords(screen_x, screen_y);

	// We take the global screen coordinates and "rotate them back" to align with our node (thus the - sign)
	Radians angle = -getRotation() * DEGREES_TO_RADIANS;
	sf::Vector2f center = windowRect.getCenter();
	return getPointRotatedAround(angle, center, screenCoords);
}

/** Takes in rotated screen coordinates and applies transformations and scaling to fit the content
  * of the WebView */
sf::Vector2f Window::transformCoordinatesForWebView(pt::Rectangle &windowRect, const sf::Vector2f &rotatedScreenCoords){
	sf::Vector2f webviewCoords(rotatedScreenCoords.x - windowRect.left,
						  rotatedScreenCoords.y - windowRect.top);

	// Scale
	float transformX = 1.0f / sprite->getScale().x;
	float transformY = 1.0f / sprite->getScale().y;

	webviewCoords.x *= transformX;
	webviewCoords.y *= transformY;

	float screenToTextureWidthRatio = webView->getTextureWidth() / ((windowRect.getWidth()) * transformX);
	float screenToTextureHeightRatio = webView->getTextureHeight() / ((windowRect.getHeight()) * transformY);
	
	// Scale to the proper texture range
	webviewCoords.x *= screenToTextureWidthRatio;
	webviewCoords.y *= screenToTextureHeightRatio;

	return webviewCoords;
}

/** Evaluates whether the global screen coordinates (all of them) fit inside the window */
bool Window::coordsInsideWindow(sf::Vector2f screenCoords[], int numCoords){
	pt::Rectangle tempWindowRect = getClientRectangle();

    for(int i = 0; i < numCoords; i++){
		sf::Vector2f rotatedScreenCoords = getRotatedScreenCoords(tempWindowRect, screenCoords[i].x, screenCoords[i].y);

		// Are the coordinates outside the window rect?
		if (!tempWindowRect.inside((int)rotatedScreenCoords.x, (int)rotatedScreenCoords.y)){
			return false;
		}
    }

    return true; // All of the coords were inside the window
}

/** Evaluates whether the global screen coordinates fit inside the current window.
 * When returning true, it also returns the local webview coordinates */
bool Window::coordsInsideWindow(float screen_x, float screen_y, sf::Vector2f &webviewCoords){
	pt::Rectangle rect;
	return coordsInsideWindow(screen_x, screen_y, rect, webviewCoords);
}

/** Evaluates whether the global screen coordinates fit inside the current window. 
 * When returning true, it also calculates and fills in the windowRect and webviewCoords params */
bool Window::coordsInsideWindow(float screen_x, float screen_y, pt::Rectangle &windowRect, sf::Vector2f &webviewCoords){
	
	pt::Rectangle tempWindowRect = getClientRectangle();
	sf::Vector2f rotatedScreenCoords = getRotatedScreenCoords(tempWindowRect, screen_x, screen_y);

	// Are the coordinates inside the window rect?
	if (tempWindowRect.inside((int)rotatedScreenCoords.x, (int)rotatedScreenCoords.y)){
		// Fill output params and return true

		webviewCoords = transformCoordinatesForWebView(tempWindowRect, rotatedScreenCoords);
		windowRect = tempWindowRect;
		return true;
	}else{
		return false;
	}
}

/** Public method (hides some of the other params) */
bool Window::coordsInsideWindow(float screen_x, float screen_y){
	pt::Rectangle windowRect;
	sf::Vector2f webviewCoords;
	return coordsInsideWindow(screen_x, screen_y, windowRect, webviewCoords);
}

/** Converts the given screen coordinates into webview coordinates */
sf::Vector2f Window::screenToWebViewCoords(const sf::Vector2f &screenCoords){
	pt::Rectangle tempWindowRect = getClientRectangle();
	sf::Vector2f rotatedScreenCoords = getRotatedScreenCoords(tempWindowRect, screenCoords.x, screenCoords.y);
	return transformCoordinatesForWebView(tempWindowRect, rotatedScreenCoords);
}

/** scrolls this window by a dx and dy amount
 * @param dx,dy the amount scrolled horizontally and vertically */
void Window::scroll(int dx, int dy){
	webView->injectScroll(dx, dy);
}

/** Handle a mouse down event
 @param x,y are the absolute screen coordinates */
void Window::onMouseDown(int cursor_id, int screen_x, int screen_y){
	pt::Rectangle windowRect;
	sf::Vector2f webviewCoords;

	// Was the click inside the window?
	if (coordsInsideWindow((float)screen_x, (float)screen_y, windowRect, webviewCoords)){
		handleMouseDown(cursor_id, webviewCoords);
	}
}


/** Handles a mouse move event
 @param x,y are the absolute screen coordinates */
void Window::onMouseMove(int cursor_id, int screen_x, int screen_y){
	pt::Rectangle windowRect;
	sf::Vector2f webviewCoords;

	// inside the window?
	if (coordsInsideWindow((float)screen_x, (float)screen_y, windowRect, webviewCoords)){
		handleMouseMove(cursor_id, webviewCoords);
	}
}

/** Checks for a mouse up event
 @param x,y are the absolute screen coordinates */
void Window::onMouseUp(int cursor_id, int screen_x, int screen_y){
	pt::Rectangle windowRect;
	sf::Vector2f webviewCoords;

	// Was the click inside the window?
	if (coordsInsideWindow((float)screen_x, (float)screen_y, windowRect, webviewCoords)){
		handleMouseUp(cursor_id, webviewCoords);
	}
}

/** Handles a mouse down event */
void Window::handleMouseDown(int cursor_id, const sf::Vector2f &webviewCoords){
	mouseDown = true;

	// If we are going to scroll, just record the position of the mouse down
	// but don't fire the event yet. We will fire it only if a scroll is NOT detected
	if (scrollOnMouseMove){
		scrolledOnMouseMove = false;
		lastMouseMoveLocation = lastMouseDownLocation = webviewCoords;
	}else{

		// Normal behavior here
		webView->injectMouseDown((int)webviewCoords.x, (int)webviewCoords.y);
	}
}

/** Handles a mouse up event */
void Window::handleMouseUp(int cursor_id, const sf::Vector2f &webviewCoords){
	mouseDown = false;

	if (scrollOnMouseMove){
		if (!scrolledOnMouseMove){
			// Fire up both a mouse down and a mouse up (because we didn't fire
			// mouse down)
			webView->injectMouseDown((int)webviewCoords.x, (int)webviewCoords.y);
			webView->injectMouseUp((int)webviewCoords.x, (int)webviewCoords.y);
		}
	}else{

		// Always inject
		webView->injectMouseUp((int)webviewCoords.x, (int)webviewCoords.y);
	}
}

/** Handles a mouse move event */
void Window::handleMouseMove(int cursor_id, const sf::Vector2f &webviewCoords){
	// Override move and do scrolling instead?
	if (scrollOnMouseMove){
		if (mouseDown){
			// How much have we scrolled?
			sf::Vector2f delta = webviewCoords - lastMouseMoveLocation;

			#define SCROLL_THRESHOLD 5.0f
			if (fabs(delta.x) > SCROLL_THRESHOLD || fabs(delta.y) > SCROLL_THRESHOLD){
				scrolledOnMouseMove = true;

				scroll((int)delta.x, (int)delta.y);

				//Update
				lastMouseMoveLocation = webviewCoords;
			}
		}
	}else{
		webView->injectMouseMove((int)webviewCoords.x, (int)webviewCoords.y);
	}
}


void Window::onKeyDown(int modifiers, int vkCode){
	webView->injectKeyDown(modifiers, vkCode, 0);
}

void Window::onKeyUp(int modifiers, int vkCode){
	webView->injectKeyUp(modifiers, vkCode, 0);
}

/** Injects a text event into the webview 
 * use this method when injecting user input */
void Window::injectText(const std::string &utf8){
	webView->injectTextEvent(utf8);
}

/** Returns the rectangle of the window (screen coordinates) */
pt::Rectangle Window::getRectangle(){
	return getClientRectangle(); // In the future if you add a border, add it to the computation here
}

/** Returns the rectangle of the content of the window (screen coordinates)
 * this rectangle is NOT rotated (it doesn't reflect the actual position on the screen), 
 * but it is scaled and translated. */
pt::Rectangle Window::getClientRectangle(){
	sf::FloatRect localBox = sprite->getLocalBounds();

	float halfWidth = localBox.width / 2.0f;
	float halfHeight = localBox.height / 2.0f;

	sf::Vector2f topLeftCorner = sf::Vector2f(-halfWidth, -halfHeight);
	sf::Vector2f bottomRightCorner = sf::Vector2f(halfWidth, halfHeight);

	// Scale + translate
	sf::Vector2f scaleVector = sprite->getScale();

	topLeftCorner.x *= scaleVector.x;
	topLeftCorner.y *= scaleVector.y;

	bottomRightCorner.x *= scaleVector.x;
	bottomRightCorner.y *= scaleVector.y;

	topLeftCorner += sprite->getPosition();
	bottomRightCorner += sprite->getPosition();

	// LOG("Window left (X): " << screenTopLeft.x);
	// LOG("Window top (Y): " << screenTopLeft.y);
	// LOG("Window right (X): " << screenBottomRight.x);
	// LOG("Window bottom (Y): " << screenBottomRight.y);

	return pt::Rectangle(topLeftCorner.x, topLeftCorner.y, bottomRightCorner.x, bottomRightCorner.y);
}

/** When set to true, a mouse move event will translate into a scroll event */
void Window::setScrollOnMouseMove(bool flag){
	scrollOnMouseMove = flag;
}

/** Executes the javascript code in the current browser window */
void Window::executeJavascript(const string &code){
	webView->executeJavascript(code);
}

/** When set to true, this window will call the javascript callbacks GLATouchDownEvent, GLATouchMoveEvent and GLATouchUpEvent
 * anytime it receives a mousedown, mousemove and mouseup event. This is useful when the developer of a 
 * web app wants to use multi touch features */
void Window::setTuioEnabled(bool flag){
	tuioEnabled = flag;
}

/** Handles the firing of TUIO events on the javascript side. 
 * @param name the name of event to fire ("touchstart", "touchmove" or "touchend") 
 * @param group the touch group associated with the cursor that first raised this event
 * @param blob_id the identifier of the blob that raised the event 
 * @param webviewCoords the already translated page coordinates for the cursor that raised this event */
void Window::fireJsTuioEvent(const string &name, TouchGroup *group, int blob_id, const sf::Vector2f &webviewCoords){
	if (tuioEnabled){

		// Do we have a touch group?
		if (group != NULL){
			// Build a list of touch objects in Javascript
			stringstream oss;
			oss << "GLA._fireTouchEvents('" << name << "' , [";

			for (int i = 0; i < group->getSize(); i++){
				Blob *touch = group->getTouch(i);

				// Params to be sent to JS
				float x, y;
				int id;
				float radiusX = 0.0f, radiusY = 0.0f;
				Degrees angle = 0.0f;

				// We have already computed the coordinates for this touch
				if (touch->id == blob_id){
					id = blob_id;
					x = webviewCoords.x;
					y = webviewCoords.y;
				}else{
					// Need to compute the webview coordinates for this touch
					sf::Vector2f coords(touch->screenX, touch->screenY);
					coords = screenToWebViewCoords(coords);

					id = touch->id;
					x = coords.x;
					y = coords.y;
				}

				// Do we have geometry information?
				if (touch->width != 0.0f){
					// Transform
					radiusX = (touch->width / getScale().x * Application::windowWidth) / 2.0f;
					radiusY = (touch->height / getScale().y * Application::windowHeight) / 2.0f;
					angle = touch->angle + getRotation();
				}

				oss << "{identifier:" << id << ",pageX:" << x << ",pageY:" << y << ",radiusX:" << radiusX << ",radiusY:" << radiusY << ",rotationAngle:" << angle << "},";

				webView->executeJavascript(oss.str());
			}
			
			// End touches JS list
			oss << "]);";
			webView->executeJavascript(oss.str());
		}else{
			// Nop, just evaluate the coordinates that fired the event
			stringstream oss;
			oss << "GLA._fireTouchEvents('" << name << "' , [{identifier:" << blob_id << ",pageX:" << webviewCoords.x << ",pageY:" << webviewCoords.y << ",radiusX:0,radiusY:0,rotationAngle:0}]);";
			webView->executeJavascript(oss.str());
		}
	}
}

/** Set the zoom of this window
 @param zoom magnification scale where 1.0 is equivalent to normal. */
void Window::setZoom(float zoom){
	stringstream ss;
	ss << "GLA.SetZoom(" << zoom << ");";
	webView->executeJavascript(ss.str());
	currentZoom = zoom;
}

/** Increments the zoom by one */
void Window::zoomIn(){
	webView->executeJavascript("GLA.ZoomIn();");
}

/** Decrements the zoom by one */
void Window::zoomOut(){
	webView->executeJavascript("GLA.ZoomOut();");
}

/** Compares the current webview reference and the one provided as an argument
 * @return true if equals */
bool Window::hasWebView(WebView *webView){
	return webView == this->webView;
}

/** Called whenever it's time to inject javascript resources on page load */
void Window::injectJavascriptResources(){

}

void Window::update(){

}

/** Pop/push functions allow for keeping track of changes in the position/rotation/scale of 
 * a window. If no items are present in the stack, the actual value is returned */
Degrees Window::popRotation(){
	if (rotationStack.empty()) return getRotation();
	else{
		Degrees result = rotationStack.top();
		rotationStack.pop();
		return result;
	}
}
sf::Vector2f Window::popScale(){
	if (scaleStack.empty()) return getScale();
	else{
		sf::Vector2f result = scaleStack.top();
		scaleStack.pop();
		return result;
	}
}
sf::Vector2f Window::popPosition(){
	if (positionStack.empty()) return getPosition();
	else{
		sf::Vector2f result = positionStack.top();
		scaleStack.pop();
		return result;
	}
}

void Window::pushRotation(){
	rotationStack.push(getRotation());
}

void Window::pushScale(){
	scaleStack.push(getScale());
}

void Window::pushPosition(){
	positionStack.push(getPosition());
}

/** Called after a crash occured and a new window has been created
 * @param description a brief description of what happened */
void Window::onCrash(const string &description){
	crashed = true;

	// Add crash report information
	crashReport["description"] = description;
	crashReport["URL"] = webView->getCurrentURL();

	// Load crash page
	this->loadURL("http://localhost:5555/system/crashpage");
}

/** Calls an API that forces the window content to be redrawn */
void Window::repaint(){
	executeJavascript("GLA._repaint();");
}

Window::~Window(){
	RELEASE_SAFELY(sprite);
	RELEASE_SAFELY(webView);
}

}