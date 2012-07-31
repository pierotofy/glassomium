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
   
#ifndef __Application_h_
#define __Application_h_

#include "stdafx.h"
#include "UIManager.h"
#include "TuioManager.h"
#include "FileManager.h"
#include "ServerManager.h"
#include "Window.h"

using namespace std;

class Application
{
public:
    Application(int argc, char* argv[]);
    virtual ~Application();

    void go();

    static float windowWidth;
    static float windowHeight;
	static float getScreenRatio();
	//static sf::Vector2f aspectRatio;
	static bool isPointOnScreenCorner(const sf::Vector2f &point, float cornerSize);
protected:
	TuioManager *tuioManager;
	sf::RenderWindow *renderWindow;
	bool fullscreen;
	bool mouseEnabled;

	void printUsage();
	void setupLayout();
	void handleMouseEvents();
};

#endif // #ifndef __Application_h_
