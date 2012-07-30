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
   
#include "Animation.h"

Animation::Animation(Window *window, void(*animationEndedCallback)(Window *) = 0)
 : animationEndedCallback(animationEndedCallback), window(window){
	thread = new sf::Thread(&Animation::animate, &(*this));
}

/** All animation must run asynchronously */
void Animation::start(){
	thread->launch();
}

/** Notifies the animation manager that this animation has ended and is awaiting post animation processing */
void Animation::notifyAnimationEnded(){
	UIManager::getSingleton()->getAnimationManager()->onAnimationEnded(this);
}

/** To be called from the main UI thread */
void Animation::postAnimate(){
	if (animationEndedCallback != 0){
		animationEndedCallback(window);
	}
}

Animation::~Animation(){
	RELEASE_SAFELY(thread);
}
