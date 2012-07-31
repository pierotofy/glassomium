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
   
#ifndef TWOFINGERGESTURE_H
#define TWOFINGERGESTURE_H

#include "stdafx.h"
#include "Gesture.h"

class TwoFingerGesture : public Gesture {
public:
	enum TwoFingerGestureAction { NONE = 0x0, // 00000000
								  ALL = 0xFFFFFFFF, // 11111111
								  SCROLL = 0x1,	   // 00000001
								  TRANSFORM = 0x2};   // 00000100

	TwoFingerGesture(Phase phase, int actionMask);
	~TwoFingerGesture();

	static TwoFingerGesture *recognize(TouchGroup &, Gesture::Phase phase);
	virtual Type getGestureType(){ return TWOFINGER; }
	
	bool containsAction(TwoFingerGestureAction action);

	/** Return the location of the first touch (range 0..1) */
	sf::Vector2f getFirstTouchLocation() const { return firstTouchLocation; }

	/** Return the location of the second touch (range 0..1) */
	sf::Vector2f getSecondTouchLocation() const { return secondTouchLocation; }

	/** Return the center location of the gesture (range 0..1) */
	sf::Vector2f getCenterLocation() const { return centerLocation; }

	// Scroll gesture methods
	sf::Vector2f getScrollDirection();
	
	// Transform (scale + rotate) gesture methods
	float getTransformDistanceFromCenter();

	void setActionMask(int actionMask);
private:
	int actionMask; // This value holds bit-wise or combination of one or more actions

	static void findScrollGesture(TouchGroup &touchGroup, int &actionMask, TwoFingerGesture *gesture);
	static void findTransformGesture(TouchGroup &touchGroup, int &actionMask, TwoFingerGesture *gesture);

	// Scroll data
	sf::Vector2f scrollDirection;
	
	// Transform data
	float transformDistanceFromCenter; // Distance from the center (median between two touches) to the farthest touch
	void fillTransformData(const TouchGroup &touchGroup);

	sf::Vector2f firstTouchLocation;
	sf::Vector2f secondTouchLocation;
	sf::Vector2f centerLocation;
	void fillGestureData(const TouchGroup &touchGroup);
};

#endif
