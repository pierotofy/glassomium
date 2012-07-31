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
   
#include "ServerManager.h"
#include "FileManager.h"
#include "Utils.h"

using namespace std;

ServerManager *ServerManager::singleton = 0;

void ServerManager::initialize(string serverAddress, int serverPort){
	ServerManager::singleton = new ServerManager(serverAddress, serverPort);
}

void ServerManager::destroy(){
	RELEASE_SAFELY(ServerManager::singleton);
}

ServerManager *ServerManager::getSingleton(){
	return singleton;
}

ServerManager::ServerManager(string serverAddress, int serverPort)
	: serverAddress(serverAddress), serverPort(serverPort){
	
}


/** Updates the local cache copy of some JS resources by pulling them
 * from the UI Server */
void ServerManager::retrieveJsResources(){
	sf::Http httpClient(serverAddress, serverPort);

	// Retrieve injectOnLoad.js
	sf::Http::Response response = httpClient.sendRequest(sf::Http::Request("/system/injectonload"));
	if (response.getStatus() == response.Ok){
		string content = response.getBody();
		cout << "Successfully retrieved inject on load code" << endl;

		// Save content to disk
		FileManager::getSingleton()->writeAll(content, "js/injectOnLoad.js");
	}else{
		cerr << "Could not contact UI Server to retrieve the inject on load script. Did you forget to start it?" << endl;
		throw runtime_error("UI server unreachable");
	}

	/** Contact the UI Server and retrieve the window menu code
      * This code is injected into each window that has a menu */

	response = httpClient.sendRequest(sf::Http::Request("/system/window/menu?response_on_one_line=1&escape_double_quotes=1"));
	if (response.getStatus() == response.Ok){
		string menuContent = response.getBody();
		
		// Add pragmatically jquery code
		menuContent = "$jsafe('body').append(\"" + menuContent + "\");";
		cout << "Successfully retrieved window menu content" << endl;
		
		// Cache menu content to disk
		FileManager::getSingleton()->writeAll(menuContent, "js/windowMenu.js");
	}else{
		cerr << "Could not contact UI Server to retrieve menu content. Did you forget to start it?" << endl;
		throw runtime_error("UI server unreachable");
	}

	// Retrieve the browser bar

	response = httpClient.sendRequest(sf::Http::Request("/system/browserbar?response_on_one_line=1&escape_double_quotes=1"));
	if (response.getStatus() == response.Ok){
		string barContent = response.getBody();
		
		// Add pragmatically jquery code
		barContent = "$jsafe('body').append(\"" + barContent + "\");";
		cout << "Successfully retrieved browser bar content" << endl;
		
		// Cache menu content to disk
		FileManager::getSingleton()->writeAll(barContent, "js/browserBar.js");
	}else{
		cerr << "Could not contact UI Server to retrieve browser bar content. Did you forget to start it?" << endl;
		throw runtime_error("UI server unreachable");
	}
}

/** Contacts the UI Server and dumps the configuration for each application
 * the caller is responsible for deallocating the result */
std::map<std::string, AppConfiguration *> *ServerManager::retrieveAppConfigurations(){
	sf::Http httpClient(serverAddress, serverPort);
	std::map<std::string, AppConfiguration *> *result = new std::map<std::string, AppConfiguration *>();

	// Retrieve application configurations
	sf::Http::Response response = httpClient.sendRequest(sf::Http::Request("/system/appconfigs"));
	if (response.getStatus() == response.Ok){
		string configuration = response.getBody();
		
		std::vector<std::string> lines = split(configuration, '\n');
		unsigned int i = 0;
		while(i < lines.size()){
			// Beginning a new app config?
			if (lines[i] == "APP_CONFIG_BEGIN"){
				// Next line is the app URL
				std::string appURL = lines[++i];

				// Set defaults first, not all config files might specify all properties!
				(*result)[appURL] = new AppConfiguration();

				// Next lines are "key<space>value" strings that represent the configuration values
				while(lines[++i] != "APP_CONFIG_END" && i < lines.size()){
					std::vector<std::string> parts = split(lines[i], ' ');
					if (parts.size() != 2){
						cerr << "Invalid configuration line received from UI Server: " << lines[i] << endl;
						throw runtime_error("Invalid UI Server communication");
					}
					std::string key = parts[0];
					std::string value = parts[1];

					// Ints?
					if (AppConfiguration::getIntDefaults().count(key) != 0){
						(*result)[appURL]->addInt(key, str_to_int(value));
					// Floats?
					}else if (AppConfiguration::getFloatDefaults().count(key) != 0){
						(*result)[appURL]->addFloat(key, str_to_float(value));
					// Bools?
					}else if (AppConfiguration::getBoolDefaults().count(key) != 0){
						(*result)[appURL]->addBool(key, value == "yes" ? true : false);
					// Strings?
					}else if (AppConfiguration::getStringDefaults().count(key) != 0){
						(*result)[appURL]->addString(key, value);
					}else{
						cout << "WARNING! Unrecognized configuration property: " << key << " for " << appURL << endl;
					}
				}
			}

			i++;
		}
	}else{
		cerr << "Could not contact UI Server to retrieve application configuration. Are you running the proper version?" << endl;
		throw runtime_error("UI server unreachable");
	}

	cout << "Successfully received application configurations!" << endl;

	return result;
}

/** Contacts the UI Server and dumps the configuration of the current theme
 * the caller is responsible for deallocating the result */
ThemeConfiguration *ServerManager::retrieveThemeConfiguration(){
	sf::Http httpClient(serverAddress, serverPort);
	ThemeConfiguration *result = new ThemeConfiguration();

	// Retrieve application configurations
	sf::Http::Response response = httpClient.sendRequest(sf::Http::Request("/system/themeconfig"));
	if (response.getStatus() == response.Ok){
		string configuration = response.getBody();
		
		std::vector<std::string> lines = split(configuration, '\n');
		if (lines[0] != "THEME_CONFIG_BEGIN"){
			cout << "FATAL! Invalid theme configuration format received from UI Server, exiting..." << endl;
			exit(EXIT_FAILURE);
		}
		unsigned int i = 0;
		while(i < lines.size()){
			// Next lines are "namespace<space>key<space>value" strings that represent the configuration values
			while(lines[++i] != "THEME_CONFIG_END" && i < lines.size()){
				std::vector<std::string> parts = split(lines[i], ' ');
				if (parts.size() != 3){
					cerr << "Invalid configuration theme line received from UI Server: " << lines[i] << endl;
					throw runtime_error("Invalid data received from UI Server");
				}

				std::string ns = parts[0];
				std::string key = parts[1];
				std::string value = parts[2];
				
				// Initialize default values
				if (!result->containsNamespace(ns)){
					result->initializeNamespace(ns, 
						ThemeConfiguration::getIntDefaults(),
						ThemeConfiguration::getFloatDefaults(),
						ThemeConfiguration::getBoolDefaults(),
						ThemeConfiguration::getStringDefaults());
				}

				// Ints?
				if (ThemeConfiguration::getIntDefaults().count(key) != 0){
					result->addInt(ns, key, str_to_int(value));
				// Floats?
				}else if (ThemeConfiguration::getFloatDefaults().count(key) != 0){
					result->addFloat(ns, key, str_to_float(value));
				// Bools?
				}else if (ThemeConfiguration::getBoolDefaults().count(key) != 0){
					result->addBool(ns, key, value == "yes" ? true : false);
				}else if (ThemeConfiguration::getStringDefaults().count(key) != 0){
					result->addString(ns, key, value);
				}else{
					cout << "WARNING! Unrecognized configuration property: " << key << " for theme" << endl;
				}
			}

			i++;
		}
	}else{
		cerr << "Could not contact UI Server to retrieve application configuration. Are you running the proper version?" << endl;
		throw runtime_error("UI server unreachable");
	}

	cout << "Successfully received theme configuration!" << endl;

	return result;
}

ServerManager::~ServerManager(){

}



