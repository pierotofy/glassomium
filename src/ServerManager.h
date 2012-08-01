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
   
#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "stdafx.h"
#include "AppConfiguration.h"
#include "ThemeConfiguration.h"

using namespace std;

class ServerManager {
private:
	static ServerManager *singleton;
	string serverAddress;
	int serverPort;
public:
	ServerManager(string serverAddress, int serverPort);
    ~ServerManager();

	static void initialize(string serverAddress, int serverPort);
	static void destroy();
	static ServerManager *getSingleton();

	void retrieveJsResources();

	std::map<std::string, AppConfiguration *> *retrieveAppConfigurations();
	ThemeConfiguration *retrieveThemeConfiguration();

};

#endif
