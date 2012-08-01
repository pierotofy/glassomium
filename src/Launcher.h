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
   
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "stdafx.h"
#include <SFGUI/SFGUI.hpp>
#include <SFML/Graphics.hpp>


using namespace std;

struct Resolution{
	int width;
	int height;
	std::string printable;
};

class Launcher {
    public:
		Launcher();
		virtual ~Launcher();

        void run();


 		void LaunchButton_Click();
		void ExitButton_Click();

		bool isFullscreen(){ return fullscreenChkBox->IsActive(); }
		int getResolutionWidth(){ return availableResolutions[resolutionComboBox->GetSelectedItem()].width; }
		int getResolutionHeight(){ return availableResolutions[resolutionComboBox->GetSelectedItem()].height; }
		/*
		sf::Vector2f getAspectRatio(){ 
			std::string text = aspectRatioComboBox->GetSelectedText();
			if (text == "16:9"){
				return sf::Vector2f(16.0f, 9.0f);
			}else{
				return sf::Vector2f(4.0f, 3.0f);
			}
		}*/
		bool isDebug(){ return debugChkBox->IsActive(); }
    private:
        // Create an SFGUI. This is required before doing anything with SFGUI.
        sfg::SFGUI sfgui;

		sf::RenderWindow *renderWindow;

		sfg::ComboBox::Ptr resolutionComboBox;
		//sfg::ComboBox::Ptr aspectRatioComboBox;
		sfg::CheckButton::Ptr fullscreenChkBox;
		sfg::CheckButton::Ptr debugChkBox;

		std::vector<Resolution> availableResolutions;
};

#endif