-- robot and death hole radius (constant)
ROBOT_R, DEATH_R = getradius()

-- number of rotational positions (constant)
ROTCOUNT = getrotcount()

-- minimal difference between two angles (constant)
ROTDIFF  = 360 / ROTCOUNT

maxgrave = getmaxgrave()

-- number of weapon types
weaponscount = getweaponscount()

-- number of players
playerscount = getplayerscount()

-- maximal values of health and energy
maxhealth, maxenergy = getmaxstatus()

-- map size and topology
map = {}
map.w, map.h, map.torus = getmapinfo()

-- view radius
sight   = getsight()

my  = {}

-- scripted robot's basic info
my.id, my.name, my.team = getmydesignation()

-- other robot's information initialization
my.x , my.y, my.xp, my.yp, my.angle, my.speed,
	my.health, my.energy, _, _, my.weapon, my.reload  = getmyinfo()

-- robot's initial position is the home position
my.home = {}
my.home.x	= my.x
my.home.y	= my.y

-- initialization last loop's coordinates
my.last = {}
my.last.x   = my.home.x
my.last.y   = my.home.y

-- initialization of other global variables
chronos	 = 0
actionmask  = 0
lastmask	= 0

STATE_NONE	= 0
STATE_EXPLORE = 1
STATE_RETURN  = 2
STATE_ATTACK  = 3

state = STATE_EXPLORE
