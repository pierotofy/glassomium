/** Magic Gesture

TODO LICENSE */

var MagicGesture = {};
MagicGesture._pointToObjectHandler = null;
MagicGesture._touchGroups = {};

/** Initializes magic gesture
 * @param pointToObjectHandler (optional) allows the user to specify a function that finds 
 * 		an object at certain screen coordinates. By default the document.elementFromPoint is used.
 *		It's useful to specify a custom handler for example when the application does the drawing on a canvas. */
MagicGesture.init = function(pointToObjectHandler){
	MagicGesture._pointToObjectHandler = pointToObjectHandler == undefined ? MagicGesture._defaultPointToObjectHandler : pointToObjectHandler;

	document.addEventListener('touchstart', MagicGesture._onTouchStart, false);
	document.addEventListener('touchmove', MagicGesture._onTouchMove, false);
	document.addEventListener('touchend', MagicGesture._onTouchEnd, false);
};

MagicGesture._onTouchStart = function(event) {    
    event.preventDefault();
    MagicGesture._touchGroups = {};

    for (var i = 0; i < event.touches.length; i++) {
    	var element = MagicGesture._pointToObjectHandler(event.touches[i].pageX, event.touches[i].pageY);
    	if (MagicGesture._touchGroups[element] == undefined) MagicGesture._touchGroups[element] = {touches:[], initialDistance:0};

    	MagicGesture._touchGroups[element].touches.push(event.touches[i]);
    }

    for (var i in MagicGesture._touchGroups){
    	var group = MagicGesture._touchGroups[i];
    	if (group.touches.length == 2){
    		var dx = group.touches[0].pageX - group.touches[1].pageX;
    		var dy = group.touches[0].pageY - group.touches[1].pageY;
    		group.initialDistance = Math.sqrt(dx * dx + dy * dy);

    		MagicGesture._RaiseEvent("gesturestart", i, 0, 0);
    	}
    }
};

MagicGesture._onTouchMove = function(event) {    
    event.preventDefault();


    for (var i = 0; i < event.touches.length; i++) {
    	// Update touches
	    for (var k in MagicGesture._touchGroups){
	    	var group = MagicGesture._touchGroups[k];
	    	for (var j in group.touches){
	    		if (group.touches[j].identifier == event.touches[i].identifier){
	    			group.touches[j] = event.touches[i];
	    			break;
	    		}
	    	}
	    }	
    }    

    // Scale gesture
    for (var i in MagicGesture._touchGroups){
    	var group = MagicGesture._touchGroups[i];
    	if (group.touches.length == 2){
    		GLA.Log("HERE");
    		var dx = group.touches[0].pageX - group.touches[1].pageX;
    		var dy = group.touches[0].pageY - group.touches[1].pageY;
    		var newDistance = Math.sqrt(dx * dx + dy * dy); 

    		MagicGesture._RaiseEvent("gesturechange", i, 0, newDistance / group.initialDistance);
    	}
    }
};

MagicGesture._onTouchEnd = function(event) {    
    event.preventDefault();
    MagicGesture._touchGroups = {};

    for (var i = 0; i < event.touches.length; i++) {
    	var element = MagicGesture._pointToObjectHandler(event.touches[i].pageX, event.touches[i].pageY);
    	if (MagicGesture._touchGroups[element] == undefined) MagicGesture._touchGroups[element] = {touches:[], distanceSquared:0};

    	MagicGesture._touchGroups[element].touches.push(event.touches[i]);
    }    


    GLA.Log("Touchend");
    GLA.Log(MagicGesture._touchGroups);
};

MagicGesture._RaiseEvent = function(name, element, rotation, scale){
	var evt = document.createEvent("Event");
	evt.initEvent(name, true, true);
	evt.target = element;
	evt.rotation = rotation;
	evt.scale = scale;
	document.dispatchEvent(evt);
};

/** @return the angle between two vectors in degrees */
MagicGesture._angleBetweenVectors = function(v1, v2){
	var v1_n = v1.slice(0); // Copy
	var v2_n = v2.slice(0); // Copy
	MagicGesture._normalizeVector(v1_n);
	MagicGesture._normalizeVector(v2_n);
	var bottom = Math.sqrt(MagicGesture._dotProduct(v1_n,v1_n))*Math.sqrt(MagicGesture._dotProduct(v2_n,v2_n));
	if (bottom != 0.0){
		var fraction = MagicGesture._dotProduct(v1_n,v2_n)/bottom;

		// acos is defined only from [-1, 1]
		if (fraction <= 1.0 && fraction >= -1.0){
			return Math.acos(fraction) * 57.2957796; // Radians to degrees
		}else{
			return 0.0;
		}
	}else{
		return 0.0;
	}
};

MagicGesture._dotProduct = function(v1, v2){
    return (v1[0]*v2[0] + v1[1]*v2[1]);
};

/** Destructive, normalize a vector */
MagicGesture._normalizeVector = function(v){
	var vector_length = Math.sqrt( v[0]*v[0] + v[1]*v[1]);
	if (vector_length > 0.0){
      v[0] /= vector_length;
      v[1] /= vector_length;
    }
  
    return vector_length;
};

MagicGesture._defaultPointToObjectHandler = function(pageX, pageY){
	return document.elementFromPoint(pageX, pageY);
};


// TODO: check the sign of the angle
var v1 = [100, 0];
var v2 = [0, -10];

console.log(MagicGesture._angleBetweenVectors(v1, v2));