/*
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

   Copyright 2012 Piero Toffanin

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
   
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "stdafx.h"
#include <fstream>

using namespace std;

class FileManager {
private:
	string currentWorkingDirectory;

	static FileManager *singleton;
public:
	FileManager();
    ~FileManager();

	static void initialize();
	static void destroy();
	static FileManager *getSingleton();

	string getCurrentWorkingDirectory() const { return currentWorkingDirectory; }
	string readAll(const string &file);
	void writeAll(const string &content, const string &file);
	wstring readAllWide(const string &file);
};

#endif
