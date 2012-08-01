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
   
#include "TouchGesture.h"
#include "Application.h"

TouchGesture::TouchGesture(Gesture::Phase phase, const TouchEvent &touchEvent) 
 : Gesture(phase), touchEvent(touchEvent){

}

/** Recognizes a touch gesture
 * @return a TouchGesture object if a valid gesture is recognized, 0 otherwise */
TouchGesture* TouchGesture::recognize(TouchGroup &touchGroup, Gesture::Phase phase, const TouchEvent &lastEvent){

	if (phase == Gesture::BEGINNING){
		if (touchGroup.getSize() == 1){
			
			return new TouchGesture(Gesture::BEGINNING, lastEvent);
		}else return 0;

	}else if (phase == Gesture::UPDATING){

		if (touchGroup.getSize() != 1) return 0;

		return new TouchGesture(Gesture::UPDATING, lastEvent);

	}else if (phase == Gesture::ENDING){
		if (touchGroup.getLastGesture() == Gesture::TOUCH){
			if (touchGroup.getSize() == 1){
				return new TouchGesture(Gesture::ENDING, lastEvent);
			}else{
				return 0; // We haven't ended our gesture yet
			}
		}else return 0; // Different gesture
	}

	return 0;
}


TouchGesture::~TouchGesture(){

}
