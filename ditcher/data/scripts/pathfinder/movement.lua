require("path")

explorer = {}
require("explorer")

returner = {}
require("returner")

attacker = {}
require("attacker")

function decideaction()
	state = STATE_NONE
	if returner.wellbeing(3 / 4, 4 / 5) and attacker.check() then
	elseif sqrdistance(my.x, my.y, my.home.x, my.home.y) < sqr(50)
		or returner.wellbeing(1 / 2, 2 / 3) then
		state = STATE_EXPLORE
		explorer.switched = true
	else
		state = STATE_RETURN
		returner.switched = true
	end
end

time = os.clock()

explorer.tostate()
explorer.core()
