/*
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

   Copyright 2012 Piero Toffanin

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
   
#ifndef GESTUREMANAGER_H
#define GESTUREMANAGER_H

#include "stdafx.h"
#include "TouchGroup.h"
#include "TouchEvent.h"
#include "GestureEvent.h"
#include "ConcurrentQueue.h"
#include "Gesture.h"
#include "Utils.h"

using namespace TUIO;

class GestureManager {
public:
	GestureManager();
    ~GestureManager();

	void onTouchDown(const TouchEvent &);
	void onTouchUp(const TouchEvent &);
	void onTouchMove(const TouchEvent &);

	void processQueue();

	// Public because we need it when deciding how to color a spritepointer (during debug)
	int findTouchGroup(TuioCursor *);
private:
	int findClosestTouchGroup(TuioCursor *, float);
	int findTouchGroupForWindow(int windowId);
	int findOverlayingWindowId(TuioCursor *);
	void updateTouchGroupObjects(TuioCursor *);
	void recognizeGestures(Gesture::Phase phase, const TouchEvent &);
	std::map<int, TouchGroup *> touchGroups;
	
	// Touch queues
	ConcurrentQueue<TouchEvent> touchDownQueue;
	ConcurrentQueue<TouchEvent> touchUpQueue;
	ConcurrentQueue<TouchEvent> touchMoveQueue;
};

#endif
