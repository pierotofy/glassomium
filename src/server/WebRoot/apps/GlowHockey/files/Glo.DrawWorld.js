var DrawWorld = function() {
	var me = {};
	
	me.draw = function(world, context) {
	    /*
	    for (var j = world.m_jointList; j; j = j.m_next) {
	        me.drawJoint(world, j, context)
	    }
	    */
	    for (var b = world.m_bodyList; b; b = b.m_next) {
	    	var b_style = (b.m_style) ? b.m_style : null;
	        for (var s = b.GetShapeList(); s != null; s = s.GetNext()) {
	        	me.drawShape(world, s, context, b_style);
	        }
	    }
	}
	
	me.drawJoint = function(world, joint, context) {
	    var b1 = joint.m_body1
	    var b2 = joint.m_body2
	    var x1 = b1.m_position
	    var x2 = b2.m_position
	    var p1 = joint.GetAnchor1()
	    var p2 = joint.GetAnchor2()
	    context.beginPath()
	    switch (joint.m_type) {
	    case b2Joint.e_distanceJoint:
	        context.moveTo(p1.x, p1.y)
	        context.lineTo(p2.x, p2.y)
	        break
	    case b2Joint.e_pulleyJoint:
	        break
	    default:
	        if (b1 == world.m_groundBody) {
	            context.moveTo(p1.x, p1.y)
	            context.lineTo(x2.x, x2.y)
	        } else if (b2 == world.m_groundBody) {
	            context.moveTo(p1.x, p1.y)
	            context.lineTo(x1.x, x1.y)
	        } else {
	            context.moveTo(x1.x, x1.y)
	            context.lineTo(p1.x, p1.y)
	            context.lineTo(x2.x, x2.y)
	            context.lineTo(p2.x, p2.y)
	        }
	        break
	    }
	    context.stroke()
	    context.closePath();
	}
	
	me.drawShape = function(world, shape, context, b_style) {
	    context.beginPath()
	    switch (shape.m_type) {
	    case b2Shape.e_circleShape:
	        {
	            var circle = shape
	            
	            var pos = circle.m_position
	            
	            var r = circle.m_radius
	            var segments = 16.0
	            var theta = 0.0
	            var dtheta = 2.0 * Math.PI / segments
	            
	            context.arc(pos.x, pos.y, r, 0, Math.PI * 2, false)
	            if (shape.m_body.IsStatic() == false) {
	                context.moveTo(pos.x, pos.y)
	                var ax = circle.m_R.col1
	                var pos2 = new b2Vec2(pos.x + r * ax.x, pos.y + r * ax.y)
	                //context.lineTo(pos2.x, pos2.y)
	            }
	            context.closePath();
	        }
	        break
	    case b2Shape.e_polyShape:
	        {
	            var poly = shape
	            var tV = b2Math.AddVV(poly.m_position, b2Math.b2MulMV(poly.m_R, poly.m_vertices[0]))
	            context.moveTo(tV.x, tV.y)
	            for (var i = 0; i < poly.m_vertexCount; i++) {
	                var v = b2Math.AddVV(poly.m_position, b2Math.b2MulMV(poly.m_R, poly.m_vertices[i]))
	                context.lineTo(v.x, v.y)
	            }
	            context.lineTo(tV.x, tV.y)
	    		context.closePath();
	        }
	        break
	    }
	    me.style(context, b_style);
	    context.fill()
	    context.stroke()
	}
	
	me.style = function(context, props) {
		if (props) {
			context.fillStyle = (props.fillStyle) ? props.fillStyle : 'rgba(0,0,0,255)';
		    context.strokeStyle = (props.strokeStyle) ? props.strokeStyle : 'rgba(0,0,0,255)';
		    context.lineCap = (props.lineCap) ? props.lineCap : 'butt';
		    //context.lineJoin = (props.lineJoin) ? props.lineJoin : 'butt';
		    context.lineWidth = (props.lineWidth) ? props.lineWidth : 0;
		    context.shadowColor = (props.shadowColor) ? props.shadowColor : 'rgba(0,0,0,255)';
		    context.shadowOffsetX = (props.shadowOffsetX) ? props.shadowOffsetX : 0;
		    context.shadowOffsetY = (props.shadowOffsetY) ? props.shadowOffsetY : 0
		    context.shadowBlur = (props.shadowBlur) ? props.shadowBlur : 0;
	    }
	}
	
	me.cullLines = function(points, num, div) {
	    var cullPoints = new Array()
	    var divnum = num * div
	    if (points.length > divnum) {
	        var mod = Math.round(points.length / divnum)
	        for (var i = 0; i < points.length; i++) {
	            if (i % mod == 0) {
	                var npt = points[i]
	                npt.X = Math.round(npt.X * 10) / 10
	                npt.Y = Math.round(npt.Y * 10) / 10
	                cullPoints.push(npt)
	            }
	        }
	    } else {
	        return points
	    }
	    return me.cullLines(cullPoints, num, div * 2)
	}
	
	return me;
}
