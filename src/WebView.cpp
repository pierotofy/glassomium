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
WebView::WebView(float windowRatio, pt::Window *parent){
	this->windowRatio = windowRatio;
    this->parent = parent;

	// Calculate the optimal texture size
	calculateTextureSize(this->windowRatio, this->textureWidth, this->textureHeight);

	// Keep track of the number of web views so that we generate unique entity names
	webViewId = WebView::webViewCount;
	WebView::webViewCount++; 

	const int kNumPixels = textureWidth*(textureHeight+1)*4;

	// Allocate buffer
	renderBuffer = new char[kNumPixels];

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
	AutoLock lock_scope(this);
	const int kBytesPerPixel = 4;
	const int numPixels = textureHeight * textureWidth;

	// Copy buffer
	memcpy(renderBuffer, buffer, numPixels * kBytesPerPixel);

	// Convert BGRA to RGBA
	unsigned int *tmpBuf = (unsigned int *)renderBuffer;
	for (int i = 0; i < numPixels; i++){
		tmpBuf[i] = (tmpBuf[i] & 0xFF00FF00) | ((tmpBuf[i] & 0x00FF0000) >> 16) | ((tmpBuf[i] & 0x000000FF) << 16);
	}

	texture->update((sf::Uint8 *)renderBuffer, textureWidth, textureHeight, 0, 0);
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

	cefWindow->SetSize(PET_VIEW, width, height);
	
	// Switch old texture with new one
	sf::Texture *oldTexture = texture;

	texture = new sf::Texture();
	texture->create(textureWidth, textureHeight);
	texture->setSmooth(true);

	RELEASE_SAFELY(oldTexture);
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

/** We calculate the size depending on the size of the rendering window. If a person is going
 * to scale our webview, we need to make sure we have a texture big enough as not to lose quality
 * this has the drawback of wasting resources when the webview is smaller than the full size of the 
 * rendering window, but as long as we don't have too many webviews we should be OK */
void WebView::calculateTextureSize(float windowRatio, int &width, int &height){
    width = (int)ceil(Application::windowWidth);
    height = (int)ceil((float)width  / windowRatio);

    if (height > Application::windowHeight){
        height = (int)ceil(Application::windowHeight);
        width = (int)ceil((float)width * windowRatio);
    }
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
	//bkWindow->mouseWheel(dx, dy);
	cout << "injectScroll NOT IMPLEMENTED" << endl;
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

	cefWindow->SendKeyEvent(KT_KEYDOWN, vkCode, modifiers, false, false);
}

/** @param scancode the original scancode that generated the event
 *  @param mods    a modifier code created by a logical or of KeyModifiers 
 *  @param vkCode     the virtual key code received from the OS */
void WebView::injectKeyUp(int modifiers, int vkCode, int scancode){
	cefWindow->SendKeyEvent(KT_KEYUP, vkCode, modifiers, false, false);
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

/** Deallocates the webview 
 * Use this method instead of the destructor to deallocate a webview */
void WebView::release(){

	// TODO: this crashes when a page is still loading and it gets closed

	// This call is async and will return immediately
	// We will dispose the actual object when onBeforeClose is called
	cefWindow->CloseBrowser();
}

WebView::~WebView(){
	if (g_debug){
		cout << "Deallocated webview" << endl;
	}

	RELEASE_SAFELY(texture);

	RELEASE_SAFELY(renderBuffer);
	
    WebView::webViewCount--;
}

}