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
   
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "stdafx.h"

#define DEFAULT_NS "global"

class Configuration{
public:
	Configuration(map<std::string, int> intDefaults, map<std::string, float> floatDefaults, map<std::string, bool> boolDefaults,  map<std::string, std::string> strDefaults);
	virtual ~Configuration();

	void addInt(const string &, int);
	int getInt(const string &);
	void addInt(const string &, const string &, int);
	int getInt(const string &, const string &);

	void addBool(const string &, bool);
	bool getBool(const string &);
	void addBool(const string &, const string &, bool);
	bool getBool(const string &, const string &);

	void addFloat(const string &, float);
	float getFloat(const string &);
	void addFloat(const string &, const string &, float);
	float getFloat(const string &, const string &);

	void addString(const string &, const std::string &);
	std::string getString(const string &);
	void addString(const string &, const string &, const std::string &);
	std::string getString(const string &, const string &);

	bool containsNamespace(const string &ns);
	void initializeNamespace(const string &ns, map<std::string, int> intDefaults, map<std::string, float> floatDefaults, map<std::string, bool> boolDefaults, map<std::string, std::string> strDefaults);
protected:
	std::map<std::string, std::map<std::string, int> > intData;
	std::map<std::string, std::map<std::string, float> > floatData;
	std::map<std::string, std::map<std::string, bool> > boolData;
	std::map<std::string, std::map<std::string, std::string> > strData;

	std::map<std::string, int> namespaces; // Just keeps track of the namespaces added to this configuration (value = 1 exists)
};

#endif
