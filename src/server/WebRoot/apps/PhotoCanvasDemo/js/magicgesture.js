/** Copyright Piero Toffanin 2012 
 Magic Gesture - A simple gesture event generator for those multi touch devices that do not support them.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

var MagicGesture = {
	_pointToObject : null,
	_idToObject: null,
	_getId: null,
	_setId: null,
	_touchGroups : {},
	_previousTouchesIn : {},
	

	/** Initializes magic gesture
	 * @param pointToObject (optional) specify a function that finds 
	 * 		an object at certain screen coordinates. By default the document.elementFromPoint is used.
	 *		It's useful to specify a custom handler for example when the application does the drawing on a canvas.
	 * @param idToObject (optional) specify a function that finds
	 *		an object based on its ID. By default document.getElementById is used.
	 * @param getId (optional) specify a function that given an element
	 		returns its id. By default element.id is used. 
	 * @param setId (optional) specify a function that given an element
	 		and an ID, associates the element with a specific id. By default element.id is used.*/
	init : function(pointToObject, idToObject, getId, setId){
		this._pointToObject = pointToObject == undefined ? this._defaultPointToObject : pointToObject;
		this._idToObject = idToObject == undefined ? this._defaultIdToObject : idToObject;
		this._getId = getId == undefined ? this._defaultGetId : getId;
		this._setId = setId == undefined ? this._defaultSetId : setId;

		document.addEventListener('touchstart', this._onTouchStart, false);
		document.addEventListener('touchmove', this._onTouchMove, false);
		document.addEventListener('touchend', this._onTouchEnd, false);
	},

	_onTouchStart : function(event) {    
		if (event.magic) return false; // It's already magic! Don't process again

		MagicGesture._onNumberOfTouchesChanged(event, "ontouchstart");
	},

	_onTouchMove : function(event) {    
		if (event.magic) return false; // It's already magic! Don't process again

	    for (var i = 0; i < event.touches.length; i++) {
	    	// Update touches
		    for (var id in MagicGesture._touchGroups){
		    	var group = MagicGesture._touchGroups[id];
		    	for (var j in group.touches){
		    		if (group.touches[j].identifier == event.touches[i].identifier){
		    			group.touches[j] = event.touches[i];
		    			break;
		    		}
		    	}
		    }	
	    }    

	    // Gestures
	    for (var id in MagicGesture._touchGroups){
	    	var group = MagicGesture._touchGroups[id];
	    	if (group.touches.length == 2){
	    		if (group.initialDistance != 0){
		    		var dx = group.touches[0].pageX - group.touches[1].pageX;
		    		var dy = group.touches[0].pageY - group.touches[1].pageY;
		    		var newDistance = Math.sqrt(dx * dx + dy * dy); 
		    		var newAngle = MagicGesture._angleBetweenVectors(group.initialVector, [dx, dy]);
	    		
	    			MagicGesture._RaiseGestureEvent("gesturechange", MagicGesture._idToObject(id), newAngle, newDistance / group.initialDistance);
	    		}
	    	}else if (group.touches.length == 1){
	    		MagicGesture._RaiseTouchEvent("touchmove", MagicGesture._idToObject(id), group.touches);
	    	}
	    }
	},

	_onTouchEnd : function(event) {  
		if (event.magic) return false; // It's already magic! Don't process again 

		MagicGesture._onNumberOfTouchesChanged(event, "ontouchend"); 
	},

	/** Called by both onTouchStart and onTouchEnd */
	_onNumberOfTouchesChanged : function(event, caller){
		for (var id in MagicGesture._touchGroups){
			MagicGesture._touchGroups[id].touches = [];
		}

		var touches = event.touches;
		if (touches.length == 0 && event.changedTouches.length > 0){
			touches = event.changedTouches;
		}

	    for (var i = 0; i < touches.length; i++) {
	    	var element = MagicGesture._pointToObject(touches[i].pageX, touches[i].pageY);
	    	if (!element) continue;

	    	// Not all elements have an ID, if that's the case we'll create one
			var elementId = MagicGesture._getId(element);
			if(!elementId) {
			  MagicGesture._setId(element, PseudoGuid.GetNew());
			}

	    	if (MagicGesture._touchGroups[elementId] == undefined) MagicGesture._touchGroups[elementId] = {touches:[], initialDistance:0, initialVector:[]};

	    	MagicGesture._touchGroups[elementId].touches.push(touches[i]);
	    }

	    for (var id in MagicGesture._touchGroups){
	    	if (MagicGesture._previousTouchesIn[id] == undefined) MagicGesture._previousTouchesIn[id] = 0;

	    	var group = MagicGesture._touchGroups[id];
	    	var numberOfTouchesChanged = MagicGesture._previousTouchesIn[id] != group.touches.length;

	    	if (numberOfTouchesChanged && group.touches.length == 2){
	    		var dx = group.touches[0].pageX - group.touches[1].pageX;
	    		var dy = group.touches[0].pageY - group.touches[1].pageY;
	    		group.initialDistance = Math.sqrt(dx * dx + dy * dy);
	    		group.initialVector = [dx, dy];

	    		MagicGesture._RaiseGestureEvent("gesturestart", MagicGesture._idToObject(id), 0, 0);
	    	}else if (numberOfTouchesChanged && group.touches.length != 2){
	    		MagicGesture._RaiseGestureEvent("gestureend", MagicGesture._idToObject(id), 0, 0);
	    	}


	    	if (caller == "ontouchstart" && group.touches.length == 1){
	    		MagicGesture._RaiseTouchEvent("touchstart", MagicGesture._idToObject(id), group.touches);
	    	}else if (caller == "ontouchend" && group.touches.length == 1){
	    		MagicGesture._RaiseTouchEvent("touchend", MagicGesture._idToObject(id), group.touches);
	    	}

	    	MagicGesture._previousTouchesIn[id] = group.touches.length;
	    }
	},

	_RaiseGestureEvent : function(name, element, rotation, scale){
		var evt = document.createEvent("Event");
		evt.initEvent(name, true, true);
		evt.rotation = rotation;
		evt.scale = scale;
		evt.magic = true;
		evt.magicTarget = element;

		if (element.dispatchEvent){
		 	element.dispatchEvent(evt);
		}else{	
			document.dispatchEvent(evt);
		}
	},

	_RaiseTouchEvent : function(name, element, touches){
		var evt = document.createEvent("Event");
		evt.initEvent(name, true, true);
		evt.magicTarget = element;
		evt.magic = true; // Avoid looping
		evt.touches = touches;
		evt.touch = touches[0];
		evt.changedTouches = touches;

		if (element.dispatchEvent){
		 	element.dispatchEvent(evt);
		}else{	
			document.dispatchEvent(evt);
		}
	},

	/** @return the angle between two vectors in degrees */
	_angleBetweenVectors : function(v1, v2){
		var v1_n = v1.slice(0); // Copy
		var v2_n = v2.slice(0); // Copy
		this._normalizeVector(v1_n);
		this._normalizeVector(v2_n);
		var bottom = Math.sqrt(this._dotProduct(v1_n,v1_n))*Math.sqrt(this._dotProduct(v2_n,v2_n));
		if (bottom != 0.0){
			var fraction = this._dotProduct(v1_n,v2_n)/bottom;

			// acos is defined only from [-1, 1]
			if (fraction <= 1.0 && fraction >= -1.0){
				var sign = 1;
				if (this._crossProduct(v1, v2) < 0.0) sign = -1;
				return Math.acos(fraction) * 57.2957796 * sign; // Radians to degrees
			}else{
				return 0.0;
			}
		}else{
			return 0.0;
		}
	},

	_dotProduct : function(v1, v2){
    	return (v1[0]*v2[0] + v1[1]*v2[1]);
	},

	/** Returns the Z component of the pross product */
	_crossProduct : function(v1, v2){
		return v1[0] * v2[1] - v1[1] * v2[0];
	},

	/** Destructive, normalize a vector */
	_normalizeVector : function(v){
		var vector_length = Math.sqrt( v[0]*v[0] + v[1]*v[1]);
		if (vector_length > 0.0){
	      v[0] /= vector_length;
	      v[1] /= vector_length;
	    }
	  
	    return vector_length;
	},

	_defaultPointToObject : function(pageX, pageY){
		return document.elementFromPoint(pageX, pageY);
	},

	_defaultIdToObject : function(id){
		return document.getElementById(id);
	},

	_defaultGetId : function(element){
		return element.id;
	},

	_defaultSetId : function(element, id){
		element.id = id;
	}
};

//http://stackoverflow.com/questions/226689/unique-element-id-even-if-element-doesnt-have-one
var PseudoGuid = new (function() {
    this.empty = "00000000-0000-0000-0000-000000000000";
    this.GetNew = function() {
        var fourChars = function() {
                return (((1 + Math.random()) * 0x10000)|0).toString(16).substring(1).toUpperCase();
        }
        return (fourChars() + fourChars() + "-" + fourChars() + "-" + fourChars() + "-" + fourChars() + "-" + fourChars() + fourChars() + fourChars());
    };
})();
