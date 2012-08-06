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
   
#include "DragGesture.h"

DragGesture::DragGesture(Gesture::Phase phase) 
 : Gesture(phase){
	 
}

/** Recognizes a drag gesture
 * @return a DragGesture object if a valid gesture is recognized, 0 otherwise */
DragGesture* DragGesture::recognize(TouchGroup &touchGroup, Gesture::Phase phase){

	if (phase == Gesture::BEGINNING){
		// If there are three touches down, we have a drag
		if (touchGroup.getSize() == 3){
			return new DragGesture(Gesture::BEGINNING);
		}else return 0;

	}else if (phase == Gesture::UPDATING){
		if (touchGroup.getLastGesture() != Gesture::DRAG) return 0;

		return new DragGesture(Gesture::UPDATING);
	}else if (phase == Gesture::ENDING){
		// If we had a valid drag gesture and now we don't have the right number of touches, the drag ended

		if (touchGroup.getLastGesture() == Gesture::DRAG){
			if (touchGroup.getSize() == 1){
				return new DragGesture(Gesture::ENDING); // We had a drag, but now the number of touches has changed
			}else{
				return 0; // We haven't ended our gesture yet
			}
		}else return 0; // Different gesture
	}

	return 0;
}


DragGesture::~DragGesture(){

}
