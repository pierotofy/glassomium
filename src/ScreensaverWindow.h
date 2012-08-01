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
   
#ifndef SCREENSAVERWINDOW_H
#define SCREENSAVERWINDOW_H

#include "stdafx.h"
#include "SystemWindow.h"
using namespace std;

namespace pt{

/** This class displays a user keyboard for input */
class ScreensaverWindow : public SystemWindow {
public:
	ScreensaverWindow(float normalizedWidth, float normalizedHeight);
    virtual ~ScreensaverWindow();

	virtual std::vector<std::wstring> getJavascriptBindings();
	virtual void onJavascriptCallback(std::wstring functionName, std::vector<std::string> params);

	virtual WindowType getType(){ return Keyboard; }
protected:

private:
	Window *linkedWindow; // When set to something != NULL, the keyboard is owned by this window and is not available
};

}

#endif
