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


/** Finds the ID of the touch group to which this touch belongs (if it belongs anywhere)
  * @return id the touch group if any, -1 otherwise */
int GestureManager::findTouchGroup(TuioCursor *touch){
	std::map<int, TouchGroup *>::iterator iter;
	for (iter = touchGroups.begin(); iter != touchGroups.end(); iter++){
		if (iter->second->contains(touch)){
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
int GestureManager::findClosestTouchGroup(TuioCursor *touch, float maxDistance){
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
int GestureManager::findOverlayingWindowId(TuioCursor *touch){
	float screen_x = touch->getX() * Application::windowWidth;
	float screen_y = touch->getY() * Application::windowHeight;

	Window *window = UIManager::getSingleton()->findFirstWindow(screen_x, screen_y);
	if (window){
		return window->getID();
	}else{
		return -1;
	}
}

/** @param phase what phase of the gesture are we trying to recognize? 
  * @param lastEvent what was the last event processed? */
void GestureManager::recognizeGestures(Gesture::Phase phase, const TouchEvent &lastEvent){
	std::map<int, TouchGroup *>::iterator iter;
	for (iter = touchGroups.begin(); iter != touchGroups.end(); iter++){
		Gesture *gesture = Gesture::recognize(*iter->second, phase, lastEvent);

		if (gesture != 0){
			// Update reference
			iter->second->setLastGesture(gesture->getGestureType());

			// Retrieve location of the gesture based on average position of the touches in the tough group
			sf::Vector2f meanLocation = iter->second->getMeanTouchLocation();

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
			}else if (gesture->getGestureType() == Gesture::DRAG){
				UIManager::getSingleton()->onDragGesture(gestureEvent);
			}else if (gesture->getGestureType() == Gesture::TOUCH){
				UIManager::getSingleton()->onTouchGesture(gestureEvent);
			}

			// Cleanup
			RELEASE_SAFELY(gesture);
		}
	}
}

/** TuioCursors are copies of the original TuioCursor objects we retrieve from TuioManager
 * because we cannot predict when the original objects will be deallocated. This forces us to 
 * make sure that the object in our touch groups are up-to-date. Every time we receive an update
 * for a TuioCursor, we have to iterate our touchgroups and update our references */
void GestureManager::updateTouchGroupObjects(TuioCursor *touch){
	std::map<int, TouchGroup *>::iterator iter;
	for (iter = touchGroups.begin(); iter != touchGroups.end(); iter++){
		iter->second->update(touch);
	}
}

/** Takes care of handling those events that are queued from other threads
  * (ex. TUIO messages are sent from another thread and queued)
  * this method MUST be called by the rendering thread */
#define MAX_TOUCH_DISTANCE 0.30f

void GestureManager::processQueue(){
	TouchEvent touchEvent;
	while (touchDownQueue.pop(touchEvent)){

		// The first touch will always get -1 here
		int windowId = findOverlayingWindowId(touchEvent.touch);
		int touchGroupId = findTouchGroupForWindow(windowId);

		// if (touchGroupId == -1){
		// // If we didn't hit any window, find the closest existing touchgroup
		// 	touchGroupId = findClosestTouchGroup(touchEvent.touch, MAX_TOUCH_DISTANCE);
		// }

		// If we don't hit a window we put the touch in a big group of touches outside any window
		// We might want to change this in the future
		
		if (touchGroupId != -1){
			// Add to existing touch group
			touchGroups[touchGroupId]->add(touchEvent.touch);
		}else{
			// Create new one
			TouchGroup *touchGroup = new TouchGroup(touchEvent.touch, windowId);
			touchGroups[touchGroup->getID()] = touchGroup;

			// Update group group id
			touchGroupId = touchGroup->getID();
		}

		// Update touch event to include a reference to the touch group
		touchEvent.group = touchGroups[touchGroupId];

		// Try to identify possible gestures (to be ended)
		recognizeGestures(Gesture::ENDING, touchEvent);

		// Try to identify possible (new) gestures
		recognizeGestures(Gesture::BEGINNING, touchEvent);

		UIManager::getSingleton()->onTrackTouchDown(touchEvent);
	}

	while (touchMoveQueue.pop(touchEvent)){
		// Update
		updateTouchGroupObjects(touchEvent.touch);

		int touchGroupId = findTouchGroup(touchEvent.touch);

		if (touchGroupId != -1){
			touchEvent.group = touchGroups[touchGroupId];
		}

		// Try to identify possible gestures
		recognizeGestures(Gesture::UPDATING, touchEvent);

		UIManager::getSingleton()->onTrackTouchMove(touchEvent);
	}

	while (touchUpQueue.pop(touchEvent)){
		int touchGroupId = findTouchGroup(touchEvent.touch);

		if (touchGroupId != -1){
			touchEvent.group = touchGroups[touchGroupId];
		}

		UIManager::getSingleton()->onTrackTouchUp(touchEvent);
		
		// Try to identify possible gestures (particularly TouchGesture
		// which needs to be notified before the touchGroup is disposed) 
		recognizeGestures(Gesture::ENDING, touchEvent);

		// After we add a touch, we (most times) should be able to remove it
		if (touchGroupId != -1){

			// Remove from touch group
			touchGroups[touchGroupId]->remove(touchEvent.touch);

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

		// Try to identify possible gestures (to be ended)
		recognizeGestures(Gesture::ENDING, touchEvent);

		// Try to identify possible (new) gestures
		recognizeGestures(Gesture::BEGINNING, touchEvent);

		// Cleanup
		RELEASE_SAFELY(touchEvent.touch);
	}
}

