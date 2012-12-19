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

#ifndef OVERLAYSPRITE_H
#define OVERLAYSPRITE_H

#include "stdafx.h"
#include "AnimatedObject.h"

/** An overlay sprite is used to cover the screen during
 * a transition of an application to fullscreen. */
class OverlaySprite : public AnimatedObject{
public:
	OverlaySprite(unsigned int width, unsigned int height);
	~OverlaySprite();
	void setColor(sf::Color color);
	void setData(void *data){ this->data = data; }
	void *getData(){ return data; }
	bool isVisible(){ return visible; };

	virtual void onAnimationStarted();
	virtual void onAnimationEnded();

	virtual sf::Sprite *getSprite(){ return sprite; }

	virtual Degrees getRotation() const { return sprite->getRotation(); }
	virtual sf::Vector2f getScale() const { return sprite->getScale(); }
	virtual sf::Vector2f getPosition() const { return sprite->getPosition(); }

	virtual void setRotation(Degrees angle){ sprite->setRotation(angle); }
	virtual void setScale(const sf::Vector2f &scale){ sprite->setScale(scale); }
	virtual void setPosition(const sf::Vector2f &screenPosition){ sprite->setPosition(screenPosition); }
private:
	sf::Sprite *sprite;
	bool visible;
	void *data;
};


#endif