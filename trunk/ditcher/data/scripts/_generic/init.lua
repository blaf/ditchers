-- robot and death hole radius (constant)
ROBOT_R, DEATH_R = getradius()

-- number of rotational positions (constant)
ROTCOUNT = getrotcount()

-- minimal difference between two angles (constant)
ROTDIFF  = 360 / ROTCOUNT

maxgrave = getmaxgrave()

-- maximal values of health and energy
maxhealth, maxenergy = getmaxstatus()

-- number of weapon types
weaponscount = getweaponscount()

-- acquiring information about shot types
shtypes		= {}
local i
for i = 0, weaponscount - 1 do
	shtypes[i] = {}
	shtypes[i].energy, shtypes[i].reload, shtypes[i].speed,
		shtypes[i].ditch, shtypes[i].splash, shtypes[i].damage,
		shtypes[i].endamage, shtypes[i].bounces, shtypes[i].livetime
		= getshottype(i)
end

-- view radius
sight   = getsight()

-- number of players
playerscount = getplayerscount()

-- map size and topology
map = {}
map.w, map.h, map.torus = getmapinfo()

my  = {}

-- scripted robot's immutable info
my.id, my.name, my.team = getmydesignation()

-- scripted robot's information initialization
my.x , my.y, my.xp, my.yp, my.angle, my.speed, my.health, my.energy,
	my.protection, my.grave, my.weapon, my.reload  = getmyinfo()

-- scripted robot's initial position is the home position
my.home = {}
my.home.x	= my.x
my.home.y	= my.y

-- initialization of last loop's coordinates
my.last = {}
my.last.x   = my.home.x
my.last.y   = my.home.y

-- initialization of other global variables
chronos	    = 0
actionmask  = 0
lastmask	= 0
