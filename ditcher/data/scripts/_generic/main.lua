require("aux")

require("mask")

require("init")

require("acquire")

-- example of what is to be done in every loop
function main()

	-- reset action mask that will be returned to game
	actionmask = 0

	-- should be equivalent to chronos = chronos + 1
	chronos = gettime()

	-- acquire information about robot's location and status
	my.x , my.y, my.xp, my.yp, my.angle, my.speed,
		my.health, my.energy, _, my.grave,
		my.weapon, my.reload  = getmyinfo()

	-- checking for real direction and speed may recognize sliding
	my.xd, my.yd  = my.x - my.last.x, my.y - my.last.y
	my.sqrdspeed  = sqr(my.xd) + sqr(my.yd)

	-- get lists of objects in sight
	acquireplayers()
	acquireshots()
	acquirehomes()
	acquiredeaths()

	-- get log entries and messages from the last loop
	acquirelog()
	acquirechat()

	-- get terrain type dead ahead, it might be useful
	pointblanc = at(
		math.ceil(my.x + my.xp * (ROBOT_R + 3)),
		math.ceil(my.y + my.yp * (ROBOT_R + 3))
		)

	time = os.clock()

	-- THE INTELLIGENCE SHOULD BE EMBEDDED HERE
	
	-- save current information
	my.last.x   = my.x
	my.last.y   = my.y
	lastmask    = actionmask

	-- send information back to the game
	return actionmask

end
