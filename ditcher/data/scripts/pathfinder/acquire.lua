-- fills shots[] records with data about shots in sight
function acquireshots()
	local i

	-- gets raw data (table filled with information about shots)
	local shotsdata = getshots()

	-- each shot takes SHOTS_ATTR rows of table, this gives number of shots in sight
	shotscount  = (table.getn(shotsdata) / SHOTS_ATTR)

	-- acquires data from table to arrays
	for i = 0, shotscount - 1 do
		shots[i] = {}
		shots[i].enemy = shotsdata[i*SHOTS_ATTR + 1]
		shots[i].x	 = shotsdata[i*SHOTS_ATTR + 2]
		shots[i].y	 = shotsdata[i*SHOTS_ATTR + 3]
		shots[i].speed = shotsdata[i*SHOTS_ATTR + 4]
		shots[i].angle = shotsdata[i*SHOTS_ATTR + 5]
		shots[i].xp	= shotsdata[i*SHOTS_ATTR + 6]
		shots[i].yp	= shotsdata[i*SHOTS_ATTR + 7]
		shots[i].type  = shotsdata[i*SHOTS_ATTR + 8]
--		print("shot	  #", i,":", shots[i].enemy, shots[i].x, shots[i].y, shots[i].speed, shots[i].angle, shots[i].xp, shots[i].yp, shots[i].type)
--		print("shot type #", shots[i].type,":", getshottype(shots[i].type))
	end
end

-- fills homes[] records with data about homes in sight
function acquirehomes()
	local i

	-- gets raw data (table filled with information about homes)
	local homesdata = gethomes()
	-- each home takes HOMES_ATTR rows of table, this gives number of homes in sight
	homescount  = (table.getn(homesdata) / HOMES_ATTR)

	-- acquires data from table to arrays
	for i = 0, homescount - 1 do
		homes[i]   = {}
		homes[i].x = homesdata[i*HOMES_ATTR + 1]
		homes[i].y = homesdata[i*HOMES_ATTR + 2]
--		print("home	  #", i,":", homes[i].x, homes[i].y)
	end
end

-- fills deaths[] records with data about death stains in sight
function acquiredeaths()
	local i

	-- gets raw data (table filled with information about death stains)
	local deathsdata = getdeaths()
	-- each stain takes DEATHS_ATTR rows of table, this gives number of death stains in sight
	deathscount  = (table.getn(deathsdata) / DEATHS_ATTR)

	-- acquires data from table to arrays
	for i = 0, deathscount - 1 do
		deaths[i]	   = {}
		deaths[i].x	 = deathsdata[i*DEATHS_ATTR + 1]
		deaths[i].y	 = deathsdata[i*DEATHS_ATTR + 2]
		deaths[i].grave = deathsdata[i*DEATHS_ATTR + 3]
--		print("death	 #", i,":", deaths[i].x, deaths[i].y, deaths[i].grave)
	end
end

-- fills chat[] records with data about new messages
function acquirechat()
	local i

	-- gets raw data (table filled with information about messages)
	local chatdata = getchat(chronos - 1)
	-- each chat message takes CHAT_ATTR rows of table, this gives number of new messages
	chatcount  = (table.getn(chatdata) / CHAT_ATTR)

	-- acquires data from table to arrays
	for i = 0, chatcount - 1 do
		chat[i] = {}
		chat[i].text   = chatdata[i*CHAT_ATTR + 1]
		chat[i].time   = chatdata[i*CHAT_ATTR + 2]
		chat[i].player = chatdata[i*CHAT_ATTR + 3]
--		print("message   #", i,":", chat[i].text, chat[i].time, chat[i].player)
	end
end

-- fills log[] records with data about new log data
function acquirelog()
	local i

	-- gets raw data (table filled with information about log data)
	local logdata = getlog(chronos - 1)
	-- each log entry takes LOG_ATTR rows of table, this gives number of log entries
	logcount  = (table.getn(logdata) / LOG_ATTR)

	-- acquires data from table to arrays
	for i = 0, logcount - 1 do
		log[i] = {}
		log[i].time   = logdata[i*LOG_ATTR + 1]
		log[i].killer = logdata[i*LOG_ATTR + 2]
		log[i].killed = logdata[i*LOG_ATTR + 3]
		log[i].weapon = logdata[i*LOG_ATTR + 4]
--		print("log entry #", i,":", log[i].time, log[i].killer, log[i].killed, log[i].weapon)
	end
end

function acquireplayers()
	local targ, targid
	if attacker.target then
		targ = attacker.target
		targid = attacker.target.id
	end
	local a, b
	for i = 0, playerscount - 1 do
		if i ~= my.id and getvisible(i) then
			players[i] = {}
			players[i].name, players[i].team  = getdesignation(i)
			players[i].x,  players[i].y,
				players[i].xp, players[i].yp,
				players[i].angle, players[i].speed = getinfo(i);
			if targ and targid == i then
				targ.loc = { x = players[i].x, y = players[i].y }
			end
		else
			players[i] = nil
		end
	end
end

-- initialization of chat, log, homes, shots and shot types arrays
CHAT_ATTR   = 3
chat		= {}

LOG_ATTR	= 4
log		 = {}


HOMES_ATTR  = 2
homes	   = {}

DEATHS_ATTR = 3
deaths	  = {}


SHOTS_ATTR  = 8
shots	   = {}

shtypes	 = {}

players	 = {}

-- acquiring information about shot types
--	 (doesn't change during game)
local i
for i = 0, weaponscount - 1 do
	shtypes[i] = {}
	shtypes[i].energy, shtypes[i].reload, shtypes[i].speed,
		shtypes[i].ditch, shtypes[i].splash, shtypes[i].damage,
		shtypes[i].endamage, shtypes[i].bounces, shtypes[i].livetime
		= getshottype(i)
end

