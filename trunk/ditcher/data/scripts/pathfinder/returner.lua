function returner.wellbeing(factore, factorh)
	if (my.energy >= (maxenergy - explorer.energyloss) * factore
		and my.health >= maxhealth * factorh) then
		return true else return false end
end

function returner.core()
	if my.dead then return end
	
	if returner.wellbeing(3 / 4, 4 / 5)
		and attacker.check() then return end
	
	if returner.switched then
		returner.switched = false
		returner.tostate()
	end

	if returner.pathlost > 1 then
		returner.pathlost = returner.pathlost - 1
	elseif returner.pathlost <= 1 and not returner.path then
		returner.path = findpath(my, returner.target, 0.2, nil)
		if returner.path then
			returner.pathlost = 0
		else
			if returner.pathlost == 1 then
				returner.pathlost = 150
			else
				returner.pathlost = 50
			end
		end
	end
	
	if returner.path then
		returner.path = followpath(returner.path) 
	elseif returner.pathlost > 50 then
		struggle(returner.target)
	else gostraight(returner.target) end
	
	if not returner.path and returner.pathlost == 0 then
		state = STATE_EXPLORE
		explorer.switched = true
	end
end

function returner.tostate()
	state = STATE_RETURN
	explorer.point	  = { x = my.x, y = my.y }
	returner.pathlost = 0
	returner.path	  = nil
	returner.target   = my.home
end

returner.main = coroutine.wrap(
	function()
		while true do
			coroute = true
			returner.core()
			coroute = false
			coroutine.yield(actionmask)
		end
	end
)

