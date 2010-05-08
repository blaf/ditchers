function explorer.core()
	if attacker.check() then return end

	if my.dead then return end
	
	if explorer.switched then
		explorer.switched = false
		explorer.tostate()
	end
	if explorer.mainpath then
		explorer.explore()
	elseif getblock(my.block.x, my.block.y) == 0 then
		explorer.struggle(my.home)
	else
		explorer.mainpath = explorer.getexplorepath()
   	end
	
	if pointblanc == 1 then
		setkey(FIRE)
		explorer.energyloss = explorer.energyloss
			+ shtypes[my.weapon].energy / 6
	end
	
	if not returner.wellbeing(1 / 2, 2 / 3) and
		sqrdistance(my.x, my.y, my.home.x, my.home.y) > sqr(50) then
		state = STATE_RETURN
		returner.switched = true
	elseif not returner.wellbeing(1, 1) and
		sqrdistance(my.x, my.y, my.home.x, my.home.y) < sqr(40) then
		unsetkey(UP)
	end
end

function explorer.explore()
	explorer.mainpath = followpath(explorer.mainpath)
	if my.speed < 0.01 and waskey(UP) then
		explorer.mainpath = findpath(my, explorer.mainpath.target,
			explorer.mainpath.movecost, explorer.mainpath.hslcost)
	end
	if not explorer.mainpath then
		explorer.mainpath = explorer.getexplorepath()
	end
end

function explorer.struggle(where)
	if chronos % 25 < 10 then
		gostraight(where)
	else
		struggle()
	end
end

function explorer.getsomeblock(tertype)
	local counter, a, b, ter, pt
	counter = 0
	repeat
		counter = counter + 1
		a = math.random(blocks.v, blocks.w - blocks.v / 2 - 1)
		b = math.random(blocks.v, blocks.h - blocks.v / 2 - 1)
		ter = getblock(a, b)
	until ter == tertype or counter > 100
	if ter == tertype then return {x = a, y = b}
	else return {x = -1, y = -1} end
end

function explorer.getexplorepath()
	local pt = explorer.getsomeblock(-1)
	if pt.x >= 0 then
		return findblockpath(my.block, pt, 0.2, 1)
	else
		pt = explorer.getsomeblock(1)
		if pt.x >= 0 then
			return findblockpath(my.block, pt, 0.2, 1)
		else return nil end
	end
end

function explorer.tostate()
	state = STATE_EXPLORE
	explorer.contpath = nil
	explorer.energyloss = 0
	if explorer.mainpath then
		explorer.mainpath =
			findpath(my, explorer.mainpath.target, 0.7, 1)
	end
end

explorer.main = coroutine.wrap(
	function()
		while true do
			coroute = true
			explorer.core()
			coroute = false
			coroutine.yield(actionmask)
		end
	end
)
