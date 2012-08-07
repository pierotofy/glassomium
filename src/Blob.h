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

#ifndef BLOB_H
#define BLOB_H

#include "stdafx.h"
using namespace TUIO;


/** A blob encapsulates TuioCursor (TUIO 1.0) and TuioBlob (TUIO 1.1)
 * we use this structure instead of the native ones because the TUIO listener
 * deallocates those objects with no predictable pattern, so we have to create a new
 * Blob object whenever we receive them. */
struct Blob {
	int id;
	float x;
	float y;
	float screenX;
	float screenY;
	float speedX;
	float speedY;

	/// Exclusive to TUIO 1.1
	float width;
	float height;
	Degrees angle;

	Blob(Blob *other);
	Blob(TuioBlob *other);
	Blob(TuioCursor *other);

	float getDistance(Blob *other) const{
		float dx = x - other->x;
		float dy = y - other->y;
		return sqrtf(dx*dx+dy*dy);
	}

	~Blob(){}
};

#endif
