var CreateWorld = function() {
	var me = {};
	
	me.createWorld = function(dimensions, gravity, polys, shapes) {
	    var worldAABB = new b2AABB()
	    worldAABB.minVertex.Set(-100, -100)
	    worldAABB.maxVertex.Set(dimensions.x + 100, dimensions.y + 100)
	    var gravity = new b2Vec2(gravity.x, gravity.y)
	    var doSleep = true
	    var world = new b2World(worldAABB, gravity, doSleep)
	    b2Settings.b2_maxPolyVertices = polys
	    b2Settings.b2_maxShapesPerBody = shapes
	    return world
	}
	
	me.createGround = function(world, x, y, w, h, style) {
	    var groundSd = new b2BoxDef()
	    groundSd.extents.Set(w, h)
	    groundSd.restitution = 0.2
	    var groundBd = new b2BodyDef()
	    groundBd.AddShape(groundSd)
	    groundBd.position.Set(x, y)
	    var b = world.CreateBody(groundBd);
	    b.m_style = style
	    return b
	}
	
	me.createCircle = function(world, x, y, radius, rotation, preventRotation, fixed, style) {
	    var circleSd = new b2CircleDef()
	    if (!fixed) circleSd.density = 1.0
	    circleSd.radius = radius
	    circleSd.friction = 0.111
	    circleSd.restitution = 0.666
	    var circleBd = new b2BodyDef()
	    circleBd.AddShape(circleSd)
	    circleBd.position.Set(x, y)
	    circleBd.rotation = rotation
	    if (preventRotation) {
	        circleBd.preventRotation = true
	    }
	    circleBd.m_userData = {
	        def: circleSd
	    }
	    var b = world.CreateBody(circleBd)
	    b.m_style = style
	    b.m_width = radius
	    b.m_height = radius
	    return b
	}
	
	me.createBox = function(world, x, y, width, height, rotation, fixed, style) {
	    if (typeof(fixed) == 'undefined') fixed = true
	    var boxSd = new b2BoxDef()
	    if (!fixed) boxSd.density = 1.0
	    boxSd.extents.Set(width, height)
	    boxSd.friction = 0.222
	    boxSd.restitution = 0.666
	    var boxBd = new b2BodyDef()
	    boxBd.AddShape(boxSd)
	    boxBd.position.Set(x, y)
	    boxBd.rotation = rotation
	    boxBd.m_userData = {
	        def: boxSd
	    }
	    var b = world.CreateBody(boxBd)
	    b.m_style = style
	    b.m_width = width
	    b.m_height = height
	    return b
	}
	
	me.createLines = function(world, x, y, points, rotation, fixed) {
	    if (typeof(fixed) == 'undefined') fixed = true
	    var linesBd = new b2BodyDef()
	    var negX = 0
	    var negY = 0
	    var posX = 0
	    var posY = 0
	    for (var i = 1; i < points.length; i++) {
	        var p1 = points[i - 1]
	        var p2 = points[i]
	        var w = new b2Vec2(p2.Y - p1.Y, p1.X - p2.X)
	
	        if (p2.X > posX) {
	            posX = Math.round(p2.x);
	        }
	        if (p2.Y > posY) {
	            posY = Math.round(p2.y);
	        }
	        if (p2.X < negX) {
	            negX = Math.round(p2.X)
	        }
	        if (p2.Y < negY) {
	            negY = Math.round(p2.Y)
	        }
	
	        w.Normalize()
	        w.Multiply(1.5)
	        var lineSd = new b2PolyDef()
	        if (!fixed) lineSd.density = 1.0
	        lineSd.vertexCount = 4
	        lineSd.restitution = 0.25
	        lineSd.friction = 0.5
	        lineSd.vertices[0].Set(p1.X, p1.Y)
	        lineSd.vertices[1].Set(p2.X, p2.Y)
	        lineSd.vertices[2].Set(p2.X - w.x, p2.Y - w.y)
	        lineSd.vertices[3].Set(p1.X - w.x, p1.Y - w.y)
	        linesBd.AddShape(lineSd)
	    }
	    linesBd.rotation = rotation
	    linesBd.position.Set(x, y)
	    linesBd.m_userData = {
	        def: lineSd
	    }
	    var b = world.CreateBody(linesBd);
	    b.m_width = Math.abs(posX - negX);
	    b.m_height = Math.abs(posY - negY);
	    return b
	}
	
	me.createPolyShape = function(world, x, y, points, rotation, fixed) {
	    if (typeof(fixed) == 'undefined') fixed = true
	    var polySd = new b2PolyDef()
	    polySd.vertexCount = points.length
	    if (!fixed) polySd.density = 1.0
	    polySd.restitution = 0.25
	    polySd.friction = 0.5
	    var negX = 0
	    var negY = 0
	    var posX = 0
	    var posY = 0
	    var flip = false
	    for (var n = 0; n < points.length; n++) {
	        var n1 = points[n]
	        if (n1.X > posX) {
	            posX = Math.round(n1.x);
	        }
	        if (n1.Y > posY) {
	            posY = Math.round(n1.y);
	        }
	        if (n1.X < negX) {
	            negX = Math.round(n1.X)
	        }
	        if (n1.Y < negY) {
	            negY = Math.round(n1.Y)
	        }
	    }
	    negX = Math.round(Math.round(negX / 50) * 50)
	    negY = Math.round(Math.round(negY / 50) * 50)
	    if (negX < 0 && negY < 0) {
	        points.reverse()
	    } else if (negX == 0 && negY == 0) {
	        points.reverse()
	    } else if (negX < 0 && negY == 0) {
	        points.reverse()
	    } else if (flip) {
	        points.reverse()
	    }
	    for (var i = 0; i < points.length; i++) {
	        var p1 = points[i]
	        p1.X = Math.ceil(p1.X)
	        p1.Y = Math.ceil(p1.Y)
	        polySd.vertices[i].Set(p1.X, p1.Y)
	    }
	    var polyBd = new b2BodyDef()
	    polyBd.rotation = rotation
	    polyBd.position.Set(Math.round(x), Math.round(y))
	    polyBd.m_width = Math.abs(posX - negX);
	    polyBd.m_height = Math.abs(posY - negY);
	    polyBd.AddShape(polySd)
	    polyBd.m_userData = {
	        def: polySd
	    }
	    return world.CreateBody(polyBd)
	}
	
	me.createDistanceJoint = function(world, x1, y1, x2, y2, body1, body2) {
	    var jointDef = new b2DistanceJointDef()
	    jointDef.body1 = body1
	    jointDef.body2 = body2
	    jointDef.anchorPoint1.Set(x1, y1)
	    jointDef.anchorPoint2.Set(x2, y2)
	    jointDef.maxForce = 1500 * body1.m_mass
	    var joint = world.CreateJoint(jointDef)
	    joint.m_userData = {
	        def: jointDef
	    }
	    return joint
	}
	
	me.createRevoluteJoint = function(world, x, y, body1, body2) {
	    var jointDef = new b2RevoluteJointDef()
	    jointDef.anchorPoint.Set(x, y)
	    jointDef.body1 = body1
	    jointDef.body2 = body2
	    var joint = world.CreateJoint(jointDef)
	    joint.m_userData = {
	        def: jointDef
	    }
	    return joint
	}
	
	me.createMouseJoint = function(world, body, x, y) {
	    var jointDef = new b2MouseJointDef()
	    jointDef.body1 = world.GetGroundBody()
	    jointDef.body2 = body
	    jointDef.target.Set(x, y)
	    jointDef.maxForce = 1000.0 * body.m_mass
	    //jointDef.dampingRatio = 0.333;
	    //jointDef.frequencyHz = 5;
		//jointDef.timeStep = 0.01666;
		
	    var joint = world.CreateJoint(jointDef)
	    joint.m_userData = {
	        def: jointDef
	    }
	    return joint
	}
	
	return me;
}
