require("heap")

require("blocks")

function setrange(range, a, b, val)
	if a < 0 or a >= blocks.w or b < 0 or b >= blocks.h then
		if map.torus then
			setrange(range,
				modulo(a, blocks.w), modulo(b, blocks.h), val)
		else end
	end
	range[a*blocks.h+b] = val
end
function getrange(range, a, b)
	if a < 0 or a >= blocks.w or b < 0 or b >= blocks.h then
		if map.torus then
			return getrange(range, 
				modulo(a, blocks.w), modulo(b, blocks.h))
		else return nil end
	end
	return range[a*blocks.h+b]
end

	-- hsl = hic sunt leones
function findblockpath(pt1, pt2, movecost, hslcost)
	
	local range = {}
	
	local queue = {}
	heap.init(queue, "d")
	
	local filled = {}
	local q
	
	q = { s = 1, d = 0, x = pt1.x, y = pt1.y }
	setrange(range, q.x, q.y, q)
	filled[q.s] = 1
	heap.push(queue, q)
	local c1 = q
   
	q = { s = 2, d = 0, x = pt2.x, y = pt2.y }
	setrange(range, q.x, q.y, q)
	filled[q.s] = 1
	heap.push(queue, q)
	local c2 = q

	local path = {}
	path.hslcost  = hslcost
	path.movecost = movecost
	path.target =
	   {x = (pt2.x+0.5) * blocks.s, y = (pt2.y+0.5) * blocks.s}
	
	local qn, i, j, cf, ter, cost
	local sqrt2 = math.sqrt(2)
	
	local counter = 0
		
	local yielded = 0
	
	while queue.size > 0 and counter < 2*blocks.h*blocks.w do
	
		counter = counter + 1

		if coroute and os.clock() - time >= 0.02 then 
			yielded = yielded + 1
			coroutine.yield(0)
		end

		q = heap.pop(queue)
		filled[q.s] = filled[q.s] - 1
		for it = 0, 2 do
			if it == 2 then i = q.x-1 else i = q.x+it end
			for jt = 0, 2 do
				if jt == 2 then j = q.y-1 else j = q.y+jt end
				ter = getblock(i, j)
				if not getrange(range, i, j) and (ter == 2 or
					(ter == -1 and hslcost ~= nil)) then
					if i == q.x or j == q.y then 
						cf = 1 else cf = sqrt2 end
					qn	= {}
					qn.x  = modulo(i, blocks.w)
					qn.y  = modulo(j, blocks.h)
					qn.s  = q.s
					qn.p  = q
					if ter == 2 then
						cost = cf*(getblockcover(i,j) + movecost)
					else
						cost = cf*(hslcost + movecost)
					end
					qn.d  = q.d + cost
					setrange(range, i, j, qn)
					filled[qn.s] = filled[qn.s] + 1
					heap.push(queue, qn)
					if qn.s == 1 and 
						sqrdistance(c1.x, c1.y, c2.x, c2.y) >
						sqrdistance(qn.x, qn.y, c2.x, c2.y) then
						c1 = qn
					elseif qn.s == 2 and
						sqrdistance(c1.x, c1.y, c2.x, c2.y) >
						sqrdistance(c1.x, c1.y, qn.x, qn.y) then
						c2 = qn
					end						
				elseif getrange(range, i, j) and
					getrange(range, i, j)["s"] ~= q.s then
					local dmax = getrange(range, i, j)["d"]
					local q1
					local q2
					if q.s == 1 then
						q1 = q
						q2 = getrange(range, i, j)
					else
						q1 = getrange(range, i, j)
						q2 = q
					end
					local pn
					local ii   = 1
					while q1.p do
						pn   = {}
						pn.x = q1.x * blocks.s + blocks.s / 2
						pn.y = q1.y * blocks.s + blocks.s / 2
						if not inline(path[ii+1], path[ii], pn) then
							ii   = ii - 1
							--print(ii, pn.x, pn.y)
						end
						path[ii] = pn
						q1 = q1.p
					end
					path.min = ii
					ii = 0
					while q2.p do
						pn   = {}
						pn.x = q2.x * blocks.s + blocks.s / 2
						pn.y = q2.y * blocks.s + blocks.s / 2
						if not inline(path[ii-1], path[ii], pn) then
							ii = ii + 1
							--print(ii, pn.x, pn.y)
						end
						path[ii] = pn
						q2  = q2.p
					end
					path.max = ii
					path.length = path.max - path.min + 1
					if path.length <= 0 then
						return nil
					end
					return path
				end
			end
		end
		if filled[q.s] == 0 then break end
	end
	return nil
end

function inline(pt1, pt2, pt3)
	if not pt1 or not pt2 or not pt3 then
		return false
	end
	return (pt1.x - pt2.x)*(pt1.y - pt3.y) == (pt1.x - pt3.x)*(pt1.y - pt2.y)
end

function findpath(from, to, movecost, hslcost)
	return findblockpath(
	  { x = math.floor(from.x / blocks.s),
		y = math.floor(from.y / blocks.s) },
	  { x = math.floor(to.x / blocks.s),
		y = math.floor(to.y / blocks.s) },
		movecost, hslcost)
end

function followpath(path)
	if path then
		while sqrdistance(my.x, my.y,
			path[path.min].x, path[path.min].y) < sqr(ROBOT_R - 1) do
			path.min = path.min + 1
			if path.min > path.max then
				path = nil
				break
			end
		end
	end
	if path then
		gostraight(path[path.min])
	end
	return path
end

function struggle()
	if math.random(3) == 1 then
		setkey(LEFT)
	elseif math.random(3) == 2 then
		setkey(RIGHT)
	end
	setkey(UP)
end

-- where must implement { .x , .y }

function gostraight(whereraw)

	local where = { x = whereraw.x, y = whereraw.y }
	
	if where.x > my.x + map.w / 2 then
		where.x = where.x - map.w
	elseif where.x < my.x - map.w / 2 then
		where.x = where.x + map.w
	end
	if where.y > my.y + map.h / 2 then
		where.y = where.y - map.h
	elseif where.y < my.y - map.h / 2 then
		where.y = where.y + map.h
	end
	
	local dist   = distance(where.x, where.y, my.x, my.y)

	local vector = {}
	vector.x	 = (where.x - my.x) / dist
	vector.y	 = (where.y - my.y) / dist

	-- negative: rotate right, positive: rotate left
	local rot	   = vector.x * my.yp - vector.y * my.xp

	-- negative: wrong way, positive: correct way
	local scal	  = vector.x * my.xp + vector.y * my.yp

	if scal < 0.99 then
		if rot < 0 then
			setkey(RIGHT)
		else
			setkey(LEFT)
		end
	end

	if scal >= 0.3 then
		setkey(UP)
	end
end
