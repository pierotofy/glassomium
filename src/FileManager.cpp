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
   
#ifdef _WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

#include "FileManager.h"
#include "Utils.h"

using namespace std;

FileManager *FileManager::singleton = 0;

void FileManager::initialize(){
	FileManager::singleton = new FileManager();
}

void FileManager::destroy(){
	RELEASE_SAFELY(FileManager::singleton);
}

FileManager *FileManager::getSingleton(){
	return singleton;
}

/** Writes a string to a file 
 * @return true on success */
void FileManager::writeAll(const string &content, const string &file){
  ofstream ofs(file.c_str());
  if (!ofs.is_open()){
      cerr << "Could not write to file " << file << endl;
      throw std::exception();
  }
  ofs << content;
  ofs.close();  
}

/** Reads a file from begin to end and puts it into a string */
string FileManager::readAll(const string &file){
  ifstream ifs (file.c_str(), ifstream::in);
  if (!ifs.is_open() ) {
  	cerr << "Could not open file " << file << endl;
    throw std::exception();
  }

  stringstream buffer;
  buffer << ifs.rdbuf();
  return buffer.str();
}

/** Reads a file and returns its content in wide char */
wstring FileManager::readAllWide(const string &file){
  string content = readAll(file);
  wchar_t *buffer = new wchar_t[content.size() + 1];
  size_t length = mbstowcs(buffer, content.c_str(), content.size());
  wstring result = wstring(buffer, length);
  RELEASE_SAFELY(buffer);
  return result;
}

FileManager::FileManager(){
	// Save current working directory
	char currentPathBuffer[FILENAME_MAX];
	if (!GetCurrentDir(currentPathBuffer, sizeof(currentPathBuffer) / sizeof(char))){
      throw exception();  
    } 
	currentPathBuffer[sizeof(currentPathBuffer) - 1] = '\0'; /* not really required */
	currentWorkingDirectory = string(currentPathBuffer);
}

FileManager::~FileManager(){

}



