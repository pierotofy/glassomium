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

ColorChangeAnimation::ColorChangeAnimation(float msecs, const sf::Color &targetColor, Window *window, void(*animationEndedCallback)(Window *) = 0)
	: Animation(window, animationEndedCallback), targetColor(targetColor), msecs(msecs){

}

void ColorChangeAnimation::animate(){

	sf::Sprite *windowSprite = window->getSprite();

	// get current color values in float (cannot use ints or addition will get truncated to nearest int)
	float ca, cr, cg, cb;
	sf::Color currentColor = windowSprite->getColor();
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

	while(windowSprite->getColor() != targetColor){
		ca += da;
		cr += dr;
		cg += dg;
		cb += db;
		windowSprite->setColor(sf::Color((sf::Uint8)cr,(sf::Uint8)cg,(sf::Uint8)cb,(sf::Uint8)ca));
		sf::sleep(sf::milliseconds(1));
	}

	notifyAnimationEnded();
}

ColorChangeAnimation::~ColorChangeAnimation(){

}

