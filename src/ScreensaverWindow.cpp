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
   
#include "ScreensaverWindow.h"
#include "Utils.h"
#include "Globals.h"
#include "UIManager.h"

namespace pt{

ScreensaverWindow::ScreensaverWindow(float normalizedWidth, float normalizedHeight)
  : SystemWindow(normalizedWidth, normalizedHeight){
	setTuioEnabled(true);
	setFullscreen(true);

	loadURL("http://localhost:5555/system/screensaver");
}

std::vector<std::wstring> ScreensaverWindow::getJavascriptBindings(){
	std::vector<std::wstring> result = SystemWindow::getJavascriptBindings();
	result.push_back(L"_GLAExitScreensaver");
	return result;
}

void ScreensaverWindow::onJavascriptCallback(std::wstring functionName, std::vector<std::string> params){
	const int defaultAnimationTime = 1000;

	if (functionName == L"_GLAExitScreensaver"){
		if (params.size() >= 1){
			int animationTime = defaultAnimationTime;
			if (params.size() == 2){
				animationTime = abs(str_to_int(params[1].c_str()));
			}

			if (params[0] == "slideup"){
				UIManager::getSingleton()->onExitScreensaverRequested(this, SlideUp, animationTime);
			}else if (params[0] == "slidedown"){
				UIManager::getSingleton()->onExitScreensaverRequested(this, SlideDown, animationTime);
			}else if (params[0] == "slideleft"){
				UIManager::getSingleton()->onExitScreensaverRequested(this, SlideLeft, animationTime);
			}else if (params[0] == "slideright"){
				UIManager::getSingleton()->onExitScreensaverRequested(this, SlideRight, animationTime);
			}else{
				// Fade out
				UIManager::getSingleton()->onExitScreensaverRequested(this, FadeOut, animationTime);
			}
		}else{
			// Fade out
			UIManager::getSingleton()->onExitScreensaverRequested(this, FadeOut, defaultAnimationTime);
		}

		return;

	// Catch improper API call on a screensaver window
	}else if (functionName == L"_GLACloseWindow"){
		cerr << "A screensaver window is trying to call GLA.CloseWindow, where it should use GLA.ExitScreensaver instead. Ignoring call." << endl;
		return;
	}
	
	SystemWindow::onJavascriptCallback(functionName, params);
}

ScreensaverWindow::~ScreensaverWindow(){
	
}

}
