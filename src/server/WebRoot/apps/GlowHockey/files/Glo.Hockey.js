var GloHockey = function () {
	var me = {
		zoom : function(zoom){
			zoomFactor = zoom;
			adjustMouseX = (375 * zoom) / 4;
			adjustMouseY = (175 * zoom) / 4;
		},

		Config:{
			fps:40,
			window:{x:$(window).width(), y:$(window).height()},
			map:{x:750, y:350},
			dimensions:{x:600, y:300},
			gravity:{x:0, y:0},
			polys:24,
			shapes:12,
			blur:0.5,
			control:'joint', // 'force', 'joint', 'ai'
			tween:JSTweener,
			create:CreateWorld(),
			draw:DrawWorld(),
			_pid:0
		},
		Styles:{
			clear: {fillStyle:'rgba(0,0,0,0)', strokeStyle:'rgba(0,0,0,0)', lineWidth:0, shadowBlur:0, shadowColor:'rgba(0,0,0,0)'},
			white: {fillStyle:'rgba(0,0,0,0)', strokeStyle:'rgba(255,255,255,0.5)', lineWidth:5, lineCap:'round', lineJoin:'round', shadowBlur:10, shadowColor:'rgba(255,255,255,0.5)'},
			explode: {fillStyle:'rgba(0,0,0,0)', strokeStyle:'rgba(255,255,255,0.5)', lineWidth:5, lineCap:'round', lineJoin:'round', shadowBlur:10, shadowColor:'rgba(255,255,255,0.5)'},
			red: {fillStyle:'rgba(0,0,0,0)', strokeStyle:'rgba(255,0,0,0.5)', lineWidth:5, shadowBlur:15, shadowColor:'rgba(255,0,0,0.5)'},
			green: {fillStyle:'rgba(0,0,0,0)', strokeStyle:'rgba(0,255,0,0.5)', lineWidth:5, shadowBlur:15, shadowColor:'rgba(0,255,0,0.5)'},
			blue: {fillStyle:'rgba(0,0,0,0)', strokeStyle:'#0000FF', lineWidth:5, shadowBlur:15, shadowColor:'#0000FF'}
		}
	};

	me.Mouse = {
		x:0,
		y:0,
		down:false,
		joint:null
	};
	
	me.Mouse2 = {
		x:0,
		y:0,
		down:false,
		joint:null
	};
	
	me.World = {};
	me.World.initiate = function () {
		me.World = me.Config.create.createWorld(me.Config.dimensions, me.Config.gravity, me.Config.polys, me.Config.shapes);
	};
	
	me.Hud = {};
	me.Hud.initiate = function () {
		
	}
	
	me.Initiate = function () {
		if((navigator.userAgent.match(/iPhone/i)) || (navigator.userAgent.match(/iPod/i))) { 
			me.Config.fps = 20;
			me.Config.blur = 0;
			me.Styles.white.shadowBlur = me.Styles.explode.shadowBlur = me.Styles.red.shadowBlur = me.Styles.green.shadowBlur = me.Styles.blue.shadowBlur = 0;
		}
	
		me.Config.padding = {left: (me.Config.map.x - me.Config.dimensions.x) * 0.5, top:(me.Config.map.y - me.Config.dimensions.y) * 0.5};
		
		$('canvas').attr({width:me.Config.map.x, height:me.Config.map.y})
		me.Canvas = $('canvas')[0];
        me.Context = me.Canvas.getContext('2d');

        me.Hud.initiate();
		me.World.initiate();
		me.Map.initiate();
		me.Events();
		me.Update();
	};
	me.Map = {};
	me.Map.initiate = function () {
		me.Ground = me.Config.create.createGround(me.World, -20, -20, 0, 0, me.Styles.clear);
		var bw = 40; // General Bar Width
		var bh = (me.Config.dimensions.y / 6) + 5;//Small Bar Widths (One third)
		var ml = me.Config.padding.left;
		var mt = me.Config.padding.top;
		
		//T
		me.Config.create.createBox(me.World, ml + me.Config.dimensions.x*0.5, mt + 0, me.Config.dimensions.x*0.5, bw, 0, true, me.Styles.clear);
		
		//B
		me.Config.create.createBox(me.World, ml + me.Config.dimensions.x*0.5, mt + me.Config.dimensions.y, me.Config.dimensions.x*0.5, bw, 0, true, me.Styles.clear);
		//L
		me.Config.create.createBox(me.World, ml + 0 - (bw*0.5), mt + bh, bw, bh, 0, true, me.Styles.clear);
		me.Config.create.createBox(me.World, ml + 0 - (bw*0.5), mt + me.Config.dimensions.y - bh, bw, bh, 0, true, me.Styles.clear);
		//R
		me.Config.create.createBox(me.World, ml + me.Config.dimensions.x + (bw*0.5), mt + bh, bw, bh, 0, true, me.Styles.clear);
		me.Config.create.createBox(me.World, ml + me.Config.dimensions.x + (bw*0.5), mt + me.Config.dimensions.y - bh, bw, bh, 0, true, me.Styles.clear);
		
		//Ents
		me.Players = [];
		me.Map.Entities = [];
		me.Map.Particles = [];
		
		me.Map.Entities[0] = me.Players[0] = me.Config.create.createCircle(me.World, ml + 100, mt + 150, 30, 0, false, false, me.Styles.green);
		me.Players[0].m_mass *= 10;
		me.Players[0].m_points = 0;
		me.Players[0].m_control = 'joint';
		me.Players[0].m_joint = me.Config.create.createMouseJoint(me.World, me.Players[0], me.Players[0].m_position.x, me.Players[0].m_position.y);
		
		me.Map.Entities[1] = me.Players[1] = me.Config.create.createCircle(me.World, ml + 500, mt + 150, 30, 0, false, false, me.Styles.red);
		me.Players[1].m_mass *= 10;
		me.Players[1].m_points = 0;
		me.Players[1].m_control = 'joint';
		me.Players[1].m_joint = me.Config.create.createMouseJoint(me.World, me.Players[1], me.Players[1].m_position.x, me.Players[1].m_position.y);
		
		me.Map.Entities[2] = me.Puck = me.Config.create.createCircle(me.World, ml + 300, mt + 150, 20, 0, false, false, me.Styles.blue);
		//me.Puck.m_mass = 0;
			
	};
	me.Reset = function () {
		
	};
	me.Events = function () {
		$(window).resize(function (event) {
			me.Config.window.x = $(window).width();
			me.Config.window.y = $(window).height();
		});
		$(document).noContext();
		
		$(document).bind('touchstart',function (event) {
		    var touches = event.originalEvent.touches || event.originalEvent.changedTouches;

		    for (var i = 0; i < touches.length; i++){
		    	var touch = touches[i];
			    var mouse;
			    if (touch.pageX < window.innerWidth / 2){
			    	mouse = me.Mouse;
			    }else{
			    	mouse = me.Mouse2;
			    }

				mouse.down = true;
				mouse.touchId = touch.identifier;
			}
		});
		$(document).bind('touchend',function (event) {
		    var touches = event.originalEvent.touches || event.originalEvent.changedTouches;

		    if (touches.length == 0){
		    	me.Mouse.touchId = undefined;
		   		me.Mouse2.touchId = undefined;    		
		    }

		    for (var i = 0; i < touches.length; i++){
		    	var touch = touches[i];
			    var mouse;
			    if (me.Mouse.touchId == touch.identifier){
			    	mouse = me.Mouse;
			    }else{
			    	mouse = me.Mouse2;
			    }

				mouse.down = false;
				mouse.touchId = undefined;
			}
		});
		$(document).bind('touchmove',function (event) {
		    event.preventDefault();

		    var touches = event.originalEvent.touches || event.originalEvent.changedTouches;

		    for (var i = 0; i < touches.length; i++){
		    	var touch = touches[i];
			    var mouse;
			    if (me.Mouse.touchId == touch.identifier){
			    	mouse = me.Mouse;
			    }else{
			    	mouse = me.Mouse2;
			    }

			    mouse.x = touch.pageX;
				mouse.y = touch.pageY;
				
				mouse.x -= me.Config.window.x * 0.5 - me.Config.map.x * 0.5;
				mouse.y -= me.Config.window.y * 0.5 - (me.Config.map.y) * 0.5;

				// Adjustments
				mouse.x /= zoomFactor;
				mouse.y /= zoomFactor;		

				mouse.x += adjustMouseX;	
				mouse.y += adjustMouseY;	
						
			}
		});
		
	};
	me.MovePlayer = function () {
		if (me.Players[0].m_control == 'joint') {
			if (me.Players[0].m_joint) {
				var ml = me.Config.padding.left;
				var mt = me.Config.padding.top;
				var p1 = new b2Vec2(me.Mouse.x,me.Mouse.y);
				if (p1.x > ml + (me.Config.dimensions.x * 0.5)) {
					p1.x = ml + (me.Config.dimensions.x * 0.5) - 3;
				}
				if (p1.x < ml) {
					p1.x = ml + 3;
				}
				me.Players[0].m_joint.SetTarget(p1);
			} else {
				var impulse = new b2Vec2( ( me.Mouse.x - me.Mouse.ox ) * 10000, ( me.Mouse.y - me.Mouse.oy ) * 10000);
				me.Players[0].ApplyImpulse(impulse, me.Players[0].m_position);
			}
		}
		me.Mouse.ox = me.Mouse.x;
		me.Mouse.oy = me.Mouse.y;
	};
	me.MovePlayer2 = function () {
		if (me.Players[1].m_control == 'joint') {
			if (me.Players[1].m_joint) {
				var mr = me.Config.padding.right;
				var ml = me.Config.padding.left;
				var mt = me.Config.padding.top;
				var p1 = new b2Vec2(me.Mouse2.x,me.Mouse2.y);
				if (p1.x < ml + (me.Config.dimensions.x * 0.5)) {
					p1.x = ml + (me.Config.dimensions.x * 0.5) + 3;
				}
				if (p1.x > me.Config.dimensions.x + ml) {
					p1.x = me.Config.dimensions.x + ml - 3;
				}
				me.Players[1].m_joint.SetTarget(p1);
			} else {
				var impulse = new b2Vec2( ( me.Mouse2.x - me.Mouse2.ox ) * 10000, ( me.Mouse2.y - me.Mouse2.oy ) * 10000);
				me.Players[1].ApplyImpulse(impulse, me.Players[1].m_position);
			}
		}
		me.Mouse2.ox = me.Mouse2.x;
		me.Mouse2.oy = me.Mouse2.y;
	};
	me.Suction = function () {
		var vec = {
			x: me.Puck.m_position.x - (me.Config.map.x * 0.5),
			y: me.Puck.m_position.y - (me.Config.map.y * 0.5),
		};
		me.Puck.ApplyForce(new b2Vec2(vec.x * -50, vec.y * -50), me.Puck.m_position);
		
		//Slowing it down, too
		me.Puck.m_linearVelocity.x = me.Puck.m_linearVelocity.x -  (me.Puck.m_linearVelocity.x - 0) * 0.0033;
		me.Puck.m_linearVelocity.y = me.Puck.m_linearVelocity.y -  (me.Puck.m_linearVelocity.y - 0) * 0.0033;
	}
	me.Camera = function () {
		
		//me.Context.translate(me.Config.window.x * 0.5 - me.Config.dimensions.x * 0.5, me.Config.window.y * 0.5 - me.Config.dimensions.y * 0.5);
	};
	me.Draw = function () {
		me.Context.save();
		
		me.Blur(me.Config.blur);
		me.Camera();
		me.Foreground(me.Context);
		me.Collisions();
		me.DrawParticles();
		
		me.Config.draw.draw(me.World, me.Context);
		
		me.Context.restore();
	};
	me.DrawParticles = function () {
		for (var p in me.Map.Particles) {
			if (me.Map.Particles[p]) {
				_particle(me.Map.Particles[p], me.Context);
			}
		}
	};
	
	me.Goal = function (player) {
		var pos = me.Puck.m_position;
		var ml = me.Config.padding.left;
		var mt = me.Config.padding.top;
		var cl = '';
		var style = null
		var place = 300;
		
		// XXX Logic = NULL
		if (player == me.Players[0]) {
			place = 390;
			cl = 'p1';
		} else{
			place = 210;
			cl = 'p2';
		}
		
		style = player.m_style;
		_particleBurst(pos.x, pos.y, 100, 0.5, style);
		_particleExplode(pos.x, pos.y,300, 1, 50, style);
		
		me.Puck.m_position.x =  ml + place;
		me.Puck.m_position.y =  mt + 150;
		me.Puck.m_linearVelocity.x = 0;
		me.Puck.m_linearVelocity.y = 0;

		
		//me.World.DestroyBody(me.Puck);
		//me.Map.Entities[2] = me.Puck = me.Config.create.createCircle(me.World, ml + place, mt + 150, 20, 0, false, false, me.Styles.blue);
		
		player.m_points += 1;

		//alert('GOAL!');
	};
	me.Collisions = function () {
		var pcl = me.Puck.GetContactList();	
		if (pcl && pcl.contact) {
			var style = null;
			if (pcl.other) {
				//console.log(pcl.other);
				if (pcl.other.IsStatic()) {
				//	console.log('wall');
				} else {
					style = pcl.other.m_style;
					me.Puck.m_lastCollision = pcl;
				}
			}
		
			var pt = pcl.contact.m_manifold[0].points[0];
			var pos = pt.position;
			var impulse = pt.normalImpulse;
			
			if (impulse > 200000) {
				_particleBurst(pos.x, pos.y, 30, 0.222, style);
				_particleExplode(pos.x, pos.y, 90, 0.333, Math.round(impulse / 100000), style);
			}
		}
		
		var p = me.Puck;
		var ml = me.Config.padding.left;
		var mt = me.Config.padding.top;
		
		//XXX Logic = NULL
		if (p.m_position.x < ml - p.m_width) {
			me.Goal (me.Players[1]);
		}
		if (p.m_position.x > (ml) + me.Config.dimensions.x + p.m_width) {
			me.Goal (me.Players[0]);
		}
	};
	
	//Massive Illustratror -> Canvas Rip here... new maps + loading soon I promise... (will be as ugly as the blob below at first though :P)
	me.Foreground = function (ctx) {
		var ml = me.Config.padding * 0.5;
		var mt = me.Config.padding.top;
	
		var alpha = ctx.globalAlpha;
		ctx.translate(15, mt); //Blah offset fix
		// layer2/Group
		ctx.globalAlpha = alpha * 0.25;
		//ctx.shadowBlur = 15;
		//ctx.shadowColor = "rgb(255, 255, 255)";
		
		// layer2/Group/Path
		ctx.save();
		ctx.beginPath();
		ctx.moveTo(358.9, 22.5);
		ctx.lineTo(358.9, 281.5);
		ctx.lineWidth = 5.0;
		ctx.strokeStyle = "rgb(255, 255, 255)";
		ctx.stroke();
		
		// layer2/Group/Path
		ctx.beginPath();
		ctx.moveTo(715.3, 152.5);
		ctx.bezierCurveTo(715.3, 183.4, 690.3, 208.5, 659.4, 208.5);
		ctx.bezierCurveTo(628.5, 208.5, 603.4, 183.4, 603.4, 152.5);
		ctx.bezierCurveTo(603.4, 121.6, 628.5, 96.5, 659.4, 96.5);
		ctx.bezierCurveTo(690.3, 96.5, 715.3, 121.6, 715.3, 152.5);
		ctx.closePath();
		ctx.stroke();
		
		// layer2/Group/Path
		ctx.beginPath();
		ctx.moveTo(452.5, 152.5);
		ctx.bezierCurveTo(452.5, 204.4, 410.4, 246.5, 358.5, 246.5);
		ctx.bezierCurveTo(306.5, 246.5, 264.5, 204.4, 264.5, 152.5);
		ctx.bezierCurveTo(264.5, 100.6, 306.5, 58.5, 358.5, 58.5);
		ctx.bezierCurveTo(410.4, 58.5, 452.5, 100.6, 452.5, 152.5);
		ctx.closePath();
		ctx.stroke();
		
		// layer2/Group/Path
		ctx.beginPath();
		ctx.moveTo(114.4, 152.5);
		ctx.bezierCurveTo(114.4, 183.4, 89.4, 208.5, 58.5, 208.5);
		ctx.bezierCurveTo(27.6, 208.5, 2.5, 183.4, 2.5, 152.5);
		ctx.bezierCurveTo(2.5, 121.6, 27.6, 96.5, 58.5, 96.5);
		ctx.bezierCurveTo(89.4, 96.5, 114.4, 121.6, 114.4, 152.5);
		ctx.closePath();
		ctx.stroke();
		
		// layer2/Path
		ctx.restore();
		ctx.globalAlpha = alpha * 1.00;
		ctx.beginPath();
		ctx.moveTo(79.5, 204.4);
		ctx.lineTo(79.5, 266.5);
		ctx.bezierCurveTo(79.5, 274.8, 86.2, 281.5, 94.5, 281.5);
		ctx.lineTo(624.5, 281.5);
		ctx.bezierCurveTo(632.7, 281.5, 639.5, 274.8, 639.5, 266.5);
		ctx.lineTo(639.5, 204.8);
		ctx.bezierCurveTo(639.5, 204.8, 649.4, 209.4, 659.4, 208.5);
		ctx.lineTo(659.4, 288.5);
		ctx.bezierCurveTo(659.4, 296.7, 652.7, 303.5, 644.4, 303.5);
		ctx.lineTo(73.5, 303.5);
		ctx.bezierCurveTo(65.2, 303.5, 58.5, 296.7, 58.5, 288.5);
		ctx.lineTo(58.5, 208.5);
		ctx.bezierCurveTo(58.5, 208.5, 71.3, 208.5, 79.5, 204.4);
		ctx.closePath();
		ctx.lineWidth = 5.0;
		ctx.strokeStyle = "rgb(255, 255, 255)";
		ctx.stroke();
		
		// layer2/Path
		ctx.beginPath();
		ctx.moveTo(79.5, 100.6);
		ctx.lineTo(79.5, 37.5);
		ctx.bezierCurveTo(79.5, 29.2, 86.2, 22.5, 94.5, 22.5);
		ctx.lineTo(624.5, 22.5);
		ctx.bezierCurveTo(632.7, 22.5, 639.5, 29.2, 639.5, 37.5);
		ctx.lineTo(639.5, 100.2);
		ctx.bezierCurveTo(648.3, 95.9, 659.4, 96.5, 659.4, 96.5);
		ctx.lineTo(659.4, 17.5);
		ctx.bezierCurveTo(659.4, 9.2, 652.7, 2.5, 644.4, 2.5);
		ctx.lineTo(73.5, 2.5);
		ctx.bezierCurveTo(65.2, 2.5, 58.5, 9.2, 58.5, 17.5);
		ctx.lineTo(58.5, 96.5);
		ctx.bezierCurveTo(58.5, 96.5, 70.9, 96.2, 79.5, 100.6);
		ctx.closePath();
		ctx.lineCap = "round";
		ctx.stroke();
		
		ctx.translate(-15, -mt); //Blah offset fix
	};
	me.Blur = function (amt) {
		if (amt) {
			me.Context.moveTo(0,0);
			me.Context.rect(0, 0, me.Config.map.x, me.Config.map.y);
			me.Context.fillStyle = 'rgba(0,0,0,' + amt + ')';
			me.Context.fill();
		} else {
			me.Context.clearRect(0,0,me.Config.map.x, me.Config.map.y);
		}
	};
	me.Update = function () {
		//Bounds
		var ml = me.Config.padding.left;
		var mt = me.Config.padding.top;
		var p1 = me.Players[0];
		var p2 = me.Players[1];
		
		if (p1.m_position.x > ml + (me.Config.dimensions.x * 0.5) - p1.m_width) {
			//p1.m_position.x = ml + (me.Config.dimensions.x * 0.5) - p1.m_width - 1;
			p1.m_linearVelocity.x = -1;
		}
		if (p1.m_position.x < ml + 0 + p1.m_width * 2) {
			p1.m_linearVelocity.x = 1;
		}
		if (p2.m_position.x < ml + (me.Config.dimensions.x * 0.5) + p2.m_width) {
			p2.m_linearVelocity.x = 1;
		}
		if (p2.m_position.x > ml + me.Config.dimensions.x - p2.m_width * 2) {
			p2.m_linearVelocity.x = -1;

		}
		
		//MouseJoint
		if (me.Mouse.down) {
			me.MovePlayer();
		}

		if (me.Mouse2.down){
			me.MovePlayer2();
		}
	
		me.World.Step(1 / me.Config.fps, 1);
		me.Draw();
		me.Suction();
		
        setTimeout(function () {
            me.Update();
        }, 1000 / me.Config.fps);
	};
	_point = function (x, y, width, style, ctx) {
		me.Config.draw.style(ctx, style);
		ctx.closePath();
		ctx.beginPath();
		ctx.arc(x, y, width, 0, Math.PI * 2, false);
		
		ctx.closePath();
		ctx.fill();
		ctx.stroke();
	};
	_particle = function (particle, ctx) {
		me.Config.draw.style(ctx, particle.style);
		ctx.closePath();
		ctx.beginPath();
		ctx.arc(particle.x, particle.y, particle.width, 0, Math.PI * 2, false);
		
		ctx.closePath();
		ctx.fill();
		ctx.stroke();
	};
	_particleExplode = function (x,y, width, time, amount, style) {
		var st = (style) ? style : me.Styles.explode;
		for (var a = 0; a < amount; a++) {
			var vec = {x: ((Math.random() * 2) - 1) * width, y: ((Math.random() * 2) - 1) * width};
			particle = {id: 'particle_' + me.Config._pid, width: 1, x: x, y: y, style: st};
			me.Config.tween.addTween(particle, {
				x:x + vec.x,
				y:y + vec.y,
				time: Math.random() * time,
				transition: 'linear',
				onComplete: function () {me.Map.Particles[this.target.id] = null;},
			});
			me.Map.Particles[particle.id] = particle;
			me.Config._pid ++;
		}
	};
	_particleBurst = function (x,y, width, time, style) {
		var st = (style) ? style : me.Styles.explode;
		particle = {id: 'particle_' + me.Config._pid, width: 0, x: x, y: y, style: st};
		me.Config.tween.addTween(particle, {
			width: width,
			time: time,
			transition: 'linear',
			onComplete: function () {me.Map.Particles[this.target.id] = null;},
		});
		me.Map.Particles[particle.id] = particle;
		me.Config._pid ++;
	};
	_strokesAt = function (x,y) {
		var strokesAtThisPoint = new Array();
		for (var b = me.World.m_bodyList; b; b = b.m_next) {
			var test = false;
			for (var shape = b.GetShapeList(); shape != null; shape = shape.GetNext()) {
				if (shape.TestPoint(new b2Vec2(x,y))) {
				  test = true;
				}
			}
			if (test) {
			  strokesAtThisPoint.push(b); 
			};
			
		}
		return strokesAtThisPoint;
	};

	me.Initiate();
	return me;
};