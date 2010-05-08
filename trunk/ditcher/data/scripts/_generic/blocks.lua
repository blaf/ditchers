function modulo(coord, size)
	if coord < 0 then coord = coord + size
	elseif coord >= size then coord = coord - size
	end
	return coord
end

function setblocks()
	my.block.x = math.floor(my.x / blocks.s)
	my.block.y = math.floor(my.y / blocks.s)
	
	for i = 0, playerscount-1 do
		if getvisible(i) then
			local loc = {}
			loc.x, loc.y = getinfo(i)
			setblocksat(loc, ROBOT_R)		
		end
	end
	for i = 0, shotscount-1 do
		setblocksat(shots[i], 5)
	end
	
	if my.block.x == my.block.last.x
		and my.block.y == my.block.last.y then
		return
	end
	
	local i1 = math.floor((my.x-sight) / blocks.s)+blocks.l
	local i2 = math.floor((my.x+sight) / blocks.s)-blocks.l
	local j1 = math.floor((my.y-sight) / blocks.s)+blocks.l
	local j2 = math.floor((my.y+sight) / blocks.s)-blocks.l
	
	if my.block.x ~= my.block.last.x then
		for j = j1, j2 do
			if my.block.x < my.block.last.x then
				acquireblock(i1, j)
			else
				acquireblock(i2, j)
			end
		end
	end
	if my.block.y ~= my.block.last.y then
		for i = i1, i2 do
			if my.block.y < my.block.last.y then
				acquireblock(i, j1)
			else
				acquireblock(i, j2)
			end
		end
	end			
	
	my.block.last.x = my.block.x
	my.block.last.y = my.block.y
end

function setblocksat(where, radius)
	local b  = {}
	b.x  = math.floor(where.x / blocks.s)
	b.y  = math.floor(where.y / blocks.s)
	b.r  = math.ceil(radius / blocks.s) * 2 - 1

	for j = b.y - b.r, b.y + b.r do
		for i = b.x - b.r, b.x + b.r do
			updateblock(i, j)
		end
	end
end

function setinitblocks()
	for j = math.floor((my.y-sight) / blocks.s),
		math.floor((my.y+sight) / blocks.s) do
		for i = math.floor((my.x-sight) / blocks.s),
			math.floor((my.x+sight) / blocks.s) do
			acquireblock(i, j)
		end
	end	
end

function printsightblocks(mode)
	local xmod, ymod
	if mode == 1 then
		xmod = 1
		ymod = 0.5
	elseif mode == 2 then
		xmod = 2
		ymod = 1
	else
		xmod = 1
		ymod = 1
	end
	
	local i1 = math.floor((my.x-sight*xmod) / blocks.s)+blocks.l
	local i2 = math.floor((my.x+sight*xmod) / blocks.s)-blocks.l
	local j1 = math.floor((my.y-sight*ymod) / blocks.s)+blocks.l
	local j2 = math.floor((my.y+sight*ymod) / blocks.s)-blocks.l
	
	print(i1, j1, i2, j2)	
 
	for j = j1, j2 do
		for i = i1, i2 do
			if i == my.block.x and j == my.block.y then
				io.write("O")
			elseif j == my.block.y 
				and (i == my.block.x - 1 or i == my.block.x + 1) then
				io.write("|")
			elseif i == my.block.x 
				and (j == my.block.y - 1 or j == my.block.y + 1) then
				io.write("-")
			elseif ((i == my.block.x - 1) and (j == my.block.y - 1)) or
				((i == my.block.x + 1) and (j == my.block.y + 1)) then
				io.write("/")
			elseif ((i == my.block.x + 1) and (j == my.block.y - 1)) or
				((i == my.block.x - 1) and (j == my.block.y + 1)) then
				io.write("\\")
			else
				printblock(i, j)
			end
		end   
		print()
	end
end

function getblock(a, b)
	if a < 0 or a >= blocks.w or b < 0 or b >= blocks.h then
		if map.torus then 
			return getblock(modulo(a, blocks.w), modulo(b, blocks.h))
		else return -2 end
	end
	local val = blocks[a*blocks.h+b]
	if val == nil then
		val = -1
	end
	return val
end

function setblock(a, b, val)
	if a < 0 or a >= blocks.w or b < 0 or b >= blocks.h then
		if map.torus then
			setblock(modulo(a, blocks.w), modulo(b, blocks.h), val)
		else return end
	end
	if val == -1 then
		val = nil
	end
	blocks[a*blocks.h+b] = val
end

function getblockcover(a, b)
	if a < 0 or a >= blocks.w or b < 0 or b >= blocks.h then
		if map.torus then return 
			getblockcover(modulo(a, blocks.w), modulo(b, blocks.h))
		else return 1 end
	end
	return blocks.r[a*blocks.h+b]
end

function setblockcover(a, b, val)
	if a < 0 or a >= blocks.w or b < 0 or b >= blocks.h then
		if map.torus then
			setblockcover(modulo(a, blocks.w), modulo(b, blocks.h), val)
		else return end
	end
	blocks.r[a*blocks.h+b] = val
end

function updateblock(a, b)
	local ter = getblock(a, b)
	if ter > 0 then
		local rat
		if ter == 1 then
			rat = covr(a*blocks.s, b*blocks.s,
				(a+1)*blocks.s-1, (b+1)*blocks.s-1)
		elseif ter == 2 then
			rat = covr((a+1-blocks.l)*blocks.s,
				(b+1-blocks.l)*blocks.s,
				(a+blocks.l)*blocks.s-1,
				(b+blocks.l)*blocks.s-1)
		end 
		if not rat then rat = 1 end
		setblockcover(a, b, rat)
	end
end

function acquireblock(a, b)
	if a >= blocks.w or b >= blocks.h or a < 0 or b < 0 then
		if map.torus then
			acquireblock(modulo(a, blocks.w), modulo(b, blocks.h))
		else setblock(a, b, -2) end
		return
	end
	if getblock(a, b) == -1 then
		local code = hom((a+1-blocks.l)*blocks.s,
			(b+1-blocks.l)*blocks.s,
			(a+blocks.l)*blocks.s-1,
			(b+blocks.l)*blocks.s-1, 3)
		if code == 0 then
			local code = hom(a*blocks.s, b*blocks.s,
				(a+1)*blocks.s-1, (b+1)*blocks.s-1, 3)
			setblock(a, b, code)
		elseif code == 1 then
			setblock(a, b, 2)
		end
	end
	updateblock(a, b)
	return getblock(a, b)
end

function printblock(a, b)
	local ter = getblock(a, b)
	if ter == -2 then
		io.write("@")
	elseif ter == -1 then
		io.write("?")
	elseif ter == 0 then
		io.write("#")
	elseif ter == 1 then
		io.write("!")
	elseif ter == 2 then
		local rat = math.ceil(getblockcover(a, b)*9)
		if (rat <= 1) then io.write(" ")
		elseif (rat <= 4) then  io.write(".")
		elseif (rat <= 7) then  io.write(":")
		else io.write("+") end		
	end
end

blocks   = {}
blocks.r = {}
blocks.l = 3
blocks.s = math.ceil((ROBOT_R * 2 + 1) / (blocks.l * 2 - 1))
blocks.h = math.ceil(map.h / blocks.s)
blocks.w = math.ceil(map.w / blocks.s)
blocks.v = math.floor(sight / blocks.s)

my.block = {}
my.block.last = {}
my.block.x = math.floor(my.x / blocks.s)
my.block.y = math.floor(my.y / blocks.s)
my.block.last.x = -1
my.block.last.y = -1

setinitblocks()
