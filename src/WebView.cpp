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

/** WebView.cpp
* 
* Some parts of this class use code adapted from the Berkelium GLUT examples */

#include "Application.h"
#include "WebView.h"
#include "FileManager.h"
#include "Utils.h"
#include "Globals.h"
#include "Window.h"

#include <SFML/OpenGL.hpp>

namespace pt{

unsigned int WebView::webViewCount = 0;

/** Creates a new web view
 * @param windowRatio this value is used to calculate the dimensions of the window
	 (it takes the resolution of the application and creates a window with the specified ratio)*/
WebView::WebView(float normalizedWidth, float normalizedHeight, pt::Window *parent)
 : needs_full_refresh(true){
    this->parent = parent;
	this->cefWindow = NULL;
	this->oldTexture = NULL;

	updateTextureSize(normalizedWidth, normalizedHeight);

	// Keep track of the number of web views so that we generate unique entity names
	webViewId = WebView::webViewCount;
	WebView::webViewCount++; 

	// Allocate scroll buffer
	scroll_buffer = new char[textureWidth*(textureHeight+1)*4];

	// TEMP
	renderBuffer = new char[textureWidth*(textureHeight+1)*4];

	// Setup CEF
    CefWindowInfo info;
	info.SetTransparentPainting(true);
    info.SetAsOffScreen(NULL);
    CefBrowserSettings browserSettings;

    cefWindow = CefBrowser::CreateBrowserSync(info, static_cast<CefRefPtr<CefClient>>(this), "", browserSettings);

    // set default browser size
    cefWindow->SetSize(PET_VIEW, textureWidth, textureHeight);

	// Setup texture
	texture = new sf::Texture();
	texture->create(textureWidth, textureHeight);
	texture->setSmooth(true);
	transparent = false;

	domLoaded = false;
 }

void WebView::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer)
{
	if (parent->isSleeping()) return; // Do not paint if the parent is sleeping

	const int kBytesPerPixel = 4;

	// Convert BGRA to RGBA
	unsigned int *tmpBuf = (unsigned int *)buffer;
	const int numPixels = textureWidth * textureHeight;
	for (int i = 0; i < numPixels; i++){
		tmpBuf[i] = (tmpBuf[i] & 0xFF00FF00) | ((tmpBuf[i] & 0x00FF0000) >> 16) | ((tmpBuf[i] & 0x000000FF) << 16);
	}

	// If we've reloaded the page and need a full update, ignore updates
	// until a full one comes in. This handles out of date updates due to
	// delays in event processing.
	if (needs_full_refresh) {
		/* TODO: CHECK!
		// Ignore partial ones
		if (bitmap_rect.left() != 0 || bitmap_rect.top() != 0 || bitmap_rect.right() != textureWidth || bitmap_rect.bottom() != textureHeight) {
			return;
		}*/

		// Do we have an old texture? We must have just resized
		if (oldTexture != NULL){
			RELEASE_SAFELY(oldTexture);

			// Notify the parent
			parent->onResizeSpriteCompleted();
		}

		// Here's our full refresh
		texture->update((sf::Uint8 *)buffer, textureWidth, textureHeight, 0, 0);

		needs_full_refresh = false;
		return;
	}


	// Now, we first handle scrolling. We need to do this first since it
	// requires shifting existing data, some of which will be overwritten by
	// the regular dirty rect update.
	/*
	if (dx != 0 || dy != 0) {

		// scroll_rect contains the Rect we need to move
		// First we figure out where the the data is moved to by translating it
		Berkelium::Rect scrolled_rect = scroll_rect.translate(-dx, -dy);

		// Next we figure out where they intersect, giving the scrolled
		// region
		Berkelium::Rect scrolled_shared_rect = scroll_rect.intersect(scrolled_rect);

		// Only do scrolling if they have non-zero intersection
		if (scrolled_shared_rect.width() > 0 && scrolled_shared_rect.height() > 0) {

			// And the scroll is performed by moving shared_rect by (dx,dy)
			Berkelium::Rect shared_rect = scrolled_shared_rect.translate(dx, dy);

			int wid = scrolled_shared_rect.width();
			int hig = scrolled_shared_rect.height();

			int inc = 1;
			char *outputBuffer = scroll_buffer;
			// source data is offset by 1 line to prevent memcpy aliasing
			// In this case, it can happen if dy==0 and dx!=0.
			char *inputBuffer = scroll_buffer+(textureWidth*1*kBytesPerPixel);
			int jj = 0;
			if (dy > 0) {

				// Here, we need to shift the buffer around so that we start in the
				// extra row at the end, and then copy in reverse so that we
				// don't clobber source data before copying it.
				outputBuffer = scroll_buffer+((scrolled_shared_rect.top()+hig+1)*textureWidth - hig*wid)*kBytesPerPixel;
				inputBuffer = scroll_buffer;
				inc = -1;
				jj = hig-1;
			}

			// Copy the data out of the texture
			texture->bind(sf::Texture::Pixels);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, inputBuffer);
			
			// Annoyingly, OpenGL doesn't provide convenient primitives, so
			// we manually copy out the region to the beginning of the
			// buffer
			
			for(; jj < hig && jj >= 0; jj+=inc) {
				memcpy(
					outputBuffer + (jj*wid) * kBytesPerPixel,
					inputBuffer + ((scrolled_shared_rect.top()+jj)*textureWidth + scrolled_shared_rect.left()) * kBytesPerPixel,
					wid*kBytesPerPixel
				);
			}

			// And finally, we push it back into the texture in the right
			// location
			texture->update((sf::Uint8 *)outputBuffer, shared_rect.width(), shared_rect.height(), shared_rect.left(), shared_rect.top());
		}
	}*/

	for (size_t i = 0; i < dirtyRects.size(); i++) {
		int wid = dirtyRects[i].width;
		int hig = dirtyRects[i].height;
		int top = dirtyRects[i].y;// - dirtyRects.top();
		int left = dirtyRects[i].x;// - dirtyRects.left();

		for(int jj = 0; jj < hig; jj++) {
			memcpy(
				scroll_buffer + jj*wid*kBytesPerPixel,
				//buffer + (left + (jj+top)*bitmap_rect.width())*kBytesPerPixel,
				(char *)buffer + (left + (jj+top)*textureWidth)*kBytesPerPixel,
				wid*kBytesPerPixel
				);
		}

		// Finally, we perform the main update, just copying the rect that is
		// marked as dirty but not from scrolled data.
		texture->update((sf::Uint8 *)scroll_buffer, dirtyRects[i].width, dirtyRects[i].height, dirtyRects[i].x, dirtyRects[i].y);
	}

	needs_full_refresh = false;	
}

int WebView::getTextureWidth(){
	return textureWidth;
}

int WebView::getTextureHeight(){
	return textureHeight;
}

/** Resizes the webview to the specified width and height in pixels
 * it will create a new texture */
void WebView::resize(int width, int height){
	textureWidth = width;
	textureHeight = height;

	// Switch old texture with new one
	oldTexture = texture;

	texture = new sf::Texture();
	texture->create(textureWidth, textureHeight);
	texture->setSmooth(true);

	char *previous_buffer = scroll_buffer;
	scroll_buffer = new char[textureWidth*(textureHeight+1)*4];
	RELEASE_SAFELY(previous_buffer);

	needs_full_refresh = true;
	cefWindow->SetSize(PET_VIEW, width, height);
	
}

/** Set the transparent property */
void WebView::setTransparent(bool transparent){
	// TODO: how do we do this?

	this->transparent = transparent;
    //bkWindow->setTransparent(transparent);
}

bool WebView::isTransparent(){
	return transparent;
}

/** Update the texture size
 * textureWidth and textureHeight will change upon call of this method.
  */
void WebView::updateTextureSize(float normalizedWidth, float normalizedHeight){
    this->textureWidth = (int)ceil(Application::windowWidth * normalizedWidth);
    this->textureHeight = (int)ceil(Application::windowHeight * normalizedHeight);
}

/** Instruct berkelium to load a URI*/
void WebView::loadURI(const string &url){
	cefWindow->GetMainFrame()->LoadURL(url);
	currentURL = url;
}

/** Concat the name with our web view count to get a unique identifier
 */ 
string WebView::getUniqueIdentifier(const string &name){
	std::stringstream out;
	out << webViewId;
	return name + "_" + out.str();
}

/** @param dx,dy the amount scrolled vertically and horizontally */
void WebView::injectScroll(int dx, int dy)
{
	// TODO!!
	//bkWindow->mouseWheel(dx, dy);
	//cout << "injectScroll NOT IMPLEMENTED" << endl;
}

/* @param x,y are relative */
void WebView::injectMouseDown(int x, int y)
{
	cefWindow->SendMouseMoveEvent(x, y, false);
	cefWindow->SendMouseClickEvent(x, y, MBT_LEFT, false, 1);
}

/* @param x,y are relative */
void WebView::injectMouseUp(int x, int y)
{
	cefWindow->SendMouseMoveEvent(x, y, false);
	cefWindow->SendMouseClickEvent(x, y, MBT_LEFT, true, 1);
}

/* @param x,y are relative */
void WebView::injectMouseMove(int x, int y)
{
	cefWindow->SendMouseMoveEvent(x, y, false);
}

/** @param scancode the original scancode that generated the event
 *  @param mods    a modifier code created by a logical or of KeyModifiers 
 *  @param vkCode     the virtual key code received from the OS */
void WebView::injectKeyDown(int modifiers, int vkCode, int scancode){
    //bkWindow->keyEvent(true, modifiers, vkCode, scancode);
	//injectTextEvent(string(1, vkCode));

	// TODO: fix
	//cefWindow->SendKeyEvent(KT_KEYDOWN, vkCode, modifiers, false, false);
}

/** @param scancode the original scancode that generated the event
 *  @param mods    a modifier code created by a logical or of KeyModifiers 
 *  @param vkCode     the virtual key code received from the OS */
void WebView::injectKeyUp(int modifiers, int vkCode, int scancode){
	// TODO: FIx
	//cefWindow->SendKeyEvent(KT_KEYUP, vkCode, modifiers, false, false);
    //bkWindow->keyEvent(false, modifiers, vkCode, scancode);
}

void WebView::injectTextEvent(const std::string &utf8){
	// TODO: finish!
	/*
	// Convert from multibyte to wide character string
    wchar_t *buffer = new wchar_t[utf8.size() + 1];
    size_t length = mbstowcs(buffer, utf8.c_str(), utf8.size());
    bkWindow->textEvent(buffer, length);
    RELEASE_SAFELY(buffer);
	*/
}


void WebView::OnLoadStart( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame ){
	AutoLock lock_scope(this);
	if (frame->IsMain()){
		domLoaded = false;
		parent->onStartLoading();
		if (g_debug){
			cout << "on start loading " << browser->GetMainFrame()->GetURL().ToString() << endl;
		}
	}
}
/*
void WebView::onPaint(Berkelium::Window *wini,
    const unsigned char *bitmap_in, const Berkelium::Rect &bitmap_rect,
    size_t num_copy_rects, const Berkelium::Rect *copy_rects,
    int dx, int dy, const Berkelium::Rect &scroll_rect) {

	if (parent->isSleeping()) return; // Do not paint if the parent is sleeping

	const int kBytesPerPixel = 4;

	// Convert BGRA to RGBA
	
	unsigned int *tmpBuf = (unsigned int *)bitmap_in;
	const int numPixels = bitmap_rect.height() * bitmap_rect.width();
	for (int i = 0; i < numPixels; i++){
		tmpBuf[i] = (tmpBuf[i] & 0xFF00FF00) | ((tmpBuf[i] & 0x00FF0000) >> 16) | ((tmpBuf[i] & 0x000000FF) << 16);
	}

	// If we've reloaded the page and need a full update, ignore updates
	// until a full one comes in. This handles out of date updates due to
	// delays in event processing.
	if (needs_full_refresh) {
		// Ignore partial ones
		if (bitmap_rect.left() != 0 || bitmap_rect.top() != 0 || bitmap_rect.right() != textureWidth || bitmap_rect.bottom() != textureHeight) {
			return;
		}

		// Do we have an old texture? We must have just resized
		if (oldTexture != NULL){
			RELEASE_SAFELY(oldTexture);

			// Notify the parent
			parent->onResizeSpriteCompleted();
		}

		// Here's our full refresh
		texture->update((sf::Uint8 *)bitmap_in, textureWidth, textureHeight, 0, 0);

		needs_full_refresh = false;
		return;
	}


	// Now, we first handle scrolling. We need to do this first since it
	// requires shifting existing data, some of which will be overwritten by
	// the regular dirty rect update.
	if (dx != 0 || dy != 0) {

		// scroll_rect contains the Rect we need to move
		// First we figure out where the the data is moved to by translating it
		Berkelium::Rect scrolled_rect = scroll_rect.translate(-dx, -dy);

		// Next we figure out where they intersect, giving the scrolled
		// region
		Berkelium::Rect scrolled_shared_rect = scroll_rect.intersect(scrolled_rect);

		// Only do scrolling if they have non-zero intersection
		if (scrolled_shared_rect.width() > 0 && scrolled_shared_rect.height() > 0) {

			// And the scroll is performed by moving shared_rect by (dx,dy)
			Berkelium::Rect shared_rect = scrolled_shared_rect.translate(dx, dy);

			int wid = scrolled_shared_rect.width();
			int hig = scrolled_shared_rect.height();

			int inc = 1;
			char *outputBuffer = scroll_buffer;
			// source data is offset by 1 line to prevent memcpy aliasing
			// In this case, it can happen if dy==0 and dx!=0.
			char *inputBuffer = scroll_buffer+(textureWidth*1*kBytesPerPixel);
			int jj = 0;
			if (dy > 0) {

				// Here, we need to shift the buffer around so that we start in the
				// extra row at the end, and then copy in reverse so that we
				// don't clobber source data before copying it.
				outputBuffer = scroll_buffer+((scrolled_shared_rect.top()+hig+1)*textureWidth - hig*wid)*kBytesPerPixel;
				inputBuffer = scroll_buffer;
				inc = -1;
				jj = hig-1;
			}

			// Copy the data out of the texture
			texture->bind(sf::Texture::Pixels);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, inputBuffer);
			
			// Annoyingly, OpenGL doesn't provide convenient primitives, so
			// we manually copy out the region to the beginning of the
			// buffer
			
			for(; jj < hig && jj >= 0; jj+=inc) {
				memcpy(
					outputBuffer + (jj*wid) * kBytesPerPixel,
					inputBuffer + ((scrolled_shared_rect.top()+jj)*textureWidth + scrolled_shared_rect.left()) * kBytesPerPixel,
					wid*kBytesPerPixel
				);
			}

			// And finally, we push it back into the texture in the right
			// location
			texture->update((sf::Uint8 *)outputBuffer, shared_rect.width(), shared_rect.height(), shared_rect.left(), shared_rect.top());
		}
	}

	for (size_t i = 0; i < num_copy_rects; i++) {
		int wid = copy_rects[i].width();
		int hig = copy_rects[i].height();
		int top = copy_rects[i].top() - bitmap_rect.top();
		int left = copy_rects[i].left() - bitmap_rect.left();

		for(int jj = 0; jj < hig; jj++) {
			memcpy(
				scroll_buffer + jj*wid*kBytesPerPixel,
				bitmap_in + (left + (jj+top)*bitmap_rect.width())*kBytesPerPixel,
				wid*kBytesPerPixel
				);
		}

		// Finally, we perform the main update, just copying the rect that is
		// marked as dirty but not from scrolled data.
		texture->update((sf::Uint8 *)scroll_buffer, copy_rects[i].width(), copy_rects[i].height(), copy_rects[i].left(), copy_rects[i].top());
	}

	needs_full_refresh = false;
}*/

void WebView::OnAddressChange( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, const CefString& url ){
	AutoLock lock_scope(this);
	currentURL = url.ToString();
}

void WebView::OnLoadEnd( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, int httpStatusCode ){
	AutoLock lock_scope(this);
	if (frame->IsMain()){
		if (!domLoaded){
			cout << " On load end" << endl;
			processPostLoad();
		}
	}
}

/** Called from javascript whenever the DOM is ready */
void WebView::notifyDomLoaded(){ 
	domLoaded = true; 
}

/** This method must be called after every page load (including during history navigation) */
void WebView::processPostLoad(){
	// Inject javascript on load
	executeJavascriptFromFile("js/injectOnLoad.js");

	parent->injectJavascriptResources();  
}

/** Executes javascript code in the current window 
 * @param code ASCII javascript code (not multibyte), see see http://stackoverflow.com/questions/246806/i-want-to-convert-stdstring-into-a-const-wchar-t */
void WebView::executeJavascript(const string &code){
    std::wstring wide_code = std::wstring(code.begin(), code.end());

	CefRefPtr<CefFrame> frame = cefWindow->GetMainFrame();
	frame->ExecuteJavaScript(CefString(code), frame->GetURL(), 0);
}

/** Executes javascript code from a file in the current window */
void WebView::executeJavascriptFromFile(const string &file){
    wstring content = FileManager::getSingleton()->readAllWide(file);
	CefRefPtr<CefFrame> frame = cefWindow->GetMainFrame();
	frame->ExecuteJavaScript(CefString(content), frame->GetURL(), 0);
}

/** Goes back by one history item */
void WebView::goBack(){
	if (cefWindow->CanGoBack()){
		cefWindow->GoBack();
		processPostLoad();// TODO : do we need this?
	}
}

/** Goes forward by one history item */
void WebView::goForward(){
	if (cefWindow->CanGoForward()){
		cefWindow->GoForward();
		processPostLoad(); // TODO : do we need this?
	}
}

/** Prevents popups from showing */
bool WebView::OnBeforePopup( CefRefPtr< CefBrowser > parentBrowser, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, const CefString& url, CefRefPtr< CefClient >& client, CefBrowserSettings& settings ){
	AutoLock lock_scope(this);
	return true;
}

/** Reload the page */
void WebView::reload(){
	cefWindow->Reload();
}

/** Intercepts Javascript bindings (public JS API) */
bool WebView::Execute( const CefString& name, CefRefPtr< CefV8Value > object, const CefV8ValueList& arguments, CefRefPtr< CefV8Value >& retval, CefString& exception){
   AutoLock lock_scope(this);

	if (g_debug){
		cout << "Intercepted " << name.ToString() << endl;
	}

	std::vector<std::string> params;
	for (unsigned int i = 0; i < arguments.size(); i++){
		params.push_back(arguments.at(i).get()->GetStringValue().ToString());
	}

	parent->onJavascriptCallback(name.ToWString(), params);

    return true;
}

void WebView::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	AutoLock lock_scope(this);

	if (frame->IsMain()){
		if (g_debug){
			cout << "Binding JS API" << endl;
		}
		std::vector<std::wstring> bindings = parent->getJavascriptBindings();

		CefRefPtr<CefV8Value> object = context->GetGlobal();
		for (unsigned int i = 0; i < bindings.size(); i++){
			object->SetValue(bindings[i], CefV8Value::CreateFunction(bindings[i], this), V8_PROPERTY_ATTRIBUTE_NONE);
		}
	}
}

// TODO: adapt to CEF
/** Bug, doesn't seem to get fired by berkelium */
/** Intercept requests for new window creation, otherwise we won't be able display popups and company */
// void WebView::onNavigationRequested (Berkelium::Window *win, Berkelium::URLString newUrl, Berkelium::URLString referrer, bool isNewWindow, bool &cancelDefaultAction){
//     if (isNewWindow) {
//         UIManager::getSingleton()->onNewWindowRequested(string(newUrl.mData));
//         cancelDefaultAction = true;
//     }
// }

/** Ignore the next onPaint events until a complete one is received */
void WebView::forceFullRefresh(){
	needs_full_refresh = true;
}

/** Creates a new berkelium window and notifies the parent that we have crashed!
 * @param description a brief description of what happened. */
void WebView::handleCrash(const string &description){
	cerr << "CRASH: " << description << endl;
	/* TODO: FINISH!!!

	Berkelium::Window *previousWindow = bkWindow;
	Berkelium::Context *bk_context = Berkelium::Context::create();
	bkWindow = Berkelium::Window::create(bk_context);
	bkWindow->resize(textureWidth, textureHeight); 
	RELEASE_SAFELY(bk_context);
	bkWindow->setDelegate(this);
	RELEASE_SAFELY(previousWindow);
	*/
	parent->onCrash(description);
}

void WebView::OnBeforeClose(CefRefPtr<CefBrowser> browser){
   AutoLock lock_scope(this);

   // Free the browser pointer so that the browser can be destroyed
   cefWindow = NULL;

   // Webview will be disposed by CEF
}


/** Performs all the actions needed to get
 * this window ready for disposal */
void WebView::prepareForDisposal(){
	if (cefWindow){
		RELEASE_SAFELY(cefWindow);
	}
}

WebView::~WebView(){
	if (cefWindow){
		RELEASE_SAFELY(cefWindow);
	}

	RELEASE_SAFELY(texture);

	RELEASE_SAFELY(renderBuffer);
	
    WebView::webViewCount--;
}

}