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
   
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "stdafx.h"

namespace pt{
	/** Rectangle struct */
	struct Rectangle
	{
	    float left;
	    float top;
	    float right;
	    float bottom;


	    Rectangle(){ left = top = right = bottom = 0; }
	    Rectangle(float left, float top, float right, float bottom)
	      : left(left), top(top), right(right), bottom(bottom){	    	
	    }


		float getWidth(){ return right - left; }
		float getHeight(){ return bottom - top; }
		sf::Vector2f getCenter(){ 
				return sf::Vector2f((right - left)/2.0f + left,
							  		(bottom - top)/2.0f + top);
		}

	    inline bool inside(int x, int y) const { return x >= left && x <= right && y >= top && y <= bottom; }
	};

	inline Rectangle intersect(const Rectangle& lhs, const Rectangle& rhs)
	{
	    Rectangle r;

	    r.left   = lhs.left   > rhs.left   ? lhs.left   : rhs.left;
	    r.top    = lhs.top    > rhs.top    ? lhs.top    : rhs.top;
	    r.right  = lhs.right  < rhs.right  ? lhs.right  : rhs.right;
	    r.bottom = lhs.bottom < rhs.bottom ? lhs.bottom : rhs.bottom;

	    return r;
	}
}

#endif
