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
   
#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include "stdafx.h"

class AnimatedObject{
public:
	AnimatedObject();
	virtual ~AnimatedObject();
	
	virtual void onAnimationStarted(){};
	virtual void onAnimationEnded(){};

	virtual sf::Sprite *getSprite() = 0;

	virtual Degrees getRotation() const = 0;
	virtual sf::Vector2f getScale() const = 0;
	virtual sf::Vector2f getPosition() const = 0;

	virtual void setRotation(Degrees angle) = 0;
	virtual void setScale(const sf::Vector2f &scale) = 0;
	virtual void setPosition(const sf::Vector2f &screenPosition) = 0;

};

#endif
