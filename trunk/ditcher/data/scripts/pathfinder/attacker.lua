function attacker.check()
	if not players then return end
	attacker.target = {}
	attacker.target.id = -1
	local dist = 99999
	local d
	for i = 0, playerscount - 1 do
		if players[i] and players[i].team ~= my.team then
			d = sqrdistance(my.x, my.y, players[i].x, players[i].y)
			if d < dist then
				attacker.target.id = i
				attacker.target.dist = d
				dist = d
			end
		end
	end
	if attacker.target.id == -1 then
		attacker.target = nil
		return false
	else
		local pl = players[attacker.target.id]
		attacker.target.loc = {x = pl.x, y = pl.y}
		attacker.target.lastseen = 99999
		attacker.recheck = math.sqrt(dist) / 4
		attacker.ispath = 0
		state = STATE_ATTACK
		return true
	end
end

function attacker.before()
	local targ = attacker.target
	targ.inscope = attacker.inscope(nil)
	if targ.inscope then
		targ.lastseen = 0		
		targ.lastpoint = { x = targ.loc.x, y = targ.loc.y }
	else
		targ.lastseen = targ.lastseen + 1
	end
	if not attacker.path then
		struggle()
	elseif targ.lastseen < 40 then
		if my.speed < 0.01 and waskey(UP) then
			struggle()
		else gostraight(targ.lastpoint) end
	end
end


function attacker.core()
	if attacker.target.dead then
		attacker.target = nil
		decideaction()
		return
	end
	local pl = players[attacker.target.id]
	if not attacker.path or attacker.recheck < 0 then
		if pl then attacker.target.loc = {x = pl.x, y = pl.y } end
		local loc = attacker.target.loc
		attacker.path = findpath(my, loc, 0.2, 1)
		if not attacker.path then
			attacker.ispath = attacker.ispath + 1
			if attacker.ispath >= 20 then decideaction() return end
		else
			attacker.ispath = 0
			attacker.recheck = distance(my.x, my.y, loc.x, loc.y) / 4
		end
	end
	if attacker.target.lastseen >= 40 and attacker.path then
		keybmask = 0
		attacker.path = followpath(attacker.path)
		attacker.recheck = attacker.recheck - 1
	end
	if not returner.wellbeing(1 / 3, 1 / 5) and
		sqrdistance(my.x, my.y, my.home.x, my.home.y) > sqr(50) then
		state = STATE_RETURN
		returner.switched = true
	end
end

function attacker.after()
	if state ~= STATE_ATTACK then return end
	if pointblanc == 1 or attacker.target.inscope then
		setkey(FIRE)
	end
end

function attacker.inscope(index)

	local loc
	if players[index] then
		loc = { x = players[index].x, y = players[index].y }
	elseif index == nil then
		loc = attacker.target.loc
	else return false end
	
	if loc.x > my.x + map.w / 2 then loc.x = loc.x + map.w
	elseif loc.x < my.x - map.w / 2 then loc.x = loc.x - map.w end
	if loc.y > my.y + map.h / 2 then loc.y = loc.y + map.h
	elseif loc.y < my.y - map.h / 2 then loc.y = loc.y - map.h end
	
	local dist	   = distance(my.x, my.y, loc.x, loc.y)
	local targ	   = { x = my.x + my.xp * dist,
		y = my.y + my.yp * dist }
	local tarsqrdist = sqrdistance(loc.x, loc.y, targ.x, targ.y)

	if tarsqrdist <= sqr(ROBOT_R * 1.2) then
		local muds = 0
		local point
		for i = ROBOT_R, dist do
			point = at(my.x + my.xp * i, my.y + my.yp * i)
			if point ~= 0 then
				if point == 1 then
					muds = muds + 1
					if muds > 24 then
						return false
					end
				elseif point == 2 then
					return false
				end
			end
		end
		return true
	else
		return false
	end
end

function shotdanger(n)
	local i

	if n < shotscount then
		local x  = shots_x[n]
		local y  = shots_y[n]
		local xp = shots_xp[n]
		local yp = shots_yp[n]

		local dist = distance(x, y, my.x, my.y)
		local tx   = x + dist * xp
		local ty   = y + dist * yp

		if sqrdistance(tx, ty, my.x, my.y) <= sqr(ROBOT_R + 3) then
			for i = 0, dist - (ROBOT_R + 3) do
				if at(x + i * xp, y + i * yp) ~= 0 then
					return false
				end
			end
			return true
		end
	end
	return false
end

attacker.main = coroutine.wrap(
	function()
		while true do
			coroute = true
			attacker.core()
			coroute = false
			coroutine.yield(actionmask)
		end
	end
)
