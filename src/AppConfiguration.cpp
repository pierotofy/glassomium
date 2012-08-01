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
   
#include "AppConfiguration.h"

AppConfiguration::AppConfiguration()
	: Configuration(getIntDefaults(), getFloatDefaults(), getBoolDefaults(), getStringDefaults()) {
}

map<std::string, int> AppConfiguration::getIntDefaults(){
	map<std::string, int> result;
	return result;
}

map<std::string, float> AppConfiguration::getFloatDefaults(){
	map<std::string, float> result;
	result["window.width"] = 0.4f;
	result["window.height"] = 0.3f;
	return result;
}

map<std::string, bool> AppConfiguration::getBoolDefaults(){
	map<std::string, bool> result;
	result["window.transparent"] = true;
	result["window.fullscreen"] = false;
	result["window.menu.show"] = true;

	return result;
}

map<std::string, std::string> AppConfiguration::getStringDefaults(){
	map<std::string, std::string> result;
	return result;
}

AppConfiguration::~AppConfiguration(){
}


