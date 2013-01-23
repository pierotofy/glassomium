/*
   Glassomium - web-based TUIO-enabled window manager
   http://www.glassomium.org

   Copyright 2013 The Glassomium Authors

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
   
#ifndef ROTATEDRECTANGLE_H
#define ROTATEDRECTANGLE_H

#include "stdafx.h"

namespace pt{
	/** A rotated rectangle contains 4 corner points (global screen coordinates) */
	struct RotatedRectangle
	{
	    sf::Vector2f topLeft;
	    sf::Vector2f topRight;
	    sf::Vector2f bottomLeft;
	    sf::Vector2f bottomRight;

	    RotatedRectangle(const sf::Vector2f &topLeft, const sf::Vector2f &topRight, const sf::Vector2f &bottomLeft, const sf::Vector2f &bottomRight)
	      : topLeft(topLeft), topRight(topRight), bottomLeft(bottomLeft), bottomRight(bottomRight){	    	
	    }
	};
}

#endif
