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
   
#ifndef TUIOMANAGER_H
#define TUIOMANAGER_H

#include "stdafx.h"

using namespace std;
using namespace TUIO;

class TuioManager : public TuioListener {
public:
	TuioManager(int port);
    ~TuioManager();

private:
	TuioClient *client;

	void addTuioObject(TuioObject *tobj);
    void removeTuioObject(TuioObject *tobj);
    void updateTuioObject(TuioObject *tobj);

	void addTuioBlob(TuioBlob *tblb);
	void updateTuioBlob(TuioBlob *tblb);
	void removeTuioBlob(TuioBlob *tblb);

    void addTuioCursor(TuioCursor *tcur);
    void removeTuioCursor(TuioCursor *tcur);
    void updateTuioCursor(TuioCursor *tcur);
    
	void refresh(TuioTime bundleTime);
};

#endif
