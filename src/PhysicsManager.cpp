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

#define METER_TO_PIXEL 50.0f
#define PIXEL_TO_METER 0.02f

PhysicsManager *PhysicsManager::singleton = 0;

void PhysicsManager::initialize(){
	PhysicsManager::singleton = new PhysicsManager();
}

void PhysicsManager::destroy(){
	RELEASE_SAFELY(PhysicsManager::singleton);
}

PhysicsManager *PhysicsManager::getSingleton(){
	return singleton;
}

PhysicsManager::PhysicsManager(){
	enabled = false;
	initialized = false;
	timeStep = 1.0f / 30.0f; // Should this be dynamic for better physics? 

	speedScalingFactor = (6.0f * Application::windowWidth * Application::windowHeight) / (800.0f * 600.0f);

	restitution = 0.0f;
	friction = 0.3f; // Default values
	world = 0;
}

/** Initializes the physics world */
void PhysicsManager::initializeWorld(){
	if (!initialized){
		b2Vec2 gravity(0.0f, 0.0f);
		world = new b2World(gravity);
		
				
		float worldWidth = Application::windowWidth * PIXEL_TO_METER;
		float worldHeight = Application::windowHeight * PIXEL_TO_METER;

		// We'll use these values later
		halfPhysicsWorldWidth = worldWidth / 2.0f;
		halfPhysicsWorldHeight = worldHeight / 2.0f;

		float thickness = max(worldHeight, worldWidth);

		// Create boundaries across the screen so that windows do not fly out of the screen
		
		// Bottom
		addBoundaryBox(0.0f, halfPhysicsWorldHeight + thickness / 2.0f, worldWidth, thickness);
		
		// Top
		addBoundaryBox(0.0f, -halfPhysicsWorldHeight - thickness / 2.0f, worldWidth, thickness);
		
		// Left
		addBoundaryBox(-halfPhysicsWorldWidth - thickness / 2.0f, 0.0f, thickness, worldHeight);

		// Right
		addBoundaryBox(halfPhysicsWorldWidth + thickness / 2.0f, 0.0f, thickness, worldHeight);

		initialized = true;
	}
}

/** Adds a boundary of the specified width and height with a center position of (x,y) (all physics coordinates) */
void PhysicsManager::addBoundaryBox(float x, float y, float width, float height){
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(x, y);
	b2Body* groundBody = world->CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(width / 2.0f, height / 2.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);
}

/** Sets whether we are making use of the physics effects */
void PhysicsManager::setEnabled(bool enabled){
	// Lazy loading
	if (!initialized){
		initializeWorld();
	}

	this->enabled = enabled;
}

/** To be called from the UI thread */
void PhysicsManager::update(){
	#define VELOCITYITERATIONS 8
	#define POSITIONITERATIONS 3

	if (enabled){
		world->Step(timeStep, VELOCITYITERATIONS, POSITIONITERATIONS);

		map<int, b2Body *>::iterator it;
		for (it = bodies.begin(); it != bodies.end(); it++){
			b2Body *body = it->second;
			Window *w = (Window *)body->GetUserData();
			
			w->setPosition(physicsToScreen(sf::Vector2f(body->GetPosition().x, body->GetPosition().y)));
			w->setRotation(body->GetAngle() * RADIANS_TO_DEGREES);			
		}
	}
}


void PhysicsManager::setFriction(float friction){
	this->friction = friction;
}

void PhysicsManager::setRestitution(float restitution){
	this->restitution = restitution;
}

bool PhysicsManager::isEnabled(){
	return enabled;
}


/** Applies a force to the window, which will cause the window to move and bounce on the 
 * boundaries of the screen
@param speed vector representing the force to be applied (pixels / seconds) */
void PhysicsManager::applyForce(Window *window, const sf::Vector2f &speed){
	if (enabled){
		updateBody(window);
		
		b2Body *body = bodies[window->getID()];
		body->ApplyLinearImpulse(b2Vec2(speed.x * PIXEL_TO_METER * speedScalingFactor, speed.y * PIXEL_TO_METER * speedScalingFactor), body->GetWorldCenter());
	}
}

/** Destroys the body associated with this window (if there's one) */
void PhysicsManager::destroyBody(Window *window){
	if (bodies.count(window->getID()) != 0){
		world->DestroyBody(bodies[window->getID()]);
		bodies.erase(window->getID());
	}
}

/** Creates a body for the the window (eliminating any previous body) */
void PhysicsManager::updateBody(Window *window){
	// Eliminate previous?
	destroyBody(window);

	// Create new body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;

	// Position, angle
	sf::Vector2f position = screenToPhysics(window->getPosition());
	bodyDef.position.Set(position.x, position.y);
	bodyDef.angle = window->getRotation() * DEGREES_TO_RADIANS;
	bodyDef.userData = window;

	// Size
	sf::Vector2f size = sf::Vector2f(window->getWidth() * PIXEL_TO_METER, window->getHeight() * PIXEL_TO_METER);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(size.x / 2.0f, size.y / 2.0f);

	// Fixture

	// Negative = don't collide, positive = collide
	#define GROUP_WINDOW -1

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.restitution = restitution;
	fixtureDef.friction = friction;
	fixtureDef.filter.groupIndex = GROUP_WINDOW; // Do not allow windows to collide

	b2Body* body = world->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
	body->SetLinearDamping(friction);
	body->SetAngularDamping(friction);

	// Keep track
	bodies[window->getID()] = body;
}

/** Stops all physics animations on the window */
void PhysicsManager::stopAllPhysics(Window *window){
	destroyBody(window);
}

/** Converts screen coordinates to a physics world range */
sf::Vector2f PhysicsManager::screenToPhysics(const sf::Vector2f &screenCoords){
	return sf::Vector2f(
		screenCoords.x * PIXEL_TO_METER - halfPhysicsWorldWidth,
		screenCoords.y * PIXEL_TO_METER - halfPhysicsWorldHeight
	);
}

/** Converts physics world coordinates to screen coordinates */
sf::Vector2f PhysicsManager::physicsToScreen(const sf::Vector2f &physicsCoords){
	return sf::Vector2f(
		(physicsCoords.x + halfPhysicsWorldWidth) * METER_TO_PIXEL,
		(physicsCoords.y + halfPhysicsWorldHeight) * METER_TO_PIXEL
	);
}

PhysicsManager::~PhysicsManager(){
	if (world){
		RELEASE_SAFELY(world);
	}
}

