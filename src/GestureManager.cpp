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

#include "Application.h"
#include "GestureManager.h"
#include "Window.h"
#include "TwoFingerGesture.h"

GestureManager::GestureManager(){

}

GestureManager::~GestureManager(){

}

/** Touch events are triggered from another thread, so are queued for later processing */
void GestureManager::onTouchDown(const TouchEvent &touchEvent){
	touchDownQueue.push(touchEvent);
}

void GestureManager::onTouchUp(const TouchEvent &touchEvent){
	touchUpQueue.push(touchEvent);
}

void GestureManager::onTouchMove(const TouchEvent &touchEvent){
	touchMoveQueue.push(touchEvent);
}


/** Finds the ID of the touch group to which this blob belongs (if it belongs anywhere)
  * @return id the touch group if any, -1 otherwise */
int GestureManager::findTouchGroup(Blob *blob){
	std::map<int, TouchGroup *>::iterator iter;
	for (iter = touchGroups.begin(); iter != touchGroups.end(); iter++){
		if (iter->second->contains(blob)){
			return iter->second->getID();
		}
	}
	return -1;
}

/** Finds the ID of the touch group to which this touch is closest to
  * 
  * @param maxDistance is the threshold level that indicates what maximum distance
		a touch might have before it's not considered part of a group
	@return the ID of the touch group if any, -1 otherwise */
int GestureManager::findClosestTouchGroup(Blob *touch, float maxDistance){
	std::map<int, TouchGroup *>::iterator iter;
	float validClosestDistance = FLT_MAX;

	// If a valid distance is never found, this will be return value
	int closestTouchGroupId = -1; 

	for (iter = touchGroups.begin(); iter != touchGroups.end(); iter++){
		float touchDistance = iter->second->shortestDistance(touch);
		if (touchDistance < validClosestDistance && touchDistance < maxDistance){
			validClosestDistance = touchDistance;
			closestTouchGroupId = iter->second->getID();
		}
	}

	return closestTouchGroupId;
}

/** Finds the ID of the touch group associated with a window id */
int GestureManager::findTouchGroupForWindow(int windowId){
	std::map<int, TouchGroup *>::iterator iter;

	for (iter = touchGroups.begin(); iter != touchGroups.end(); iter++){
		if (iter->second->getWindowID() == windowId){
			return iter->second->getID();
		}
	}

	return -1;
}

/** Finds the ID of the window that the cursor is currently over
  * @return ID of the window or -1 if no window is currently covering the location of the touch */
int GestureManager::findOverlayingWindowId(Blob *touch){
	Window *window = UIManager::getSingleton()->findFirstWindow(touch->screenX, touch->screenY);
	if (window){
		return window->getID();
	}else{
		return -1;
	}
}


// Uncomment this to enable debug messages for touch gestures
//#define COUT_GESTURES 1

/** @param touchGroup the touchgroup we are currently processing
  * @param phase what phase of the gesture are we trying to recognize? 
  * @param lastEvent what was the last event processed? */
void GestureManager::recognizeGestures(TouchGroup *touchGroup, Gesture::Phase phase, const TouchEvent &lastEvent){
	Gesture *gesture = Gesture::recognize(*touchGroup, phase, lastEvent);

	if (gesture != 0){
		// Update reference
		touchGroup->setLastGesture(gesture->getGestureType());

		// Retrieve location of the gesture based on average position of the touches in the tough group
		sf::Vector2f meanLocation = touchGroup->getMeanTouchLocation();

		// Convert to screen coordinates
		meanLocation.x *= Application::windowWidth;
		meanLocation.y *= Application::windowHeight;

		// Wrap a GestureEvent and send to UIManager
		GestureEvent gestureEvent(gesture, meanLocation);
			
		if (gesture->getGestureType() == Gesture::TWOFINGER){
			if (((TwoFingerGesture *)gesture)->containsAction(TwoFingerGesture::SCROLL)){
				UIManager::getSingleton()->onScrollGesture(gestureEvent);
			}

			if (((TwoFingerGesture *)gesture)->containsAction(TwoFingerGesture::TRANSFORM)){
				UIManager::getSingleton()->onTransformGesture(gestureEvent);
			}

			#ifdef COUT_GESTURES
				cout << "TwoFinger " << phase << endl;
			#endif
				
		}else if (gesture->getGestureType() == Gesture::DRAG){
			UIManager::getSingleton()->onDragGesture(gestureEvent);

			#ifdef COUT_GESTURES
				cout << "Drag " << phase << endl;
			#endif
		}else if (gesture->getGestureType() == Gesture::TOUCH){
			UIManager::getSingleton()->onTouchGesture(gestureEvent);

			#ifdef COUT_GESTURES
				cout << "Touch " << phase << endl;
			#endif
		}

		// Cleanup
		RELEASE_SAFELY(gesture);
	}else{
		if (touchGroup->getSize() == 0){
			touchGroup->setLastGesture((int)Gesture::NONE);
		}
	}
}

/** TuioCursors are copies of the original TuioCursor objects we retrieve from TuioManager
 * because we cannot predict when the original objects will be deallocated. This forces us to 
 * make sure that the object in our touch groups are up-to-date. Every time we receive an update
 * for a TuioCursor, we have to iterate our touchgroups and update our references */
void GestureManager::updateTouchGroupObjects(Blob *touch){
	std::map<int, TouchGroup *>::iterator iter;
	for (iter = touchGroups.begin(); iter != touchGroups.end(); iter++){
		iter->second->update(touch);
	}
}

//#define MAX_TOUCH_DISTANCE 0.30f
/** Takes care of handling those events that are queued from other threads
  * (ex. TUIO messages are sent from another thread and queued)
  * this method MUST be called by the rendering thread */
void GestureManager::processQueue(){
	TouchEvent touchEvent;
	while (touchDownQueue.pop(touchEvent)){

		// The first touch will always get -1 here
		int windowId = findOverlayingWindowId(touchEvent.blob);
		int touchGroupId = findTouchGroupForWindow(windowId);

		// if (touchGroupId == -1){
		// // If we didn't hit any window, find the closest existing touchgroup
		// 	touchGroupId = findClosestTouchGroup(touchEvent.blob, MAX_TOUCH_DISTANCE);
		// }

		// If we don't hit a window we put the touch in a big group of touches outside any window
		// We might want to change this in the future
		
		if (touchGroupId != -1){
			// Add to existing touch group
			touchGroups[touchGroupId]->add(touchEvent.blob);
		}else{
			// Create new one
			TouchGroup *touchGroup = new TouchGroup(touchEvent.blob, windowId);
			touchGroups[touchGroup->getID()] = touchGroup;

			// Update group group id
			touchGroupId = touchGroup->getID();
		}

		// Update touch event to include a reference to the touch group
		touchEvent.group = touchGroups[touchGroupId];

		// Try to identify possible (new) gestures
		recognizeGestures(touchGroups[touchGroupId], Gesture::BEGINNING, touchEvent);

		UIManager::getSingleton()->onTrackTouchDown(touchEvent);
	}

	while (touchMoveQueue.pop(touchEvent)){
		// Update
		updateTouchGroupObjects(touchEvent.blob);

		int touchGroupId = findTouchGroup(touchEvent.blob);

		if (touchGroupId != -1){
			touchEvent.group = touchGroups[touchGroupId];

			// Try to identify possible gestures
			recognizeGestures(touchGroups[touchGroupId], Gesture::UPDATING, touchEvent);
		}

		UIManager::getSingleton()->onTrackTouchMove(touchEvent);
	}

	while (touchUpQueue.pop(touchEvent)){
		int touchGroupId = findTouchGroup(touchEvent.blob);

		if (touchGroupId != -1){
			touchEvent.group = touchGroups[touchGroupId];
		}

		UIManager::getSingleton()->onTrackTouchUp(touchEvent);
		
		// After we add a touch, we (most times) should be able to remove it
		if (touchGroupId != -1){

			// Try to identify possible gestures
			recognizeGestures(touchGroups[touchGroupId], Gesture::ENDING, touchEvent);

			// Remove from touch group
			touchGroups[touchGroupId]->remove(touchEvent.blob);

			// Check for cleanup (if a touchgroup has no more members, it doesn't have a purpose anymore)
			if (touchGroups[touchGroupId]->getSize() == 0){
				TouchGroup *touchGroup = touchGroups[touchGroupId];
				RELEASE_SAFELY(touchGroup);
				touchGroups.erase(touchGroupId);
			}
		}else{
			// Caught an orphan touch up event (we might have missed a touch down event or received multiple touch up)
			// Remember, these messages are sent throgh UDP!			

			// Do nothing
		}

		// Cleanup
		RELEASE_SAFELY(touchEvent.blob);
	}
}

