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
   
#include "TransformAnimation.h"

TransformAnimation::TransformAnimation(float msecs, const sf::Vector2f &targetScale, const sf::Vector2f &targetPosition, Degrees targetRotation, Easing easing, Window *window, void(*animationEndedCallback)(Window *))
	: Animation(window, animationEndedCallback), targetScale(targetScale), targetPosition(targetPosition), targetRotation(targetRotation), easing(easing), msecs(msecs){
}

void TransformAnimation::animate(){

	sf::Sprite *windowSprite = window->getSprite();
	
	// get current values
	sf::Vector2f currentScale = windowSprite->getScale();
	sf::Vector2f currentPosition = window->getPosition();
	Degrees currentRotation = window->getRotation();

	// deltas
	sf::Vector2f deltaScale = (targetScale - currentScale) / msecs;
	sf::Vector2f deltaPosition = (targetPosition - currentPosition) / msecs;
	Degrees deltaRotation = (targetRotation - currentRotation) / msecs;

	// actual amounts incremented at each iteration
	sf::Vector2f scaleIncrement;
	sf::Vector2f positionIncrement;
	Degrees rotationIncrement = 0.0f;

	sf::Vector2f mScale; // Default to 0,0
	sf::Vector2f mPosition;
	Degrees mRotation = 0.0f;

	// if we are interpolating, we need to calculate a few things
	// Divide the time domain in 3 parts, the first part will accelerate, the second will stay constant, the second will decelarate
	const float N = 3.0f; 

	if (easing == Linear){
		float nFactor = N / (N - 1);
		float mFactor = N / msecs;

		deltaScale.x *= nFactor;
		deltaScale.y *= nFactor;

		deltaPosition.x *= nFactor;
		deltaPosition.y *= nFactor;

		deltaRotation *= nFactor;

		// Linear slope
		mScale = deltaScale;
		mScale.x *= mFactor;
		mScale.y *= mFactor;

		mPosition = deltaPosition;
		mPosition.x *= mFactor;
		mPosition.y *= mFactor;

		mRotation = deltaRotation * mFactor;
	}else{
		// No easing, simple add delta at each iteration

		scaleIncrement = deltaScale;
		positionIncrement = deltaPosition;
		rotationIncrement = deltaRotation;
	}

	int msecsElapsed = 0;

	while(msecsElapsed < msecs){
		if (easing != None){
			if (msecsElapsed < msecs/N){
				scaleIncrement += mScale;
				positionIncrement += mPosition;
				rotationIncrement += mRotation;
			}else if (msecsElapsed > (msecs - msecs/N)){
				scaleIncrement -= mScale;
				positionIncrement -= mPosition;
				rotationIncrement -= mRotation;
			}
		}

		currentScale += scaleIncrement;
		windowSprite->setScale(currentScale);

		currentPosition += positionIncrement;
		window->setPosition(currentPosition);

		currentRotation += rotationIncrement;
		window->setRotation(currentRotation);

		sf::sleep(sf::milliseconds(1));
		msecsElapsed++; // Not accurate, but relatively close
	}

	// Set the properties to their actual values (sometimes they might be off)
	window->setScale(targetScale);
	window->setPosition(targetPosition);
	window->setRotation(targetRotation);

	notifyAnimationEnded();
}

TransformAnimation::~TransformAnimation(){

}

