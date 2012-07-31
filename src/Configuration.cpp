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
   
#include "Configuration.h"

Configuration::Configuration(map<std::string, int> intDefaults, map<std::string, float> floatDefaults, map<std::string, bool> boolDefaults, map<std::string, std::string> strDefaults){
	initializeNamespace(DEFAULT_NS, intDefaults, floatDefaults, boolDefaults, strDefaults);
}

/** Initializes a namespace with the given values */
void Configuration::initializeNamespace(const string &ns, map<std::string, int> intDefaults, map<std::string, float> floatDefaults, map<std::string, bool> boolDefaults, map<std::string, std::string> strDefaults){
	intData[ns] = intDefaults;
	floatData[ns] = floatDefaults;
	boolData[ns] = boolDefaults;
	strData[ns] = strDefaults;
	namespaces[ns] = 1;
}

bool Configuration::containsNamespace(const string &ns){
	return namespaces.count(ns) > 0;
}

void Configuration::addInt(const string &key, int value){
	addInt(DEFAULT_NS, key, value);
}

int Configuration::getInt(const string &key){
	return getInt(DEFAULT_NS, key);
}

void Configuration::addInt(const string &ns, const string &key, int value){
	intData[ns][key] = value;
	namespaces[ns] = 1;
}

int Configuration::getInt(const string &ns, const string &key){
	return intData[ns][key];
}



void Configuration::addFloat(const string &key, float value){
	addFloat(DEFAULT_NS, key, value);
}

float Configuration::getFloat(const string &key){
	return getFloat(DEFAULT_NS, key);
}

void Configuration::addFloat(const string &ns, const string &key, float value){
	floatData[ns][key] = value;
	namespaces[ns] = 1;
}

float Configuration::getFloat(const string &ns, const string &key){
	return floatData[ns][key];
}



void Configuration::addBool(const string &key, bool value){
	addBool(DEFAULT_NS, key, value);
}

bool Configuration::getBool(const string &key){
	return getBool(DEFAULT_NS, key);
}

void Configuration::addBool(const string &ns, const string &key, bool value){
	boolData[ns][key] = value;
	namespaces[ns] = 1;
}

bool Configuration::getBool(const string &ns, const string &key){
	return boolData[ns][key];
}



void Configuration::addString(const string &key, const std::string &value){
	addString(DEFAULT_NS, key, value);
}

std::string Configuration::getString(const string &key){
	return getString(DEFAULT_NS, key);
}
void Configuration::addString(const string &ns, const string &key, const std::string &value){
	strData[ns][key] = value;
	namespaces[ns] = 1;
}

std::string Configuration::getString(const string &ns, const string &key){
	return strData[ns][key];
}

Configuration::~Configuration(){
}


