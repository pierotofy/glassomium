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
   
#ifndef TOUCHGROUP_H
#define TOUCHGROUP_H

#include "stdafx.h"
#include "Blob.h"

class TouchGroup {
public:
	TouchGroup(Blob *, int);
    ~TouchGroup();

	void add(Blob *);
	void remove(Blob *);
	void update(Blob *);

	int getSize() const { return touchList.size(); }
	int getID() const { return id; }
	int getWindowID() const { return windowId; }
	bool contains(Blob *);
	float shortestDistance(Blob *);
	Blob *getTouch(int i) const{ return touchList[i]; }
	sf::Vector2f getMeanTouchLocation() const; // Center point
	float getLongestDistanceFromPoint(sf::Vector2f point) const;

	void setLastGesture(int gesture); // Gesture::Type 
	int getLastGesture();

	sf::Vector2f getConstCenter();
	void resetConstCenter();
	void updateConstCenter();
private:
	sf::Vector2f constCenter;
	static int instances_count;
	int windowId;
	int lastGesture; // The last (valid) gesture associated with this touchgroup

	int id;
	std::vector<Blob *> touchList;
	std::map<int, sf::Vector2f *> touchDeltas;

};

#endif
