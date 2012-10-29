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
TwoFingerGesture::TwoFingerGesture(Phase phase) 
 : Gesture(phase){
}

/** Recognizes a two finger gesture
 * @return a TwoFingerGesture object if a valid gesture is recognized, 0 otherwise */
TwoFingerGesture* TwoFingerGesture::recognize(TouchGroup &touchGroup, Gesture::Phase phase){

	if (phase == Gesture::BEGINNING){
		// Make sure we come from a touch
		if (touchGroup.getSize() == 2 && touchGroup.getLastGesture() != Gesture::DRAG){
			TwoFingerGesture *gesture = new TwoFingerGesture(Gesture::BEGINNING);
			gesture->fillGestureData(touchGroup);
			return gesture;
		}else return 0;

	}else if (phase == Gesture::UPDATING){
		// We need always two fingers to do any two finger gesture
		if (touchGroup.getSize() != 2 || touchGroup.getLastGesture() != Gesture::TWOFINGER) return 0;

		TwoFingerGesture *gesture = new TwoFingerGesture(Gesture::UPDATING);
		gesture->fillGestureData(touchGroup);
		return gesture;
		
	}else if (phase == Gesture::ENDING){
		if (touchGroup.getLastGesture() == Gesture::TWOFINGER){
			if (touchGroup.getSize() == 2){
				TwoFingerGesture *gesture = new TwoFingerGesture(Gesture::ENDING);
				gesture->fillGestureData(touchGroup);
				return gesture;
			}else{
				return 0; // We haven't ended our gesture yet
			}
		}else return 0; // Different gesture
	}

	return 0;
}

void TwoFingerGesture::fillGestureData(const TouchGroup &touchGroup){
	assert(touchGroup.getSize() == 2);

	// Save location of the touches
	firstTouchLocation = sf::Vector2f(touchGroup.getTouch(0)->x, touchGroup.getTouch(0)->y);
	secondTouchLocation = sf::Vector2f(touchGroup.getTouch(1)->x, touchGroup.getTouch(1)->y);
}


TwoFingerGesture::~TwoFingerGesture(){

}
