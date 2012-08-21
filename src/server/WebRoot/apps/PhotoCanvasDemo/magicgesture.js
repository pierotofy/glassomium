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
	_pointToObjectHandler : null,
	_idToObjectHandler: null,
	_getIdHandler: null,
	_setIdHandler: null,
	_touchGroups : {},
	_touchStartedOn : {}, // elements where a touchstart happened (element => bool)

	/** Initializes magic gesture
	 * @param pointToObjectHandler (optional) specify a function that finds 
	 * 		an object at certain screen coordinates. By default the document.elementFromPoint is used.
	 *		It's useful to specify a custom handler for example when the application does the drawing on a canvas.
	 * @param idToObjectHandler (optional) specify a function that finds
	 *		an object based on its ID. By default document.getElementById is used.
	 * @param getIdHandler (optional) specify a function that given an element
	 		returns its id. By default element.id is used. 
	 * @param setIdHandler (optional) specify a function that given an element
	 		and an ID, associates the element with a specific id. By default element.id is used.*/
	init : function(pointToObjectHandler, idToObjectHandler, getIdHandler, setIdHandler){
		this._pointToObjectHandler = pointToObjectHandler == undefined ? this._defaultPointToObjectHandler : pointToObjectHandler;
		this._idToObjectHandler = idToObjectHandler == undefined ? this._defaultIdToObjectHandler : idToObjectHandler;
		this._getIdHandler = getIdHandler == undefined ? this._defaultGetIdHandler : getIdHandler;
		this._setIdHandler = setIdHandler == undefined ? this._defaultSetIdHandler : setIdHandler;

		document.addEventListener('touchstart', this._onTouchStart, false);
		document.addEventListener('touchmove', this._onTouchMove, false);
		document.addEventListener('touchend', this._onTouchEnd, false);
	},

	_onTouchStart : function(event) {    
		MagicGesture._onNumberOfTouchesChanged(event, "ontouchstart");
	},

	_onTouchMove : function(event) {    
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
	    		var dx = group.touches[0].pageX - group.touches[1].pageX;
	    		var dy = group.touches[0].pageY - group.touches[1].pageY;
	    		var newDistance = Math.sqrt(dx * dx + dy * dy); 
	    		var newAngle = MagicGesture._angleBetweenVectors(group.initialVector, [dx, dy]);

	    		MagicGesture._RaiseEvent("gesturechange", MagicGesture._idToObjectHandler(id), newAngle, newDistance / group.initialDistance);
	    	}
	    }


	},

	_onTouchEnd : function(event) {    
		MagicGesture._onNumberOfTouchesChanged(event, "ontouchend"); 
	},

	/** Called by both onTouchStart and onTouchEnd */
	_onNumberOfTouchesChanged : function(event, caller){
	    MagicGesture._touchGroups = {};

	    for (var i = 0; i < event.touches.length; i++) {
	    	var element = MagicGesture._pointToObjectHandler(event.touches[i].pageX, event.touches[i].pageY);
	    	if (!element) continue;

	    	// Not all elements have an ID, if that's the case we'll create one
			var elementId = MagicGesture._getIdHandler(element);
			if(!elementId) {
			  MagicGesture._setIdHandler(element, PseudoGuid.GetNew());
			}

	    	if (MagicGesture._touchGroups[elementId] == undefined) MagicGesture._touchGroups[elementId] = {touches:[], initialDistance:0, initialVector:[]};

	    	MagicGesture._touchGroups[elementId].touches.push(event.touches[i]);
	    }

	    for (var id in MagicGesture._touchGroups){
	    	var group = MagicGesture._touchGroups[id];

	    	if (group.touches.length == 2 && !MagicGesture._touchStartedOn[id]){
	    		var dx = group.touches[0].pageX - group.touches[1].pageX;
	    		var dy = group.touches[0].pageY - group.touches[1].pageY;
	    		group.initialDistance = Math.sqrt(dx * dx + dy * dy);
	    		group.initialVector = [dx, dy];

	    		MagicGesture._RaiseEvent("gesturestart", MagicGesture._idToObjectHandler(id), 0, 0);
	    		MagicGesture._touchStartedOn[id] = true;
	    	}else if (group.touches.length != 2 && MagicGesture._touchStartedOn[id]){
	    		MagicGesture._RaiseEvent("gestureend", MagicGesture._idToObjectHandler(id), 0, 0);
	    		MagicGesture._touchStartedOn[id] = false;
	    	}
	    }
	},

	_RaiseEvent : function(name, element, rotation, scale){
		var evt = document.createEvent("Event");
		evt.initEvent(name, true, true);
		evt.rotation = rotation;
		evt.scale = scale;

		if (element.dispatchEvent) element.dispatchEvent(evt);
		else{
			evt._target = element;
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

	_defaultPointToObjectHandler : function(pageX, pageY){
		return document.elementFromPoint(pageX, pageY);
	},

	_defaultIdToObjectHandler : function(id){
		return document.getElementById(id);
	},

	_defaultGetIdHandler : function(element){
		return element.id;
	},

	_defaultSetIdHandler : function(element, id){
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


// // TODO: check the sign of the angle
// var v1 = [100, 0];
// var v2 = [0, 10];

// console.log(MagicGesture._angleBetweenVectors(v1, v2));