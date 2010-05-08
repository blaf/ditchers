function main()
    return think()
end

function think()
        x , y, xp, yp, angle, speed, health, energy  = getmyinfo()

        acquireshots()

        xd      = x - lastx
        yd      = y - lasty

        local dirdifx = xd / xp
        local dirdify = yd / yp

--      if dirdifx ~= 0 then
                dirdif = dirdify / dirdifx - 1
--      else
--              dirdif = 0
--      end

        keybmask = 0

        if state == STATE_ATTACK and (targetseen > 120 or energy < maxenergy / 3) then
                state = STATE_EXPLORE
        end

        if state == STATE_EXPLORE and (energy < maxenergy / 2 or health < maxhealth / 2) then
                state = STATE_RETURN
        elseif state == STATE_RETURN and (energy == maxenergy and health == maxhealth) then
                state = STATE_EXPLORE
                crumbs = 0
        end

        if (energy > maxenergy / 3 * 2 and health > maxhealth / 3) then
                for i = 0, playerscount - 1 do
                	_, histeam = getdesignation(i)
                        if i ~= myid and histeam ~= team and insight(i) then
                                state   = STATE_ATTACK
                                target  = i
                                targetseen      = 0
                                targetx, targety        = getinfo(i)
                                break
                        end
                end
        end

        local point     = at(x + xp * (ROBOT_R + 3), y + yp * (ROBOT_R + 3))

        if keepstate > 0 then
                keepstate = keepstate - 1
                keybmask = lastmask
        elseif state == STATE_ATTACK then
                if insight(target) then
                        targetseen      = 0
                        targetx, targety        = getinfo(target)
                else
                        targetseen      = targetseen + 1
                        if (sqrdistance(x, y, targetx, targety) < sqr(ROBOT_R * 2 - 1)) then
                                state = STATE_EXPLORE
                        end
                end
                goto(targetx, targety)
        elseif state == STATE_EXPLORE then
                if point == 2 or (lastmask % 2 == 1 and speed < 0.1) or math.abs(dirdif) > 0.01 then
                        if point == 2 or speed < 0.01 then
                                if (math.random(2) == 1) then
                                        keybmask = 5
                                else
                                        keybmask = 9
                                end
                                keepstate = 9
                        else
                                keybmask = lastmask
                        end
                elseif math.random(128) == 1 then
                        if (math.random(2) == 1) then
                                keybmask = 5
                        else
                                keybmask = 9
                        end
                        keepstate = math.random(16)
                elseif point == 1 then
                        keybmask = 17
                elseif point == 0 then
                        keybmask = 1
                else
                        keybmask = 4
                end

        elseif state == STATE_RETURN then
                followcrumbs()
        end

        if state == STATE_EXPLORE or state == STATE_ATTACK or (state == STATE_RETURN and energy > maxenergy / 4) then
                if point == 0 and not bwand(keybmask, 16) then
                        for i = 0, playerscount - 1 do
                        	_, histeam = getdesignation(i)
                                if i ~= myid and histeam ~= team and inscope(i) then
                                        keybmask = keybmask + 16
                                        break
                                end
                        end
                end
        end

        if state == STATE_EXPLORE or state == STATE_ATTACK then
                if bwand(keybmask, 4) or bwand(keybmask, 8) then
                        if nocrumb > 4 and sqrdistance(x, y, crumbx[crumbs], crumby[crumbs]) > sqr(24) then
                                crumbs = crumbs + 1
                                crumbx[crumbs] = x
                                crumby[crumbs] = y
                                nocrumb = 0
                        end
                end
                nocrumb = nocrumb + 1
        end


        lastx   = x
        lasty   = y
        lastmask = keybmask
        return keybmask
end

function sqr(base)
        return base * base
end

function acquireshots()
        shots   = getshots()
        shotscount  = (table.getn(shots) / shotsattr)

        for i = 0,shotscount-1 do
                shots_enemy[i] = shots[i*shotsattr + 1]
                shots_x[i]     = shots[i*shotsattr + 2]
                shots_y[i]     = shots[i*shotsattr + 3]
                shots_speed[i] = shots[i*shotsattr + 4]
                shots_angle[i] = shots[i*shotsattr + 5]
                shots_xp[i]    = shots[i*shotsattr + 6]
                shots_yp[i]    = shots[i*shotsattr + 7]
--              print(shots_enemy[i], shots_x[i], shots_y[i], shots_speed[i], shots_angle[i], shots_xp[i], shots_yp[i])
        end
end

function insight(i)
        local x2
        local y2
        x2, y2  = getinfo(i)
        if x2 and y2 then
                local dist      = distance(x, y, x2, y2)
                if dist < 250 then
                        local xa   = (x2 - x) / dist
                        local ya   = (y2 - y) / dist
                        local muds = 0
                        local point
                        for i = ROBOT_R, dist do
                                point = at(x + xa * i, y + ya * i)
                                if point ~= 0 then
                                        if point == 1 then
                                                muds = muds + 1
                                                if muds > 24 then
                                                        return false
                                                end
                                        elseif point == 2 then
                                                return false
                                        end
                                end
                        end
                        return true
                else
                        return false
                end
        else
         return false
        end
end

function inscope(i)
        local x2
        local y2
        x2, y2  = getinfo(i)
        if x2  and y2 then
                local dist      = distance(x, y, x2, y2)
                local targx     = x + xp * dist
                local targy     = y + yp * dist
                local tarsqrdist        = sqrdistance(x2, y2, targx, targy)

                if tarsqrdist <= sqr(ROBOT_R * 1.2) then
                        local muds = 0
                        local point
                        for i = ROBOT_R, dist do
                                point = at(x + xp * i, y + yp * i)
                                if point ~= 0 then
                                        if point == 1 then
                                                muds = muds + 1
                                                if muds > 24 then
                                                        return false
                                                end
                                        elseif point == 2 then
                                                return false
                                        end
                                end
                        end
                        return true
                else
                        return false
                end
        else
                return false
        end
end

function bwand(mask, key)
        return math.floor((mask % (key * 2)) / key) > 0
end

function followcrumbs()

        if crumbs >= 0 then
                local sqrdist = sqrdistance(crumbx[crumbs], crumby[crumbs], x, y)

                while crumbs > 0 and sqrdist < sqr(12) do
                        crumbs = crumbs - 1
                        sqrdist = sqrdistance(crumbx[crumbs], crumby[crumbs], x, y)
                end

                if crumbs > 0 or sqrdist > sqr(20) then
                        goto(crumbx[crumbs], crumby[crumbs])
                end

        end
end

function goto(tox, toy)

        local dist              = distance(tox, toy, x, y)

        local vectorx   = (tox - x) / dist
        local vectory   = (toy - y) / dist

        -- negative: rotate right, positive: rotate left
        local rot       = vectorx * yp - vectory * xp

        -- negative: wrong way, positive: correct way
        local scal      = vectorx * xp + vectory * yp

        if lastmask % 2 == 1 and speed < 0.1 then
                if math.random(2) == 1 then
                        keybmask = keybmask + 9
                else
                        keybmask = keybmask + 5
                end
                keepstate = 9
        else
                if scal < 0.99 then
                        if rot < 0 then
                                keybmask = keybmask + 8
                        else
                                keybmask = keybmask + 4
                        end
                end
                if scal > 0.5 then
                        keybmask = keybmask + 1
                end
        end

end

function distance(x1, y1, x2, y2)
        return math.sqrt(sqrdistance(x1, y1, x2, y2))
end

function sqrdistance(x1, y1, x2, y2)
        return sqr(x1 - x2) + sqr(y1 - y2)
end

ROBOT_R         = 13

STATE_EXPLORE   = 1
STATE_RETURN    = 2
STATE_ATTACK    = 3

target          = 0
targetx         = 0
targety         = 0
targetseen      = 0

key_up          = 0
key_down        = 0
key_left        = 0
key_right       = 0
key_fire        = 0
key_weapon      = 0

playerscount = getplayerscount()
mapwidth, mapheight, maptorus = getmapinfo()
sight   = getsight()

maxhealth, maxenergy    = getmaxstatus()

myid, _, team    = getmydesignation()
homex, homey, xp, yp, angle, speed   = getmyinfo()

state   = STATE_EXPLORE

keybmask = 0
lastmask = 0

nocrumb = 0
crumbs  = 0

crumbx  = {}
crumby  = {}

crumbx[0]       = homex
crumby[0]       = homey

shotsattr       = 7
shots_enemy     = {}
shots_x         = {}
shots_y         = {}
shots_speed     = {}
shots_angle     = {}
shots_xp        = {}
shots_yp        = {}

keepstate       = 0

lastx = homex
lasty = homey

--print (sight, playerscount, myid, mapwidth, mapheight, maptorus, homex, homey)
