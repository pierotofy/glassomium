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
   
#include "TwoFingerGesture.h"

/** @param actionMask bitwise-or combination of TwoFingerGestureAction items */
TwoFingerGesture::TwoFingerGesture(Phase phase, int actionMask) 
 : Gesture(phase), actionMask(actionMask){
}

/** Recognizes a two finger gesture
 * @return a TwoFingerGesture object if a valid gesture is recognized, 0 otherwise */
TwoFingerGesture* TwoFingerGesture::recognize(TouchGroup &touchGroup, Gesture::Phase phase){

	if (phase == Gesture::BEGINNING){
		// If there are two touches down, we have a candidate
		if (touchGroup.getSize() == 2){
			TwoFingerGesture *gesture = new TwoFingerGesture(Gesture::BEGINNING, ALL);
			gesture->fillGestureData(touchGroup);
			gesture->fillTransformData(touchGroup);
			return gesture;
		}else return 0;

	}else if (phase == Gesture::UPDATING){
		// We need always two fingers to do any two finger gesture
		if (touchGroup.getSize() != 2) return 0;

		int actionMask = NONE;
		TwoFingerGesture *gesture = new TwoFingerGesture(Gesture::UPDATING, actionMask);

		TwoFingerGesture::findScrollGesture(touchGroup, actionMask, gesture);

		// Do not find scaling and rotation if we have found a scroll (causes annoying overlaps)
		if ((actionMask & SCROLL) != SCROLL){
			TwoFingerGesture::findTransformGesture(touchGroup, actionMask, gesture);
		}

		if (actionMask != NONE){
			gesture->setActionMask(actionMask);
			gesture->fillGestureData(touchGroup);
			return gesture;
		}else return 0;

	}else if (phase == Gesture::ENDING){
		if (touchGroup.getLastGesture() == Gesture::TWOFINGER){
			if (touchGroup.getSize() == 2){
				TwoFingerGesture *gesture = new TwoFingerGesture(Gesture::ENDING, ALL);
				gesture->fillGestureData(touchGroup);
				return gesture;
			}else{
				return 0; // We haven't ended our gesture yet
			}
		}else return 0; // Different gesture
	}

	return 0;
}

/** @param actionMask, gesture will be updated accordingly if a gesture is found */
inline void TwoFingerGesture::findScrollGesture(TouchGroup &touchGroup, int &actionMask, TwoFingerGesture *gesture){
	sf::Vector2f scrollDirection(0.0f, 0.0f);
	TuioCursor *firstTouch = touchGroup.getTouch(0);
	TuioCursor *secondTouch = touchGroup.getTouch(1);

	sf::Vector2f firstScrollDirection(firstTouch->getXSpeed(), -firstTouch->getYSpeed());
	sf::Vector2f secondScrollDirection(secondTouch->getXSpeed(), -secondTouch->getYSpeed());
	
	// The slowest direction betweent the two touches is the direction we'll use
	if (squaredLength(firstScrollDirection) < squaredLength(secondScrollDirection)){
		scrollDirection = firstScrollDirection;
	}else{
		scrollDirection = secondScrollDirection;
	}

	#define SCROLL_SPEED_THRESHOLD 0.1f
	#define SCROLL_ANGLE_THRESHOLD 10

	// Fast enough?
	if (fabs(scrollDirection.x) > SCROLL_SPEED_THRESHOLD || fabs(scrollDirection.y) > SCROLL_SPEED_THRESHOLD){ 

		// Compute the angle between the two vectors, if within a certain angle, then same direction
		Radians angleBetweenTouches = angleBetween(firstScrollDirection, secondScrollDirection);
		if (angleBetweenTouches * RADIANS_TO_DEGREES < SCROLL_ANGLE_THRESHOLD){
			actionMask |= SCROLL;
			gesture->scrollDirection = scrollDirection;
		}
	}
}

inline void TwoFingerGesture::findTransformGesture(TouchGroup &touchGroup, int &actionMask, TwoFingerGesture *gesture){
	sf::Vector2f transformDirection(0.0f, 0.0f);
	TuioCursor *firstTouch = touchGroup.getTouch(0);
	TuioCursor *secondTouch = touchGroup.getTouch(1);

	sf::Vector2f firstTransformDirection(firstTouch->getXSpeed(), firstTouch->getYSpeed());
	sf::Vector2f secondTranformDirection(secondTouch->getXSpeed(), secondTouch->getYSpeed());
	
	// The fastest direction betweent the two touches is the direction we'll use
	if (squaredLength(firstTransformDirection) > squaredLength(secondTranformDirection)){
		transformDirection = firstTransformDirection;
	}else{
		transformDirection = secondTranformDirection;
	}

	#define TRANSFORM_SPEED_THRESHOLD 0.1f
	#define TRANSFORM_ANGLE_THRESHOLD 30

	bool firstTransformDirectionFastEnough = (fabs(firstTransformDirection.x) > TRANSFORM_SPEED_THRESHOLD || 
										  fabs(firstTransformDirection.y) > TRANSFORM_SPEED_THRESHOLD);
	bool secondTranformDirectionFastEnough = (fabs(secondTranformDirection.x) > TRANSFORM_SPEED_THRESHOLD || 
										   fabs(secondTranformDirection.y) > TRANSFORM_SPEED_THRESHOLD);

	// Fast enough?
	if (firstTransformDirectionFastEnough || secondTranformDirectionFastEnough){

		// If a touch is not moving, we might still want to be able to transform (but right now the
		// speed vector is not defined, so we don't detect a gesture)
		
		// Redefine speed vector to be the opposite of the faster one (if needed)
		if (!firstTransformDirectionFastEnough){
			firstTransformDirection = -secondTranformDirection;
		}else if (!secondTranformDirectionFastEnough){
			secondTranformDirection = -firstTransformDirection;
		}

		// Compute the angle between one vector and the inverse of other vector
		// if it's within 180 - threshold, then they are moving in opposite direction
		Radians angleBetweenTouches = angleBetween(firstTransformDirection, secondTranformDirection);
		if (angleBetweenTouches * RADIANS_TO_DEGREES > (180.0f - TRANSFORM_ANGLE_THRESHOLD)){

			gesture->fillTransformData(touchGroup);

			actionMask |= TRANSFORM;
		}
	}

}

void TwoFingerGesture::fillTransformData(const TouchGroup &touchGroup){
	// Find longest distance of a touch from the center
	sf::Vector2f center = touchGroup.getMeanTouchLocation();			
	this->transformDistanceFromCenter = touchGroup.getLongestDistanceFromPoint(center);
}

void TwoFingerGesture::fillGestureData(const TouchGroup &touchGroup){
	assert(touchGroup.getSize() == 2);

	// Save location of the touches
	firstTouchLocation = sf::Vector2f(touchGroup.getTouch(0)->getX(), touchGroup.getTouch(0)->getY());
	secondTouchLocation = sf::Vector2f(touchGroup.getTouch(1)->getX(), touchGroup.getTouch(1)->getY());

	centerLocation = touchGroup.getMeanTouchLocation();
}

bool TwoFingerGesture::containsAction(TwoFingerGestureAction action){
	return (actionMask & action) == action;
}

void TwoFingerGesture::setActionMask(int actionMask){
	this->actionMask = actionMask;
}

/* Getters */

sf::Vector2f TwoFingerGesture::getScrollDirection(){
	return scrollDirection;
}

float TwoFingerGesture::getTransformDistanceFromCenter(){
	return transformDistanceFromCenter;
}

TwoFingerGesture::~TwoFingerGesture(){

}
