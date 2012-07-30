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
   
#ifndef TRANSFORMANIMATION_H
#define TRANSFORMANIMATION_H

#include "stdafx.h"
#include "Animation.h"

class TransformAnimation : public Animation {
public:
	enum Easing{
		None, Linear
	};

	TransformAnimation(float msecs, const sf::Vector2f &targetScale, const sf::Vector2f &targetPosition, Degrees targetRotation, Easing easing, Window *window, void(*animationEndedCallback)(Window *) = 0);
	virtual ~TransformAnimation();

	virtual void animate();
private:
	sf::Vector2f targetScale;
	sf::Vector2f targetPosition;
	Degrees targetRotation;
	Easing easing;
	float msecs;
};

// Derived
class ScaleAnimation : public TransformAnimation{
public:
	ScaleAnimation(float msecs, const sf::Vector2f &targetScale, Easing easing, Window *window, void(*animationEndedCallback)(Window *) = 0)
		: TransformAnimation(msecs, targetScale, window->getPosition(), window->getRotation(), easing, window, animationEndedCallback){}
};

class MoveAnimation : public TransformAnimation{
public:
	MoveAnimation(float msecs, const sf::Vector2f &targetPosition, Easing easing, Window *window, void(*animationEndedCallback)(Window *) = 0)
		: TransformAnimation(msecs, window->getScale(), targetPosition, window->getRotation(), easing, window, animationEndedCallback){}
};


#endif
