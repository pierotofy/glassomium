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
   
#include "Application.h"
#include "TuioManager.h"
#include "TouchEvent.h"
#include "Utils.h"

/** @param port UDP port where TUIO messages are sent to */
TuioManager::TuioManager(int port){
	client = new TuioClient(port);
	client->addTuioListener(this);
	client->connect();
}

TuioManager::~TuioManager(){
	client->disconnect();
	RELEASE_SAFELY(client);
}

/** this is called when an object becomes visible */
void TuioManager::addTuioObject(TuioObject *tobj){
	cout << "Add tuio object (angle: " << tobj->getAngleDegrees() << " ) X: " << tobj->getX() << " Y: " << tobj->getY() << endl;
}

/** an object was removed from the table */
void TuioManager::removeTuioObject(TuioObject *tobj){
	cout << "Remove tuio object (angle: " << tobj->getAngleDegrees() << " ) X: " << tobj->getX() << " Y: " << tobj->getY() << endl;
}
    
/** an object was moved on the table surface */
void TuioManager::updateTuioObject(TuioObject *tobj){
	cout << "Update tuio object (angle: " << tobj->getAngleDegrees() << " ) X: " << tobj->getX() << " Y: " << tobj->getY() << endl;
}

/** A blob was added */
void TuioManager::addTuioBlob(TuioBlob *tblb){
	cout << "Add tuio blob (area: " << tblb->getArea() << " ) X: " << tblb->getX() << " Y: " << tblb->getY() << endl;
}
		
/** A blob was moved */
void TuioManager::updateTuioBlob(TuioBlob *tblb){
	cout << "Add tuio blob (area: " <<  tblb->getArea() << " ) X: " << tblb->getX() << " Y: " << tblb->getY() << endl;
}
		
/** A blob was removed from the surface */
void TuioManager::removeTuioBlob(TuioBlob *tblb){
	cout << "Add tuio blob (area: " <<  tblb->getArea() << " ) X: " << tblb->getX() << " Y: " << tblb->getY() << endl;
}

/** this is called when a new cursor is detected */
void TuioManager::addTuioCursor(TuioCursor *tcur){
	UIManager::getSingleton()->getGestureManager()->onTouchDown(TouchEvent((int)(tcur->getX() * Application::windowWidth),
											(int)(tcur->getY() * Application::windowHeight), 
											tcur->getCursorID(),
											new TuioCursor(tcur)));
}

/** a cursor was removed from the table */
void TuioManager::removeTuioCursor(TuioCursor *tcur){
	UIManager::getSingleton()->getGestureManager()->onTouchUp(TouchEvent((int)(tcur->getX() * Application::windowWidth),
											(int)(tcur->getY() * Application::windowHeight), 
											tcur->getCursorID(),
											new TuioCursor(tcur)));
}

/** a cursor was moving on the table surface */
void TuioManager::updateTuioCursor(TuioCursor *tcur){

	/** NOTE: The TuioCursor constructor has been modified from the original repository to
	 * carry the x_speed and y_speed of the original cursor. If you update the TUIO library
	 * make sure to carry those changes */

	UIManager::getSingleton()->getGestureManager()->onTouchMove(TouchEvent((int)(tcur->getX() * Application::windowWidth),
										(int)(tcur->getY() * Application::windowHeight), 
										tcur->getCursorID(),
										new TuioCursor(tcur)));
}

/** this method is called after each bundle, use it to repaint your screen for example */
void TuioManager::refresh(TuioTime bundleTime){

}
