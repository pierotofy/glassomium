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
   
#include "Gesture.h"
#include "DragGesture.h"
#include "TwoFingerGesture.h"
#include "TouchGesture.h"

Gesture::Gesture(Gesture::Phase phase) : phase(phase){
}


/** Tries to identify a gesture from a touchgroup
 * @return if a valid gesture is found, the proper gesture object is returned, 0 otherwise */
Gesture *Gesture::recognize(TouchGroup &touchGroup, Gesture::Phase phase, const TouchEvent &lastEvent){
	Gesture *gesture;

	if ((gesture = DragGesture::recognize(touchGroup, phase)) != 0) return gesture;
	if ((gesture = TwoFingerGesture::recognize(touchGroup, phase)) != 0) return gesture;	
	if ((gesture = TouchGesture::recognize(touchGroup, phase, lastEvent)) != 0) return gesture;	

	return 0;
}

Gesture::~Gesture(){

}
