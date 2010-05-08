-- action masks
UP	  	= 1
DOWN	= 2
LEFT	= 4
RIGHT   = 8
FIRE	= 16
WEAPON  = 32

-- auxiliary functions to comfortably manage action mask

function iskey(key)
	return iskeymask(actionmask, key)
end

function waskey(key)
	return iskeymask(lastmask, key)
end

function iskeymask(mask, key)
	return mask % (key * 2) >= key
end

function setkey(key)
	if not iskeymask(actionmask, key) then
		actionmask = actionmask + key
	end
end

function unsetkey(key)
	if iskeymask(actionmask, key) then
		actionmask = actionmask - key
	end
end
