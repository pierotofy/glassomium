/*
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

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
   
#ifndef GESTURE_H
#define GESTURE_H

#include "stdafx.h"
#include "TouchGroup.h"
#include "TouchEvent.h"

class Gesture{
public:
	enum Type { NONE, DRAG, TWOFINGER, TOUCH };
	enum Phase { BEGINNING, UPDATING, ENDING };

	Gesture(Gesture::Phase phase);
	virtual ~Gesture();
	
	static Gesture *recognize(TouchGroup &, Phase, const TouchEvent &);

	virtual Type getGestureType() = 0;
	Phase getPhase(){ return phase; }
private:
	Phase phase;
};

#endif
