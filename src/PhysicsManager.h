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
   
#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "stdafx.h"
#include "ThemeConfiguration.h"
#include "Window.h"

using namespace std;
using namespace pt;

class PhysicsManager {
private:
	static PhysicsManager *singleton;

	bool enabled;
	bool initialized;

	b2World *world;
	float timeStep;
	float halfPhysicsWorldWidth;
	float halfPhysicsWorldHeight;

	float friction;
	float restitution;

	//float speedScalingFactor;

	std::map<int, b2Body*> bodies;

	void updateBody(Window *window);
	void destroyBody(Window *window);

	void addBoundaryBox(float x, float y, float width, float height);

	inline sf::Vector2f screenToPhysics(const sf::Vector2f &screenCoords);
	inline sf::Vector2f physicsToScreen(const sf::Vector2f &physicsCoords);
public:
	PhysicsManager();
    ~PhysicsManager();

	static void initialize();
	static void destroy();
	static PhysicsManager *getSingleton();

	void initializeWorld();
	void update();
	void setEnabled(bool);
	bool isEnabled();

	void setFriction(float);
	void setRestitution(float);

	void applyForce(Window *window, const sf::Vector2f &speed);
	void stopAllPhysics(Window *window);
};

#endif
