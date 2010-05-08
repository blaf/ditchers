heap = {}

	-- elements must implement [h.p]
function heap.init(h, pratr)
	h.size = 0
	h.p	= pratr
end

function heap.push(h, key)
	h.size = h.size + 1
	h[h.size] = key
	heap.up(h, h.size)
end

function heap.up(h, index)
	local parent = math.floor(index / 2)
	if parent >= 1 and h[parent][h.p] > h[index][h.p] then
		h[parent], h[index] = h[index], h[parent]
		heap.up(h, parent)
	end
end

function heap.down(h, index)
	local son1 = index * 2
	local son2 = index * 2 + 1
	local son
	
	if son1 > h.size then
		return
	elseif son2 > h.size then
		son = son1
	else
		if h[son1][h.p] < h[son2][h.p] then
			son = son1 else son = son2
		end
	end
	
	if h[index][h.p] > h[son][h.p] then
		h[index], h[son] = h[son], h[index]
		heap.down(h, son)
	end
end

function heap.pop(h)	
	local key = h[1]
	h[1]	  = h[h.size]
	h[h.size] = nil
	h.size	= h.size - 1
	if h.size > 0 then 
		heap.down(h, 1)
	else
		h.size = 0
	end
	return key
end
