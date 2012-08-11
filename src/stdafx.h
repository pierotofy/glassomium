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
   
#ifndef STDAFX_H
#define STDAFX_H

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_display_handler.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_request_handler.h"
#include "include/cef_render_handler.h"
#include "include/internal/cef_ptr.h"

#include "TUIO/TuioCursor.h"
#include "TUIO/TuioClient.h"
#include "TUIO/TuioBlob.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
#include <stack>
#include <float.h>
#include <assert.h>
#include <stdexcept>

#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Network/Http.hpp>

#include "Utils.h"

#endif
