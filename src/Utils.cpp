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
   
#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "Utils.h"

void log_exception(const char *msg, bool abort){
	std::cerr << "Exception: " << msg << std::endl;
	if (abort){
		std::cerr << "Exiting with error code 1" << std::endl;
		exit(1);
	}
}

void log_exception(const string& msg, bool abort){
	log_exception(msg.c_str(), abort);
}


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

int str_to_int(const char *str){
	int number;
	string input(str);

	stringstream mystream(input);
	if (mystream >> number){
		return number;
	}else{
		return -1; // Invalid number
	}
}

int str_to_int(const string &str){
	return str_to_int(str.c_str());
}

float str_to_float(const char *str){
	float number;
	string input(str);

	stringstream mystream(input);
	if (mystream >> number){
		return number;
	}else{
		return -1.0f; // Invalid number
	}
}

float str_to_float(const string &str){
	return str_to_float(str.c_str());
}


string utf8unescape(const string &str){
	if (str.length() <= 1) return str;
	if (str[0] != '\\' || str[1] != 'u') return str;

    int i = 0;
	int len = str.length();
    stringstream ss;;
    while (i < len) {
		char c = str[i];
		if (i <= (len - 6) && c == '\\' && str[i+1] == 'u') {
			c = (char) strtol(str.substr(i+2, i+6).c_str(), 0, 16);
			i += 6;
        }else{
			i++;
		}

        ss << c;
    }

	return ss.str();
}

float dotProduct(const sf::Vector2f &v1, const sf::Vector2f &v2){
    return (v1.x*v2.x + v1.y*v2.y);
}
 
Radians angleBetween(const sf::Vector2f &v1, const sf::Vector2f &v2){
	sf::Vector2f v1_n = v1;
	sf::Vector2f v2_n = v2;
	normalize(v1_n);
	normalize(v2_n);
	float bottom = sqrt(dotProduct(v1_n,v1_n))*sqrt(dotProduct(v2_n,v2_n));
	if (!FEQUALZERO(bottom)){
		float fraction = dotProduct(v1_n,v2_n)/bottom;

		// acos is defined only from [-1, 1]
		if (fraction <= 1.0f && fraction >= -1.0f){
			return acos(fraction);
		}else{
			return 0.0f;
		}
	}else{
		return 0.0f;
	}
}

/** Returns the Z component of the pross product */
float crossProduct(const sf::Vector2f &v1, const sf::Vector2f &v2){
	return v1.x * v2.y - v1.y * v2.x;
}

/** Destructive */
float normalize(sf::Vector2f &v1){
	const float vector_length = sqrt( v1.x*v1.x + v1.y*v1.y);
	if (vector_length > 0.0){
      v1.x /= vector_length;
      v1.y /= vector_length;
    }
  
    return vector_length;
}

float pointDistance(const sf::Vector2f &v1, const sf::Vector2f &v2){
	return sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));
}

float squaredLength(const sf::Vector2f &v){
	return v.x * v.x + v.y * v.y;
}


