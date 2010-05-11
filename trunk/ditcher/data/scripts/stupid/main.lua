function main()
	-- this would be even simpler
    -- return math.random(0, 63)

	w = 0    
    k = math.random(0, 31)
    	
   		if k == 0 then u = 1-u
    elseif k == 1 then d = 1-d
    elseif k == 2 then l = 1-l
    elseif k == 3 then r = 1-r
    elseif k == 4 then f = 1-f
    elseif k == 5 then w = 1 end
    
    return u*1+d*2+l*4+r*8+f*16+w*32
end

u = 0
d = 0
l = 0
r = 0
f = 0
w = 0
k = 0
