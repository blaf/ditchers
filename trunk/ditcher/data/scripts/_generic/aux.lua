function sqr(base)
		return base * base
end

function sgn(base)
	if base > 0 then return 1
	elseif base < 0 then return -1
	else return 0 end
end

function abs(base)
	if base < 0 then 
		return -base
	else
		return base
	end
end

function distance(x1, y1, x2, y2)
		return math.sqrt(sqrdistance(x1, y1, x2, y2))
end

function sqrdistance(x1, y1, x2, y2)
		return sqr(x1 - x2) + sqr(y1 - y2)
end
