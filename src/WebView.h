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

#ifndef WEBVIEW_H
#define WEBVIEW_H

#include "stdafx.h"
#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_display_handler.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_request_handler.h"
#include "include/cef_render_handler.h"
#include "include/internal/cef_ptr.h"

namespace pt{

class Window;

using namespace std;
using namespace pt;

class WebView : public CefClient,
			    public CefDisplayHandler,
				public CefLoadHandler,
			    public CefLifeSpanHandler,
			    public CefRequestHandler,
			    public CefRenderHandler,
				public CefV8Handler,
				public Berkelium::WindowDelegate{
public:
	static unsigned int webViewCount;

	WebView(float windowRatio, Window *parent);
    ~WebView();

   virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
   virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE { return this; }
   virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE { return this; }
   virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }

    // CefLifeSpanHandler methods
   virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
   virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

   virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
      const RectList& dirtyRects, const void* buffer) OVERRIDE;

   virtual void OnLoadStart( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame ) OVERRIDE;
   virtual void OnLoadEnd( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, int httpStatusCode ) OVERRIDE;

   virtual void OnAddressChange( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, const CefString& url ) OVERRIDE;

	sf::Texture* getTexture(){ return texture; }
	void loadURI(const string&);

	int getTextureWidth();
	int getTextureHeight();

	void resize(int width, int height);

	// Input injectors
	void injectMouseDown(int x, int y);
	void injectMouseUp(int x, int y);
	void injectMouseMove(int x, int y);
	void injectKeyUp(int modifiers, int vk_code, int scancode);
	void injectKeyDown(int modifiers, int vk_code, int scancode);
	void injectTextEvent(const std::string &utf8);
	void injectScroll(int dx, int dy);

	// Javascript stuff
	void executeJavascript(const string &code);
	void executeJavascriptFromFile(const string &file);

	void goBack();
	void goForward();

	void reload();

	void adjustZoom(int mode);

	void setTransparent(bool);
	bool isTransparent();

	void processPostLoad();

	void notifyDomLoaded();
	std::string getCurrentURL(){ return currentURL; }
private:
	// The child browser window
   CefRefPtr<CefBrowser> cefWindow;

   // Include the default reference counting implementation.
   IMPLEMENT_REFCOUNTING(WebView);

   // Include the default locking implementation.
   IMPLEMENT_LOCKING(WebView);

	void bindJSAPI();

	// Reference to the parent object that hosts this webview
	Window *parent;

    // The Berkelium window, i.e. our web page
    Berkelium::Window* bkWindow;

	float windowRatio;

    // Width and height (in pixels) of our exture. These are also the sizes of the berkelium window
    int textureWidth, textureHeight;

	// The actual thing that gets rendered
	sf::Texture *texture;

	// Bool indicating when we need to refresh the entire texture
    bool needs_full_refresh;

    // Buffer used to store the image for the texture
    char* renderBuffer;

	std::string currentURL; // Keep track of the current URL

	// This variable is set to true via callback from javascript (berkelium's pageLoad is not accurate)
	bool domLoaded;

	bool transparent;

	// The color used to blend the texture with during shading
	//Ogre::Vector4 blendColor;

	// Unique for each existing web view
	unsigned int webViewId;

	// This method needs to be called after some of the material properties have changed
	void updateMaterial();

	void calculateTextureSize(float windowRatio, int &width, int &height);
	string getUniqueIdentifier(const string &);

	virtual void onJavascriptCallback(Berkelium::Window *win, void* replyMsg, Berkelium::URLString url, 
				Berkelium::WideString funcName, Berkelium::Script::Variant *args, size_t numArgs);
	
	void handleCrash(const string &description);
      
	// virtual void onNavigationRequested (Berkelium::Window *win, 
	// 		Berkelium::URLString newUrl, 
	// 		Berkelium::URLString referrer, 
	// 		bool isNewWindow, bool &cancelDefaultAction);

	virtual void onCrashedWorker (Berkelium::Window *win){
		handleCrash("A worker crashed!");
	}

	virtual void onCrashed (Berkelium::Window *win){
		handleCrash("Crashed!");
	}

	virtual void onUnresponsive (Berkelium::Window *win){
		handleCrash("Unresponsive window!");
	}

	virtual void onCrashedPlugin (Berkelium::Window *win, Berkelium::WideString pluginName){
		handleCrash("Plugin crashed!");
	}
};

}

#endif