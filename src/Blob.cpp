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

#include "Blob.h"
#include "Application.h"


Blob::Blob(Blob *other)
	: id(other->id), x(other->x), y(other->y), screenX(other->screenX),
	screenY(other->screenY), speedX(other->speedX), speedY(other->speedY),
	width(other->width), height(other->height), angle(other->angle){

}

Blob::Blob(TuioBlob *other)
	: id(other->getBlobID()), x(other->getX()), y(other->getY()), screenX(other->getX() * Application::windowWidth),
	screenY(other->getY() * Application::windowHeight), speedX(other->getXSpeed()), speedY(other->getYSpeed()),
	width(other->getWidth()), height(other->getHeight()), angle(-other->getAngle()){
		 
}

Blob::Blob(TuioCursor *other)
	: id(other->getCursorID()), x(other->getX()), y(other->getY()), screenX(other->getX() * Application::windowWidth),
	screenY(other->getY() * Application::windowHeight), speedX(other->getXSpeed()), speedY(other->getYSpeed()),
    width(0.0f), height(0.0f), angle(0.0f){

}