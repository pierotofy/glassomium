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
   
#ifndef UTILS_H_
#define UTILS_H_

#include "stdafx.h"

#define DEBUG 1

using namespace std;

#define RELEASE_SAFELY(__POINTER) { delete __POINTER; __POINTER = NULL; }

#ifdef DEBUG
#define LOG(__MSG) { std::cout << "DEBUG: " << __MSG << std::endl; }
#else
#define LOG(__MSG) { }
#endif

#define FEQUAL(a,b) (fabs((a) - (b)) < 0.01f)
#define FEQUALZERO(a) (fabs(a) < 0.01f)
#define FEQUALVECTORS(a,b) ( ( FEQUAL(a.x, b.x) ) && ( FEQUAL(a.y, b.y) ) )

void log_exception(const char *, bool);
void log_exception(const string&, bool);

// String utils
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
int str_to_int(const char *str);
int str_to_int(const string &str);
float str_to_float(const char *str);
float str_to_float(const string &str);
std::string utf8unescape(const string &str);

// Math utils
typedef float Radians;
typedef float Degrees;
#define DEGREES_TO_RADIANS 0.0174532925f
#define RADIANS_TO_DEGREES 57.2957796f

float dotProduct(const sf::Vector2f &v1, const sf::Vector2f &v2); 
Radians angleBetween(const sf::Vector2f &v1, const sf::Vector2f &v2);
float crossProduct(const sf::Vector2f &v1, const sf::Vector2f &v2);
float normalize(sf::Vector2f &v1);
float pointDistance(const sf::Vector2f &v1, const sf::Vector2f &v2);
float squaredLength(const sf::Vector2f &v);

#endif
