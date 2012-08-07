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
   
#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

#include "stdafx.h"
#include "Blob.h"

struct TouchEvent
{
	int screen_x;
	int screen_y;
	int touch_id;
	bool mouseSimulated; // If this flag is true, touch = 0 (mouse events don't generate tuio cursors)
	Blob *blob;
	TouchGroup *group; // This can be 0 at any point in time. Code using this member should verify for != NULL before dereferencing it.
	
	TouchEvent()
	    : screen_x(0), screen_y(0), touch_id(0), mouseSimulated(true), blob(0), group(0) {}

	TouchEvent(int screen_x, int screen_y, int touch_id)
	    : screen_x(screen_x), screen_y(screen_y), touch_id(touch_id), mouseSimulated(true), blob(0), group(0) {	    	
	}
	TouchEvent(int screen_x, int screen_y, int touch_id, Blob *blob)
	    : screen_x(screen_x), screen_y(screen_y), touch_id(touch_id), mouseSimulated(false), blob(blob), group(0) {	    	
	}
	TouchEvent(int screen_x, int screen_y, int touch_id, Blob *blob, TouchGroup *group)
	    : screen_x(screen_x), screen_y(screen_y), touch_id(touch_id), mouseSimulated(false), blob(blob), group(group) {	    	
	}


};
#endif
