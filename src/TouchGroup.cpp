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
   
#include "TouchGroup.h"
#include "Utils.h"

int TouchGroup::instances_count = 0;

/** @param touch the first touch part of this group
  * @param window ID that this touchgroup is assigned to */
TouchGroup::TouchGroup(Blob *touch, int windowId)
	: windowId(windowId){
	TouchGroup::instances_count++;
	id = TouchGroup::instances_count;
	lastGesture = 0; // NONE

	add(touch);
}

void TouchGroup::add(Blob *touch){
	touchList.push_back(touch);
}

void TouchGroup::update(Blob *touch){
	for (unsigned int i = 0; i < touchList.size(); i++){
		if (touchList[i]->id == touch->id){
			RELEASE_SAFELY(touchList[i]); // Delete previous object
			touchList[i] = touch; // Set reference to new one
			break;
		}
	}
}

void TouchGroup::remove(Blob *touch){
	// Remove it from touchDeltas (if it's there)
	if (touchDeltas.count(touch->id) == 1){
		RELEASE_SAFELY(touchDeltas[touch->id]);
		touchDeltas.erase(touch->id);
	}

	for (unsigned int i = 0; i < touchList.size(); i++){
		if (touchList[i]->id == touch->id){

			// If the touch in our list is a duplicate, we take care of deallocating it
			if (touchList[i] != touch){
				Blob *elementToErase = touchList[i];
				RELEASE_SAFELY(elementToErase);
			}

			touchList.erase(touchList.begin() + i); // Actually erase the ith element
			break;
		}
	}
}

bool TouchGroup::contains(Blob *touch){
	for (unsigned int i = 0; i < touchList.size(); i++){
		if (touchList[i]->id == touch->id){
			return true;
		}
	}
	return false;
}

void TouchGroup::setLastGesture(int gesture){
	lastGesture = gesture;
}

int TouchGroup::getLastGesture(){
	return lastGesture;
}

/** Calculates the shortest distance that cursor is relative to all members
 * of this touch group (the distance between the cursor and the closest cursor member of this
 * touch group) */
float TouchGroup::shortestDistance(Blob *touch){
	float minDistance = FLT_MAX;
	for (unsigned int i = 0; i < touchList.size(); i++){
		float distance = touchList[i]->getDistance(touch);
		if (distance < minDistance){
			minDistance = distance;
		}
	}
	return minDistance;
}

/** Computes the average of the positions of the cursors in this touchgroup
 * @return a 2D location that is the point "in the middle" (value ranges between 0 and 1) */
sf::Vector2f TouchGroup::getMeanTouchLocation() const{
	sf::Vector2f meanTouchLocation(0.0f, 0.0f);
	assert(touchList.size() > 0);

	for (unsigned int i = 0; i < touchList.size(); i++){
		meanTouchLocation.x += touchList[i]->x;
		meanTouchLocation.y += touchList[i]->y;

	}
	meanTouchLocation.x /= (float)touchList.size();
	meanTouchLocation.y /= (float)touchList.size();

	return meanTouchLocation;
}

/** Computes the distance of from each cursor in this touchgroup to point and returns the longest
 * @param point reference point to calculate the distances (range 0..1) */
float TouchGroup::getLongestDistanceFromPoint(sf::Vector2f point) const{
	sf::Vector2f touchLocation;
		
	float longestDistance = FLT_MIN;

	for (unsigned int i = 0; i < touchList.size(); i++){
		touchLocation.x = touchList[i]->x;
		touchLocation.y = touchList[i]->y;
		float distance = pointDistance(point, touchLocation);
		if (distance > longestDistance) longestDistance = distance;
	}

	return longestDistance;
}

/** The constant center is the same as the mean location with the difference
 * that changing the number of touches in the group will not cause a big shift of 
 * the center (thus the name "constant"). Touches that are taken off or added to the 
 * group to not directly contribute to the change of position of the constant center.
 * The constant center is modified only in the deltaX and deltaY changes of each touch.
 * You need to call resetConstCenter() and updateConstCenter() before you can get
 * a meaningful value.
 * @return the location of the constant center of this group (values range between 0 and 1) */
sf::Vector2f TouchGroup::getConstCenter(){
	return constCenter;
}

/** Resets the value of the constant center */
void TouchGroup::resetConstCenter(){
	constCenter = this->getMeanTouchLocation();

	for (unsigned int i = 0; i < touchList.size(); i++){
		// Already there?
		if (touchDeltas.count(touchList[i]->id) == 1){
			touchDeltas[touchList[i]->id]->x = touchList[i]->x;
			touchDeltas[touchList[i]->id]->y = touchList[i]->y;
		}else{
			touchDeltas[touchList[i]->id] = new sf::Vector2f(touchList[i]->x, touchList[i]->y);
		}
	}
}

/** Updates the location of the constant center */
void TouchGroup::updateConstCenter(){
	// Calculate new delta average
	sf::Vector2f delta;
	int evaluatedCount = 0;

	for (unsigned int i = 0; i < touchList.size(); i++){
		// Already there?
		if (touchDeltas.count(touchList[i]->id) == 1){
			delta.x += (touchList[i]->x - touchDeltas[touchList[i]->id]->x);
			delta.y += (touchList[i]->y - touchDeltas[touchList[i]->id]->y);
			evaluatedCount++;

			// Reset
			touchDeltas[touchList[i]->id]->x = touchList[i]->x;
			touchDeltas[touchList[i]->id]->y = touchList[i]->y;
		}else{
			touchDeltas[touchList[i]->id] = new sf::Vector2f(touchList[i]->x, touchList[i]->y);
		}
	}

	delta.x /= (float)evaluatedCount;
	delta.y /= (float)evaluatedCount;

	// Move const center
	constCenter += delta;
}

TouchGroup::~TouchGroup(){
	for (std::map<int, sf::Vector2f *>::iterator it = touchDeltas.begin(); it != touchDeltas.end(); it++){
		RELEASE_SAFELY(it->second);
	}
	touchDeltas.clear();

	TouchGroup::instances_count--;
}

