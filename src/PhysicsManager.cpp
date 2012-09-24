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
   
#include "PhysicsManager.h"
#include "Application.h"
#include "Utils.h"

using namespace std;

PhysicsManager::PhysicsManager(){
	enabled = false;
	initialized = false;
}

/** Initializes the physics world */
void PhysicsManager::initialize(){
	if (!initialized){


		initialized = true;
	}
}

/** Sets whether we are making use of the physics effects */
void PhysicsManager::setEnabled(bool enabled){
	// Lazy loading
	if (!initialized){
		initialize();
	}

	this->enabled = enabled;
}

bool PhysicsManager::isEnabled(){
	return enabled;
}

PhysicsManager::~PhysicsManager(){

}

