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
   
#include "ThemeConfiguration.h"

ThemeConfiguration::ThemeConfiguration()
	: Configuration(getIntDefaults(), getFloatDefaults(), getBoolDefaults(), getStringDefaults()) {
}

map<std::string, int> ThemeConfiguration::getIntDefaults(){
	map<std::string, int> result;
	result["screensaver.wait"] = 180;
	result["window.z-order"] = 0;
	result["windows.drag-color"] = 0x00FFEF00; // Yellow (00,R,G,B) B = LSB
	result["desktop.fade-transition-color"] = 0x00000000; // Black
	return result;
}

map<std::string, float> ThemeConfiguration::getFloatDefaults(){
	map<std::string, float> result;
	result["window.width"] = 0.5f;
	result["window.height"] = 0.5f;
	result["window.position.x"] = 0.5f;
	result["window.position.y"] = 0.5f;
	result["window.rotation"] = 0.0f;
	result["physics.drag-friction"] = 0.6f;
	result["physics.drag-restitution"] = 0.3f;
	result["windows.close-threshold"] = 0.12f;
	return result;
}

map<std::string, bool> ThemeConfiguration::getBoolDefaults(){
	map<std::string, bool> result;
	result["physics.enabled"] = false;
	result["windows.minimize.enabled"] = false;
	return result;
}

map<std::string, std::string> ThemeConfiguration::getStringDefaults(){
	map<std::string, std::string> result;
	result["window.type"] = "system";
	result["window.URL"] = "";
	return result;
}

/** @return the list of components (windows, keyboards, etc.) in this configuration
 * a.k.a. the list of namespaces expect the default namespace */
std::vector<std::string> ThemeConfiguration::getComponentsList(){
	std::vector<std::string> result;
	map<std::string, int>::iterator it;
	for (it = namespaces.begin(); it != namespaces.end(); it++){
		if (it->first != DEFAULT_NS) result.push_back(it->first);
	}
	return result;
}

/** Prints the information contained in this configuration (debug purposes) */
void ThemeConfiguration::dump(){
	std::vector<std::string> components;
	map<std::string, int>::iterator it;
	for (it = namespaces.begin(); it != namespaces.end(); it++){
		components.push_back(it->first);
	}

	cout << "Dumping theme configuration" << endl << "Components found in theme: " << endl;
	for (unsigned int i = 0; i < components.size(); i++){
		cout << "  " << components[i] << endl;
		
		map<std::string, float>::iterator floatIt;
		for (floatIt = floatData[components[i]].begin(); floatIt != floatData[components[i]].end(); floatIt++){
			cout << "    " << floatIt->first << " : " << floatIt->second << endl;
		}

		map<std::string, bool>::iterator boolIt;
		for (boolIt = boolData[components[i]].begin(); boolIt != boolData[components[i]].end(); boolIt++){
			cout << "    " << boolIt->first << " " << (boolIt->second ? "yes" : "no") << endl;
		}

		map<std::string, int>::iterator intIt;
		for (intIt = intData[components[i]].begin(); intIt != intData[components[i]].end(); intIt++){
			cout << "    " << intIt->first << " : " << intIt->second << endl;
		}

		map<std::string, std::string>::iterator strIt;
		for (strIt = strData[components[i]].begin(); strIt != strData[components[i]].end(); strIt++){
			cout << "    " << strIt->first << " : " << strIt->second << endl;
		}
	}
}

ThemeConfiguration::~ThemeConfiguration(){

}


