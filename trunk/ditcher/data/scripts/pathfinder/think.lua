require("aux")

require("mask")

require("init")

require("acquire")

require("check")

require("movement")

-- this is the core function which is called by the game and expected
--	 to return an or-combination of action keys
function think()

	-- reset action mask that will be returned to game
	actionmask = 0

	-- equivalent to chronos = chronos + 1
	chronos = gettime()

	-- acquire information about robot's location and status
	my.x , my.y, my.xp, my.yp, my.angle, my.speed,
		my.health, my.energy, _, my.grave,
		my.weapon, my.reload  = getmyinfo()
	my.xd, my.yd  = my.x - my.last.x, my.y - my.last.y
	my.sqrdspeed  = sqr(my.xd) + sqr(my.yd)

	-- get lists of unusual objects in sight
	acquirehomes()
	checkhomes()
	acquiredeaths()
	acquireshots()
	acquireplayers()

	-- get log entries and messages from the last loop
	acquirelog()
	checklog()
	if my.dead then
		if my.grave == maxgrave then
			decideaction()
		elseif my.grave == 0 then
			my.dead = false
		end
	end
	acquirechat()

	-- get terrain type dead ahead	
	pointblanc = at(
		math.ceil(my.x + my.xp * (ROBOT_R + 6)),
		math.ceil(my.y + my.yp * (ROBOT_R + 6))
		)

	time = os.clock()
	
	setblocks()

    laststate = state
	if state == STATE_EXPLORE then
		actionmask = explorer.main()
	elseif state == STATE_RETURN then
		actionmask = returner.main()
	elseif state == STATE_ATTACK then
		attacker.before()
		actionmask = attacker.main()
		attacker.after()
	end

	-- save current information
	my.last.x   = my.x
	my.last.y   = my.y
	lastmask = actionmask

	-- send information back to the game
	return actionmask

end
