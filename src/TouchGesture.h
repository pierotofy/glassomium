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
   
#ifndef TOUCHGESTURE_H
#define TOUCHGESTURE_H

#include "stdafx.h"
#include "Gesture.h"
#include "TouchEvent.h"

class TouchGesture : public Gesture {
public:
	TouchGesture(Gesture::Phase phase);
	TouchGesture(Gesture::Phase phase, const TouchEvent &touchEvent);
	~TouchGesture();

	static TouchGesture *recognize(TouchGroup &, Gesture::Phase phase, const TouchEvent &);
	virtual Type getGestureType(){ return TOUCH; }

	TouchEvent getTouchEvent(){ return touchEvent; }
private:
	TouchEvent touchEvent;
};

#endif
