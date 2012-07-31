/*
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

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

#include "UserWindow.h"
#include "Utils.h"
#include "Globals.h"

namespace pt{

UserWindow::UserWindow(float normalizedWidth, float normalizedHeight)
  : Window(normalizedWidth, normalizedHeight){
	  injectMenu = true;
}

/** Sets whether a window menu is going to be injected in this window */
void UserWindow::setInjectMenu(bool flag){
	injectMenu = flag;
}

void UserWindow::injectJavascriptResources(){
	if (injectMenu && !crashed){
		webView->executeJavascriptFromFile("js/windowMenu.js");
		if (g_debug){
			cout << "Injected menu!" << endl;
		}
	}
		
	Window::injectJavascriptResources();
}

UserWindow::~UserWindow(){

}

}
