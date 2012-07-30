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
   
#include "AnimationManager.h"
#include "Animation.h"

AnimationManager::AnimationManager(){

}

AnimationManager::~AnimationManager(){

}

void AnimationManager::onAnimationEnded(Animation *animation){
	endedAnimationQueue.push(animation);
}

void AnimationManager::processQueue(){
	while(!endedAnimationQueue.empty()){
		Animation *animation = 0;
		endedAnimationQueue.pop(animation);
		animation->postAnimate();
		RELEASE_SAFELY(animation);
	}
}

