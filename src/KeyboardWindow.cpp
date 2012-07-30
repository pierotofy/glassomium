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
   
#include "KeyboardWindow.h"
#include "Utils.h"
#include "Globals.h"

namespace pt{

KeyboardWindow::KeyboardWindow(float normalizedWidth, float normalizedHeight)
  : SystemWindow(normalizedWidth, normalizedHeight){
	hide();

	linkedWindow = NULL;
}

bool KeyboardWindow::available(){
	return linkedWindow == NULL;
}

void KeyboardWindow::linkTo(Window *window){
	linkedWindow = window;
}

void KeyboardWindow::unlink(){
	linkedWindow = NULL;
}

bool KeyboardWindow::isLinkedTo(Window *window){
	return linkedWindow == window;
}

std::vector<std::wstring> KeyboardWindow::getJavascriptBindings(){
	std::vector<std::wstring> result = SystemWindow::getJavascriptBindings();
	result.push_back(L"_GLAKeyboardOnKeyPressed");
	return result;
}


#define VKEY_CHAR(A,B) if (key == A) { vKey = B; }
void KeyboardWindow::onJavascriptCallback(std::wstring functionName, std::vector<std::string> params){
	if (functionName == L"_GLAKeyboardOnKeyPressed" && params.size() == 1){
		if (linkedWindow != NULL){
			string key = params[0];
			
			int vKey = 0;
			VKEY_CHAR("\\n", 0xD);
			VKEY_CHAR("\\b", '\b');

			if (vKey != 0){
				linkedWindow->onKeyDown(0, vKey);
			}else{
				linkedWindow->injectText(utf8unescape(key));
			}
		}

		return;
    }

	SystemWindow::onJavascriptCallback(functionName, params);
}

KeyboardWindow::~KeyboardWindow(){
	
}

}
