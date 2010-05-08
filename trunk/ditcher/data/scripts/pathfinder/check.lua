base  = {}
basec = {}

base[my.id]   = my.home
basec[my.home.x*map.h+my.home.y] = my.id

function checkhomes()
	for i = 0, homescount - 1 do
		if basec[homes[i].x*map.h+homes[i].y] == nil then
			basec[homes[i].x*map.h+homes[i].y] = -1
		end
	end
end

function checklog()
	for i = 0, logcount - 1 do
		if log[i].killed == my.id then
			my.dead = true
		elseif attacker.target
			and log[i].killed == attacker.target.id then
			attacker.target.dead = true
		end
	end
end

