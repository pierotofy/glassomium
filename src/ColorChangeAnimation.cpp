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
   
#include "ColorChangeAnimation.h"

ColorChangeAnimation::ColorChangeAnimation(float msecs, const sf::Color &targetColor, AnimatedObject *object, void(*animationEndedCallback)(AnimatedObject *) = 0, bool alphaOnly = false)
	: Animation(object, animationEndedCallback), targetColor(targetColor), msecs(msecs), alphaOnly(alphaOnly){

}

void ColorChangeAnimation::animate(){
	sf::Sprite *sprite = object->getSprite();

	// get current color values in float (cannot use ints or addition will get truncated to nearest int)
	float ca, cr, cg, cb;
	sf::Color currentColor = sprite->getColor();
	ca = currentColor.a;
	cr = currentColor.r;
	cg = currentColor.g;
	cb = currentColor.b;

	// Calculate color deltas
	float da, dr, dg, db;
	da = (float)targetColor.a - currentColor.a;
	dr = (float)targetColor.r - currentColor.r;
	dg = (float)targetColor.g - currentColor.g;
	db = (float)targetColor.b - targetColor.b;

	// These are the total color displacement
	// Divide by number of msecs to get the color displacement per msec
	da /= msecs;
	dr /= msecs;
	dg /= msecs;
	db /= msecs;

	int c = 0;
	while(c < msecs){
		ca += da;

		if (!alphaOnly){
			cr += dr;
			cg += dg;
			cb += db;
		}

		sprite->setColor(sf::Color((sf::Uint8)cr,(sf::Uint8)cg,(sf::Uint8)cb,(sf::Uint8)ca));
		sf::sleep(sf::milliseconds(1));
		c++;
	}

	if (alphaOnly){
		sf::Color color = currentColor;
		color.a = targetColor.a;
		sprite->setColor(color);
	}else{
		sprite->setColor(targetColor);
	}

	notifyAnimationEnded();
}

ColorChangeAnimation::~ColorChangeAnimation(){

}

