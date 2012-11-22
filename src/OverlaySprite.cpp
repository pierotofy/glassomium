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
   
#include "OverlaySprite.h"

/** @param width,height size of the sprite (usually matches the screen size) */
OverlaySprite::OverlaySprite(unsigned int width, unsigned int height){
	sprite = new sf::Sprite();
	sf::Texture *spriteTex = new sf::Texture();
	spriteTex->create(1, 1);

	int *data = new int[1];
	data[0] = 0xFFFFFFFF;
	spriteTex->update((sf::Uint8 *)data, 1, 1, 0, 0);
	RELEASE_SAFELY(data);
	
	sprite->setTexture(*spriteTex, true);

	// Set center anchor point
	sprite->setOrigin(0, 0);

	// Resize
	sprite->setScale(sf::Vector2f((float)width, (float)height));

	visible = false;
}

/** Sets the color for this overlay 
 * Note that the overlay will remain transparent
 * until an animation is applied to it */
void OverlaySprite::setColor(sf::Color color){
	color.a = 0;
	sprite->setColor(color);
	visible = false;
}

void OverlaySprite::onAnimationStarted(){
	visible = true;
}

void OverlaySprite::onAnimationEnded(){
	if (sprite->getColor().a == 0){
		visible = false;
	}
}

OverlaySprite::~OverlaySprite(){
	RELEASE_SAFELY(sprite);
}

