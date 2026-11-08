-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- NPC Control v96 by Necrym59 and Lee
-- DESCRIPTION: The attached NPC will be controlled by this behavior.
-- DESCRIPTION: [SENSE_TEXT$="Who's that ..an intruder??"]
-- DESCRIPTION: [SENSE_RANGE=500(0,2000)]
-- DESCRIPTION: [@SENSE_MODE=1(1=Threaten/Attack, 2=Instant Attack, 3=Sighted/End Game, 4=Sighted/Trigger IfUsed, 5=Sighted/Trigger IfUsed + Attack, 6=Sighted/Reload Level)]
-- DESCRIPTION: [@NPC_CAN_FLEE=2(1=Yes,2=No)]
-- DESCRIPTION: [#IDLE_TIME=3000(0,20000)]
-- DESCRIPTION: [#ATTACK_RANGE=100(0,1000)]
-- DESCRIPTION: [ATTACK_DAMAGE=10(0,100)]
-- DESCRIPTION: [@RANDOM_DAMAGE=2(1=Yes,2=No)]
-- DESCRIPTION: [@NPC_CAN_ROAM=1(1=Yes,2=No)]
-- DESCRIPTION: [ROAM_RANGE=500(0,3000)]
-- DESCRIPTION: [#NPC_ANIM_SPEED=0.8(0.1,5.0)]
-- DESCRIPTION: [#NPC_MOVE_SPEED=100(0,1000)]
-- DESCRIPTION: [#NPC_RUN_SPEED=110(0,1000)]
-- DESCRIPTION: [#NPC_TURN_SPEED=100(0,1000)]
-- DESCRIPTION: [@NPC_CAN_SHOOT=2(1=Yes,2=No,3=Yes-Weapon Hidden))]
-- DESCRIPTION: [@IDLE1_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@IDLE2_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@WALK_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@RUN_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@THREAT_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@ATTACK1_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [ATTACK1_HITFRAME=10] and shoot hitframe adjustment
-- DESCRIPTION: [@ATTACK2_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [ATTACK2_HITFRAME=10]
-- DESCRIPTION: [@ATTACK3_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [ATTACK3_HITFRAME=10]
-- DESCRIPTION: [@SHOOT_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@HURT_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@DEATH1_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@DEATH2_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@LASTFLAG_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [#LASTFLAG_TIME=3000(0,20000)]
-- DESCRIPTION: [@LASTFLAG_LOOP=2(1=On,2=Off)]
-- DESCRIPTION: [@FORCE_MOVE=2(1=On,2=Off)]
-- DESCRIPTION: [@NPC_TILTING=1(1=On,2=Off)]
-- DESCRIPTION: [@ON_DEATH=1(1=Nothing,2=Activate IfUsed)]
-- DESCRIPTION: [IFUSED$=""]
-- DESCRIPTION: [DIAGNOSTICS!=0]
-- DESCRIPTION: <Sound0> for movement sound
-- DESCRIPTION: <Sound1> for attacking sound effect
-- DESCRIPTION: <Sound2> for threat/hurt sound effect
-- DESCRIPTION: <Sound3> for death sound effect

local U = require "scriptbank\\utillib"
g_LegacyNPC = {}
g_GibsEnabled = {}
local lower = string.lower
local npc_control = {}
local sense_text = {}
local sense_range = {}
local sense_mode = {}
local npc_can_flee = {}
local idle_time = {}
local attack_range = {}
local attack_interval = {}
local attack_damage = {}
local random_damage = {}
local npc_can_roam = {}
local roam_range = {}
local npc_anim_speed = {}
local npc_move_speed = {}
local npc_run_speed = {}
local npc_turn_speed = {}
local npc_can_shoot = {}
local idle1_animation = {}
local idle2_animation = {}
local walk_animation = {}
local run_animation = {}
local threat_animation = {}
local attack1_animation = {}
local attack1_hitframe = {}
local attack2_animation = {}
local attack2_hitframe = {}
local attack3_animation = {}
local attack3_hitframe = {}
local shoot_animation = {}
local hurt_animation = {}
local death1_animation = {}
local death2_animation = {}
local lastflag_animation = {}
local lastflag_time = {}
local lastflag_loop = {}
local force_move = {}
local npc_tilting = {}
local on_death = {}
local ifused = {}
local diagnostics = {}

local name1 = {}
local tdamage = {}
local status = {}
local state = {}
local state_choice = {}
local idlestate_choice = {}
local idle_delay = {}
local attack_delay = {}
local action_delay = {}
local lastflag_delay = {}
local start_health = {}
local init_health = {}
local anim_var = {}
local senseonce = {}
local issensed = {}
local animonce = {}
local idlemonce = {}
local attkonce = {}
local wandonce = {}
local doonce = {}
local patrolonce = {}
local shootonce = {}
local sayonce = {}
local scareonce = {}
local shooting = {}
local plrseen = {}
local heardistance = {}
local lostsight = {}
local currpause = {}
local plrlost = {}
local hurtonce = {}
local actiononce = {}
local aggro = {}
local scare = {}
local wait = {}
local startx = {}
local starty = {}
local startz = {}
local destx = {}
local desty = {}
local destz = {}
local pathdelay = {}
local pointcount = {}
local pointcountp = {}
local callhelponce = {}
local start_sense_range = {}
local regen = {}
local patrolmode = {}
local closestflag = {}
local previousflag = {}
local startfr = {}
local finishfr = {}
local colobj = {}
local playgsound = {}
local plrwithinmesh = {}
local allegiance = {}
local avoidance = {}
local svolume = {}
local svolume_last = {}
local root_time	= {}
local resetstate = {}
local staanim1 = {}
local staanim2 = {}
local staanim3 = {}
local finanim1 = {}
local finanim2 = {}
local finanim3 = {}
local frameadjust1 = {}
local frameadjust2 = {}
local frameadjust3 = {}
local threatanimstr = {}
local threatanimfin = {}
local redirect = {}
local chanceflee = {}
local ishit = {}
local setframes = {}
local dist = {}
local tmphit = {}
local deathifusedno = {}
local has_hit_player = {}

g_GibsEnabled = 0
g_ShowObjectDebugVisuals = 0

function npc_control_properties(e, sense_text, sense_range, sense_mode, npc_can_flee, idle_time, attack_range, attack_damage, random_damage, npc_can_roam, roam_range, npc_anim_speed, npc_move_speed, npc_run_speed, npc_turn_speed, npc_can_shoot, idle1_animation,  idle2_animation, walk_animation, run_animation, threat_animation, attack1_animation, attack1_hitframe, attack2_animation, attack2_hitframe, attack3_animation, attack3_hitframe, shoot_animation, hurt_animation, death1_animation, death2_animation, lastflag_animation, lastflag_time, lastflag_loop, force_move, npc_tilting, on_death, ifused, diagnostics)
	npc_control[e].sense_text = sense_text or ""
	npc_control[e].sense_range = sense_range or 500
	npc_control[e].sense_mode = sense_mode or 1
	npc_control[e].npc_can_flee = npc_can_flee or 2
	npc_control[e].idle_time = idle_time or 3000
	npc_control[e].attack_range = attack_range or 100
	npc_control[e].attack_damage = attack_damage or 10
	npc_control[e].random_damage = random_damage or 1
	npc_control[e].npc_can_roam = npc_can_roam or 1
	npc_control[e].roam_range = roam_range or 500
	npc_control[e].npc_anim_speed = npc_anim_speed or 0.8
	npc_control[e].npc_move_speed = npc_move_speed or 100
	npc_control[e].npc_run_speed = npc_run_speed or 100
	npc_control[e].npc_turn_speed = npc_turn_speed or 100
	npc_control[e].npc_can_shoot = npc_can_shoot or 2
	npc_control[e].idle1_animation = "=" .. tostring(idle1_animation)
	npc_control[e].idle2_animation = "=" .. tostring(idle2_animation)
	npc_control[e].walk_animation = "=" .. tostring(walk_animation)
	npc_control[e].run_animation = "=" .. tostring(run_animation)
	npc_control[e].threat_animation = "=" .. tostring(threat_animation)
	npc_control[e].attack1_animation = "=" .. tostring(attack1_animation)
	npc_control[e].attack1_hitframe = attack1_hitframe or 10
	npc_control[e].attack2_animation = "=" .. tostring(attack2_animation)
	npc_control[e].attack2_hitframe = attack2_hitframe or 10
	npc_control[e].attack3_animation = "=" .. tostring(attack3_animation)
	npc_control[e].attack3_hitframe = attack3_hitframe or 10
	npc_control[e].shoot_animation = "=" .. tostring(shoot_animation)
	npc_control[e].hurt_animation = "=" .. tostring(hurt_animation)
	npc_control[e].death1_animation = "=" .. tostring(death1_animation)
	npc_control[e].death2_animation = "=" .. tostring(death2_animation)
	npc_control[e].lastflag_animation = "=" .. tostring(lastflag_animation)
	npc_control[e].lastflag_time = lastflag_time or 3000
	npc_control[e].lastflag_loop = lastflag_loop or 2
	npc_control[e].force_move = force_move or 2
	npc_control[e].npc_tilting = npc_tilting or 1
	npc_control[e].on_death = on_death or 1
	npc_control[e].ifused = string.lower(ifused) or ""
	npc_control[e].diagnostics = diagnostics or 0
end

function npc_control_init_name(e,name)
	npc_control[e] = {}
	npc_control[e].sense_text = ""
	npc_control[e].sense_range = 500
	npc_control[e].sense_mode = 1
	npc_control[e].npc_can_flee = 2
	npc_control[e].idle_time = 3000
	npc_control[e].attack_range = 100
	npc_control[e].attack_interval = 1000
	npc_control[e].attack_damage = 10
	npc_control[e].random_damage = 1
	npc_control[e].npc_can_roam = 1
	npc_control[e].roam_range = 500
	npc_control[e].npc_anim_speed = 0.8
	npc_control[e].npc_move_speed = 100
	npc_control[e].npc_run_speed = 110
	npc_control[e].npc_turn_speed = 100
	npc_control[e].npc_can_shoot = 2
	npc_control[e].idle1_animation = ""
	npc_control[e].idle2_animation = ""
	npc_control[e].walk_animation = ""
	npc_control[e].run_animation = ""
	npc_control[e].threat_animation = ""
	npc_control[e].attack1_animation = ""
	npc_control[e].attack1_hitframe = 10
	npc_control[e].attack2_animation = ""
	npc_control[e].attack2_hitframe = 10
	npc_control[e].attack3_animation = ""
	npc_control[e].attack3_hitframe = 10
	npc_control[e].shoot_animation = ""
	npc_control[e].hurt_animation = ""
	npc_control[e].death1_animation = ""
	npc_control[e].death2_animation = ""
	npc_control[e].lastflag_animation = ""
	npc_control[e].lastflag_time = 3000
	npc_control[e].lastflag_loop = 2
	npc_control[e].force_move = 2
	npc_control[e].npc_tilting = 1
	npc_control[e].on_death = 1
	npc_control[e].ifused = ""	
	npc_control[e].diagnostics = 0

	action_delay[e] = math.huge
	lastflag_delay[e] = math.huge
	anim_var[e] = 0
	senseonce[e] = 0
	issensed[e] = 0
	animonce[e] = 0
	idlemonce[e] = 0
	attkonce[e] = 0
	wandonce[e] = 0
	patrolonce[e] = 0
	shootonce[e] = 0
	hurtonce[e] = 0
	scareonce[e] = 0
	actiononce[e] = 0
	shooting[e] = 0
	plrseen[e] = 0
	heardistance[e] = 600
	lostsight[e] = 30
	currpause[e] = 0
	plrlost[e] = 0
	sayonce[e] = 0
	aggro[e] = 0
	scare[e] = 0
	wait[e] = math.huge
	status[e] = "init"
	state[e] = "flag_pathing"
	state_choice[e] = 0
	idlestate_choice[e] = 0
	idle_delay[e] = math.huge
	root_time[e] =  math.huge
	startx[e] = 0
	starty[e] = 0
	startz[e] = 0
	destx[e] = 0
	desty[e] = 0
	destz[e] = 0
	startfr[e] = 0
	finishfr[e] = 0
	pointcount[e] = 0
	pointcountp[e] = 0
	pathdelay[e] = math.huge
	patrolmode[e] = 0
	callhelponce[e] = 0
	regen[e] = math.huge
	start_sense_range[e] = 0
	closestflag[e] = 0
	previousflag[e] = 0
	name1[e] = name
	colobj[e] = 0
	allegiance[e] = 0
	avoidance[e] = 0
	playgsound[e] = 0
	plrwithinmesh[e] = 1
	svolume[e] = 0
	svolume_last[e] = 0
	resetstate[e] = 0
	g_LegacyNPC = 0
	g_GibsEnabled = 0
	frameadjust1[e] = 0
	frameadjust2[e] = 0
	frameadjust3[e] = 0
	threatanimstr[e] = 0
	threatanimfin[e] = 0
	init_health[e] = 0
	redirect[e] = 0
	chanceflee[e] = 0
	diagnostics[e] = 0
	ishit[e] = 0
	setframes[e] = 0
	dist[e] = 0
	tmphit[e] = 0
	deathifusedno[e] = 0
	has_hit_player[e] = 0
	math.randomseed(os.time())
	math.random(); math.random(); math.random()
end

function npc_control_main(e)
	CollisionOn(e)
	if status[e] == "init" then
		if npc_control[e].ifused ~= "" and deathifusedno[e] == 0 then
			for p = 1, g_EntityElementMax do
				if p ~= nil and g_Entity[p] ~= nil then
					deathifusedno[e] = p
					if string.lower(GetEntityName(p)) == npc_control[e].ifused then					
						deathifusedno[e] = p
						break
					end
				end
			end
		end	
		allegiance[e] = GetEntityAllegiance(e) -- (0-enemy, 1-ally, 2-neutral)
		SetAnimationSpeed(e,1.0) --SetAnimationSpeed(e,npc_control[e].npc_anim_speed) WAS being used for both ModulateSpeed and SetAnimationSpeed! Messes Up legacy! Fix At 1.0
		SetEntityMoveSpeed(e,npc_control[e].npc_move_speed)
		SetEntityTurnSpeed(e,npc_control[e].npc_turn_speed)
		SetPreExitValue(e,0)
		attack_delay[e] = 10000
		if g_Entity[e]['health'] < 1000 then g_Entity[e]['health'] = g_Entity[e]['health'] + 1000 end
		start_health[e] = g_Entity[e]['health']
		init_health[e] = start_health[e]
		SetEntityHealth(e,start_health[e])
		startx[e] = g_Entity[e]['x']
		starty[e] = g_Entity[e]['y']
		startz[e] = g_Entity[e]['z']
		start_sense_range[e] = npc_control[e].sense_range
		idle_delay[e] = g_Time + npc_control[e].idle_time
		pathdelay[e] = g_Time + 3000
		regen[e] = g_Time + 3000
		idlestate_choice[e] = math.random(1,5)
		if has_hit_player[e] == nil then has_hit_player[e] = 0 end -- Initialize gate
		LoadGlobalSound("audiobank\\user\\" ..name1[e].. ".wav", g_Entity[e])
		if npc_control[e].npc_can_shoot == 3 then SetAttachmentVisible(e,0) end -- 1 sets the entities attachment to be visible (such as their weapon), 0 switches it off
		status[e] = "endinit"
	end

	-- Functions ------------------------------------------
	local function GetFlatDistance(e,x,z)
		local x1,z1 = 0,0
		x1,z1 = g_Entity[e]['x'],g_Entity[e]['z']
		local dx = x-x1
		local dz = z-z1
		local ddist = dx*dx+dz*dz
		return ddist
	end
	local function CheckDistance(e)
		CheckDX = (g_Entity[e]['x'] - startx[e])
		CheckDY = (g_Entity[e]['y'] - starty[e])
		CheckDZ = (g_Entity[e]['z'] - startz[e])
		if math.abs(CheckDY) > 100 then
			CheckDY = CheckDY * 4
		end
		return math.sqrt(math.abs(CheckDX*CheckDX)+math.abs(CheckDY*CheckDY)+math.abs(CheckDZ*CheckDZ));
	end
	local function GetFlatDistanceFromEntity(e,t)
		local x1,z1 = g_Entity[e].x,g_Entity[e].z
		local x2,z2 = g_Entity[t].x,g_Entity[t].z
		local dx = x2-x1
		local dz = z2-z1
		local ddist = dx*dx+dz*dz
		return ddist
	end
	local function GetNextFlag(e,gp)
		local tflag = -1
		for a = 0, 9 do
			local connectede = GetEntityRelationshipID(closestflag[e],a)
			if connectede > 0 then
				if GetEntityMarkerMode(connectede) == 11 then
					if connectede ~= previousflag[e] then
						tflag = connectede
						break
					end
				end
			end
		end
		-- If not a valid flag then end of the path go to the previous flag
		if tflag == -1 then
			if npc_control[e].lastflag_animation > "" then
				lastflag_delay[e] = g_Time + npc_control[e].lastflag_time
				state[e] = "lastflag"
			end
			tflag = previousflag[e]
		end
		-- Change our target flag
		local tflag2 = closestflag[e]
		closestflag[e] = tflag
		previousflag[e] = tflag2
		-- Get path to new flag
		local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
		local tx,ty,tz = GetEntityPosAng(closestflag[e])
		return tx,ty,tz
	end
	-------------------------------------------------------

	if state[e] == "idle" then
		StopSound(e,0)
		if idlemonce[e] == 0 then
			anim_var[e] = math.random(1,2)
			if anim_var[e] == 1 then SetAnimationName(e,npc_control[e].idle1_animation) end -- Idle1
			if anim_var[e] == 2 then SetAnimationName(e,npc_control[e].idle2_animation) end -- Idle2
			LoopAnimation(e)
			idlemonce[e] = 1
		end
		senseonce[e] = 0
		aggro[e] = 0
		scare[e] = 0
		wandonce[e] = 0
		animonce[e] = 0
		resetstate[e] = 0
		if g_Time > idle_delay[e] then
			idlestate_choice[e] = math.random(1,5)
			if idlestate_choice[e] < 5 then
				if npc_control[e].npc_can_roam == 2 then --No Roaming
					state[e] = "idle"
					idlemonce[e] = math.random(0,1)
				end
				if npc_control[e].npc_can_roam == 1 then --Can Roam
					state[e] = "idle"
				end
			end
			if idlestate_choice[e] == 5 then
				if npc_control[e].npc_can_roam == 1 then --Can Roam
					state[e] = "roam"
					wandonce[e] = 0
					idlemonce[e] = 0
				end
			end
			idle_delay[e] = g_Time + npc_control[e].idle_time
		end
	end
	--------------------------------------------------------------------------------------------------------------------------------
	plrwithinmesh[e] = RDIsWithinMesh(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ) -- returns 1 if the XYZ coordinate is on the nav mesh
	allegiance[e] = GetEntityAllegiance(e)

	if GetPlayerDistance(e) > npc_control[e].sense_range and allegiance[e] ~= 0 then
		if npc_control[e].npc_can_roam == 1 then
			if g_Time > action_delay[e] then
				aggro[e] = 1
				state[e] = "idle"
				pathdelay[e] = g_Time + 100
				pointcount[e] = 0
				issensed[e] = 0
			end
		end
	end

	if GetPlayerDistance(e) <= npc_control[e].sense_range and allegiance[e] == 0 then
		g_LegacyNPC = 1
	else
		g_LegacyNPC = 0
	end

	if GetPlayerDistance(e) <= npc_control[e].sense_range and aggro[e] == 0 and allegiance[e] > 2 then
		if npc_control[e].npc_can_roam == 1 then state[e] = "roam" end
	end

	if GetPlayerDistance(e) <= npc_control[e].sense_range and aggro[e] == 0 and allegiance[e] == 0 and plrwithinmesh[e] == 1 and redirect[e] == 0 then
		state[e] = "sensed"
	end

	GetEntityPlayerVisibility(e)
	if GetPlayerDistance(e) > npc_control[e].sense_range and g_Entity[e]['plrvisible'] == 1 and aggro[e] == 0 and allegiance[e] == 0 and plrseen[e] == 0 and plrwithinmesh[e] == 1 then
		npc_control[e].sense_range = GetEntityViewRange(e)
		plrseen[e] = 1
		state[e] = "sensed"
	end

	local flee_range = math.random(2,3)
	if GetPlayerDistance(e) <= npc_control[e].sense_range/flee_range and aggro[e] == 0 and allegiance[e] == 2 then
		if npc_control[e].npc_can_roam == 1 then
			if npc_control[e].npc_can_flee == 1 then
				scare[e] = 1
				state[e] = "roam"
			end
		end
	end

	if GetPlayerDistance(e) <= npc_control[e].sense_range*3 and g_PlayerGunFired == 1 and aggro[e] == 0 and allegiance[e] == 2 then
		if npc_control[e].npc_can_roam == 1 then
			if npc_control[e].npc_can_flee == 1 then
				scare[e] = 1
				RotateY(e,math.random(90,240))
				state[e] = "roam"
			end
		end
	end

	---------------------------------------------------------------------------------------------------------------------------------
	if state[e] == "sensed" then
		GetEntityPlayerVisibility(e)
		if npc_control[e].sense_mode == 1 or  npc_control[e].sense_mode == 2 then
			if GetPlayerDistance(e) < npc_control[e].sense_range and allegiance[e] == 0 then
				issensed[e] = 1
				RotateToPlayerSlowly(e,GetEntityTurnSpeed(e)/2)
				if senseonce[e] == 0 then
					if npc_control[e].npc_can_roam == 1 or npc_control[e].npc_can_roam == 2 then						
						SetAnimationName(e,npc_control[e].threat_animation)
						threatanimstr[e],threatanimfin[e] = GetEntityAnimationStartFinish(e,npc_control[e].threat_animation) -- return the start and finish frame
						ModulateSpeed(e,npc_control[e].npc_anim_speed)
						PlayAnimation(e)
						PlaySound(e,2)
						svolume_last[e] = 2
						if sayonce[e] == 0 then
							PromptDuration(npc_control[e].sense_text,1000)
							sayonce[e] = 1
						end
						senseonce[e] = 1
						action_delay[e] = g_Time + 100
					end
				end

				if npc_control[e].sense_mode == 1 and allegiance[e] == 0 and GetEntityAnimationNameExistAndPlaying(e,npc_control[e].threat_animation) > 0 and g_Entity[e]['frame'] == threatanimfin[e] and g_Time > action_delay[e] then
					if npc_control[e].npc_can_flee == 1 then
						chanceflee[e] = math.random(1,2)
						if (g_Entity[e]['health']-1000) < 100 and redirect[e] == 0 and chanceflee[e] == 2 then
							aggro[e] = 0
							scare[e] = 1
							wandonce[e] = 0
							state[e] = "roam"
							redirect[e] = 1
						end
						if (g_Entity[e]['health']-1000) > 100 then
							aggro[e] = 1
							scare[e] = 0
							hurtonce[e] = 0
							attkonce[e] = 0
							state[e] = "pursue"
							pathdelay[e] = g_Time + 50
							pointcount[e] = 0
						end
					end
					if npc_control[e].npc_can_flee == 2 then
						aggro[e] = 1
						scare[e] = 0
						hurtonce[e] = 0
						attkonce[e] = 0
						senseonce[e] = 0
						state[e] = "pursue"
						pathdelay[e] = g_Time + 50
						pointcount[e] = 0
					end
				end
				if npc_control[e].sense_mode == 2 and allegiance[e] == 0 and g_Time > action_delay[e] then
					if npc_control[e].npc_can_flee == 1 then
						chanceflee[e] = math.random(1,2)
						if (g_Entity[e]['health']-1000) < 100 and redirect[e] == 0 and chanceflee[e] == 2 then
							aggro[e] = 0
							scare[e] = 1
							wandonce[e] = 0
							state[e] = "roam"
							redirect[e] = 1
						end
						if (g_Entity[e]['health']-1000) > 100 then
							aggro[e] = 1
							scare[e] = 0
							hurtonce[e] = 0
							attkonce[e] = 0
							state[e] = "pursue"
							pathdelay[e] = g_Time + 50
							pointcount[e] = 0
						end
					end
					if npc_control[e].npc_can_flee == 2 then
						aggro[e] = 1
						scare[e] = 0
						hurtonce[e] = 0
						attkonce[e] = 0
						senseonce[e] = 0
						state[e] = "pursue"
						pathdelay[e] = g_Time + 50
						pointcount[e] = 0
					end
				end
			end			
			-- Check if player moves out of sense range after being sensed --
			if GetPlayerDistance(e) > npc_control[e].sense_range and allegiance[e] == 0 and issensed[e] == 1 and senseonce[e] == 1 then
				if GetEntityAnimationNameExistAndPlaying(e,npc_control[e].threat_animation) > 0 and g_Entity[e]['frame'] == threatanimfin[e] and g_Time > action_delay[e] then
					aggro[e] = 1
					scare[e] = 0
					hurtonce[e] = 0
					attkonce[e] = 0
					senseonce[e] = 0
					issensed[e] = 0
					state[e] = "pursue"
					pathdelay[e] = g_Time + 50
					pointcount[e] = 0
				end
			end
		end	
		if npc_control[e].sense_mode == 3 or npc_control[e].sense_mode == 4 or npc_control[e].sense_mode == 5 or npc_control[e].sense_mode == 6 and g_Entity[e]['plrvisible'] == 1 then
			if senseonce[e] == 0 then
				RotateToPlayerSlowly(e,GetEntityTurnSpeed(e)/2)
				SetAnimationName(e,npc_control[e].threat_animation)
				threatanimstr[e],threatanimfin[e] = GetEntityAnimationStartFinish(e,npc_control[e].threat_animation)
				ModulateSpeed(e,npc_control[e].npc_anim_speed)
				PlayAnimation(e)
				PlaySound(e,2)
				svolume_last[e] = 2
				if sayonce[e] == 0 then
					PromptDuration(npc_control[e].sense_text,1000)
					sayonce[e] = 1
				end
				senseonce[e] = 1
				action_delay[e] = g_Time + 500
			end	
			if g_Entity[e]['frame'] == threatanimfin[e] or g_Time > action_delay[e] then
				if npc_control[e].sense_mode == 3 then
					LoseGame()
				end
				if npc_control[e].sense_mode == 4 then
					SetEntityIfUsed(e,npc_control[e].ifused)
					ActivateIfUsed(e)
				end
				if npc_control[e].sense_mode == 5 then
					SetEntityIfUsed(e,npc_control[e].ifused)
					ActivateIfUsed(e)
					npc_control[e].sense_mode = 1
				end
				if npc_control[e].sense_mode == 6 then
					JumpToLevel("mapbank\\" ..g_LevelFilename)
				end	
				senseonce[e] = 0
			end	
		end
	end

	if state[e] == "idle" or state[e] == "roam" or state[e] == "die" then
		--End Combat Music
		StopGlobalSound(g_Entity[e])
		playgsound[e] = 0
	end

	if state[e] == "pursue" then
		-- Trigger Combat Music
		if playgsound[e] == 0 then
			PlayGlobalSound(g_Entity[e])
			playgsound[e] = 1
		end
		-----------------------
		svolume[e] = (2000-GetPlayerDistance(e))/10
		SetSound(e,svolume_last[e])
		SetSoundVolume(svolume[e])

		GetEntityPlayerVisibility(e)
		if GetPlayerDistance(e) <= npc_control[e].attack_range and aggro[e] == 1 and npc_control[e].npc_can_shoot == 1 and g_Entity[e]['plrvisible'] == 1 then
			state[e] = "attack"
			attkonce[e] = 0
			ishit[e] = 2
		end
		if GetPlayerDistance(e) <= npc_control[e].attack_range and aggro[e] == 1 and npc_control[e].npc_can_shoot == 2 then
			state[e] = "attack"
			attkonce[e] = 0
			ishit[e] = 2
		end
		if GetPlayerDistance(e) <= npc_control[e].attack_range and aggro[e] == 1 and npc_control[e].npc_can_shoot == 3 and g_Entity[e]['plrvisible'] == 1 then
			state[e] = "attack"
			attkonce[e] = 0
			ishit[e] = 2
		end

		if GetPlayerDistance(e) > npc_control[e].attack_range or GetPlayerDistance(e) < npc_control[e].attack_range and aggro[e] == 1 then
			if g_Time > pathdelay[e] or hurtonce[e] == 1 then
				local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
				destx[e] = g_PlayerPosX
				desty[e] = g_PlayerPosY
				destz[e] = g_PlayerPosZ
				RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
				pointcount[e] = RDGetPathPointCount()
				if pointcount[e] > 0 then
					SetEntityPathRotationMode(e,1)
					if npc_control[e].npc_tilting == 1 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),1,0) end
					if npc_control[e].npc_tilting == 2 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),0,0) end
				end
				GetEntityPlayerVisibility(e)
				if g_Entity[e]['plrvisible'] == 0 or plrwithinmesh[e] == 0 and plrlost[e] == 0 then
					if currpause[e] < lostsight[e] then
						currpause[e] = currpause[e] + 1
					end
					if currpause[e] >= lostsight[e] then
						plrlost[e] = 1
						currpause[e] = 0
					end
				end
				if g_Entity[e]['plrvisible'] == 1 then currpause[e] = 0 end
				if pointcount[e] == 0 or plrlost[e] == 1 then
					if resetstate[e] == 0 then
						local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
						destx[e] = ex
						desty[e] = ey
						destz[e] = ez
						RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
						pointcount[e] = RDGetPathPointCount()
						SetEntityPathRotationMode(e,1)
						StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),1,0)
						npc_control[e].sense_range = start_sense_range[e]
						resetstate[e] = 1
						plrlost[e] = 0
					end
					idlemonce[e] = 0
					aggro[e] = 0
					issensed[e] = 0
					plrseen[e] = 0
					wandonce[e] = 0
					state[e] = "idle"
				end
				pathdelay[e] = g_Time + 100 -- Check for new destination
			end
			if animonce[e] == 0 then
				SetAnimationName(e,npc_control[e].run_animation)
				LoopAnimation(e)
				LoopSound(e,0)
				svolume_last[e] = 0
				animonce[e] = 1
				hurtonce[e] = 0
			end
			SetEntityMoveSpeed(e,npc_control[e].npc_run_speed)
			RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
			MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e))

			if npc_control[e].force_move == 1 then
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ox,oy,oz = U.Rotate3D(0,0,150, 0,math.rad(eay),0)
				colobj[e] = IntersectAll(ex,ey+35,ez,ex+ox,ey+oy+35,ez+oz,g_Entity[e]['obj'])
				if colobj[e] > 0 then
					MoveForward(e,GetEntityMoveSpeed(e)*0)
					RotateY(e,math.random(45,240))
				end
				MoveForward(e,GetEntityMoveSpeed(e)*2)
			end
			shooting[e] = 0
		else
			SetEntityMoveSpeed(e,npc_control[e].npc_move_speed)
		end
	end

	if state[e] == "attack" then
		------------------------CHECK ANIMATION FRAMES --------------------------------------------------------------------------------------------------------------------------
		if setframes[e] == 0 then
			staanim1[e], finanim1[e] = GetEntityAnimationStartFinish(e,npc_control[e].attack1_animation) -- return the start and finish frame of the specified animation in the object
			staanim2[e], finanim2[e] = GetEntityAnimationStartFinish(e,npc_control[e].attack2_animation) -- return the start and finish frame of the specified animation in the object
			staanim3[e], finanim3[e] = GetEntityAnimationStartFinish(e,npc_control[e].attack3_animation) -- return the start and finish frame of the specified animation in the object
			frameadjust1[e] = staanim1[e] + npc_control[e].attack1_hitframe
			frameadjust2[e] = staanim2[e] + npc_control[e].attack2_hitframe
			frameadjust3[e] = staanim3[e] + npc_control[e].attack3_hitframe
			setframes[e] = 1
		end
		------------------------CHECK ANIMATION FRAMES --------------------------------------------------------------------------------------------------------------------------
		if GetEntityAnimationNameExistAndPlaying(e,npc_control[e].attack1_animation) > 0 and g_Entity[e]['frame'] == frameadjust1[e] then ishit[e] = 1 end
		if GetEntityAnimationNameExistAndPlaying(e,npc_control[e].attack2_animation) > 0 and g_Entity[e]['frame'] == frameadjust2[e] then ishit[e] = 1 end
		if GetEntityAnimationNameExistAndPlaying(e,npc_control[e].attack3_animation) > 0 and g_Entity[e]['frame'] == frameadjust3[e] then ishit[e] = 1 end
		-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		GetEntityPlayerVisibility(e)
		if GetPlayerDistance(e) <= npc_control[e].attack_range and g_Entity[e]['health'] > 0 and state[e] ~= "die" and g_Entity[e]['plrvisible'] == 1 then
			animonce[e] = 0
			RotateToPlayer(e)
			if attkonce[e] == 0 then
				has_hit_player[e] = 0 -- Added to ensure gate's open
				if npc_control[e].npc_can_shoot == 2 then --can Melee
					SetAnimationName(e,npc_control[e].attack1_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
					svolume_last[e] = 1
					anim_var[e] = 0
				end
				if npc_control[e].npc_can_shoot == 1 or npc_control[e].npc_can_shoot == 3 then --can Shoot
					SetAnimationName(e,npc_control[e].shoot_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					if shooting[e] == 1 then FireWeapon(e) end
					if shooting[e] == 1 and npc_control[e].force_move == 1 then
						if npc_control[e].random_damage == 1 then HurtPlayer(-1,math.random(1,npc_control[e].attack_damage)) end
						if npc_control[e].random_damage == 2 then HurtPlayer(-1,npc_control[e].attack_damage) end
					end
					anim_var[e] = 5
				end
				attkonce[e] = 1
			end
			if GetTimer(e) >= attack_delay[e] then
				if anim_var[e] == 1 then  -- Attack1
					SetAnimationName(e,npc_control[e].attack1_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					anim_var[e] = 0
				end
				if anim_var[e] == 2 then -- Attack2
					SetAnimationName(e,npc_control[e].attack2_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					anim_var[e] = 0
				end
				if anim_var[e] == 3 then -- Attack3
					SetAnimationName(e,npc_control[e].attack3_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					anim_var[e] = 0
				end
				if anim_var[e] == 4 and npc_control[e].npc_can_roam ~= 2 then -- Threat
					SetAnimationName(e,npc_control[e].threat_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					PlaySound(e,2)
					svolume_last[e] = 2
					anim_var[e] = 0
				end
				if anim_var[e] == 5 then  -- Shoot
					SetAnimationName(e,npc_control[e].shoot_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					if shooting[e] == 1 then FireWeapon(e) end
					if shooting[e] == 1 and npc_control[e].force_move == 1 then
						if npc_control[e].random_damage == 1 then HurtPlayer(-1,math.random(1,npc_control[e].attack_damage)) end
						if npc_control[e].random_damage == 2 then HurtPlayer(-1,npc_control[e].attack_damage) end
					end
					ishit[e] = 2
					anim_var[e] = 0
				end
				if ishit[e] == 0 then attack_delay[e] = GetTimer(e) + 10000 end
				if ishit[e] == 2 then
					if npc_control[e].npc_can_shoot == 1 or npc_control[e].npc_can_shoot == 3 then attack_delay[e] = (GetTimer(e) + 50)+(npc_control[e].attack1_hitframe*2) end
					if npc_control[e].npc_can_shoot == 2 then attack_delay[e] = GetTimer(e) + 10000 end
				end
				if npc_control[e].npc_can_shoot == 2 then anim_var[e] = math.random(1,3) end -- Attack random animation variation
				if npc_control[e].npc_can_shoot == 1 or npc_control[e].npc_can_shoot == 3 then
					anim_var[e] = 5
					shooting[e] = 1
					if GetPlayerDistance(e) < 100 then
						anim_var[e] = math.random(2,3)
						ishit[e] = 0
					end
					if GetPlayerDistance(e) > 100 and GetPlayerDistance(e) < npc_control[e].attack_range then
						ishit[e] = 2
					end
				end
				animonce[e] = 0
			end

			if npc_control[e].npc_can_shoot == 1 or npc_control[e].npc_can_shoot == 3 then
				if ishit[e] == 1 and GetPlayerDistance(e) > 100 and GetPlayerDistance(e) < npc_control[e].attack_range then
					ishit[e] = 2
					attack_delay[e] = GetTimer(e) + 1000
				end
			end

		if ishit[e] == 1 and GetPlayerDistance(e) < npc_control[e].attack_range then
			if has_hit_player[e] == 0 then -- GATE CHECK
				StopSound(e,0)
				PlaySound(e,1)
				svolume_last[e] = 1            
				-- Damage Calculation
				local final_dmg = npc_control[e].attack_damage
				if npc_control[e].random_damage == 1 then 
					final_dmg = math.random(1, npc_control[e].attack_damage) 
				end            
				HurtPlayer(-1, final_dmg)            
				-- Visual/Feedback effects
				GamePlayerControlAddShakeTrauma(math.random(30.9,35.9))
				GamePlayerControlAddShakePeriod(90.0)
				GamePlayerControlAddShakeFade (1.9)
				GamePlayerControlSetShakeTrauma(2.4)
				GamePlayerControlSetShakePeriod(100.0)            
				has_hit_player[e] = 1 -- CLOSE GATE
			end        
			ishit[e] = 0
			attack_delay[e] = GetTimer(e) + 1000
		end

		-- RESET GATE: When the animation is no longer on a hit frame, open the gate for the next swing
		if g_Entity[e]['frame'] ~= frameadjust1[e] and g_Entity[e]['frame'] ~= frameadjust2[e] and g_Entity[e]['frame'] ~= frameadjust3[e] then
			has_hit_player[e] = 0
		end
 
		end
		if GetPlayerDistance(e) < npc_control[e].attack_range and g_Entity[e]['plrvisible'] == 0 then
			state[e] = "pursue"
			attkonce[e] = 0
			attack_delay[e] = GetTimer(e) + 100
		end
		if GetPlayerDistance(e) > npc_control[e].attack_range or g_Entity[e]['plrvisible'] == 0 and aggro[e] == 1 then
			state[e] = "pursue"
			attkonce[e] = 0
			attack_delay[e] = GetTimer(e) + 100
		end
	end

	if state[e] == "roam" then
		svolume[e] = (2000-GetPlayerDistance(e))/10
		SetSound(e,svolume_last[e])
		SetSoundVolume(svolume[e])

		if wandonce[e] == 0 then -- get a random point on a circle around the current location
			if avoidance[e] == 0 then
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ang = math.rad(math.random(1,360))
				dist[e] = npc_control[e].roam_range
				if scare[e] == 0 then dist[e] = npc_control[e].roam_range end
				if scare[e] == 1 then dist[e] = (npc_control[e].roam_range*2) end
				destx[e] = startx[e] + math.cos(ang) * dist[e]
				desty[e] = starty[e]
				destz[e] = startz[e] + math.sin(ang) * dist[e]
			end
			if avoidance[e] == 1 then
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ang = math.rad(math.random(-10,10))
				dist[e] = (npc_control[e].roam_range/90)
				destx[e] = startx[e] + math.cos(ang) * dist[e]
				desty[e] = starty[e]
				destz[e] = startz[e] + math.sin(ang) * dist[e]
			end
			----------------------------------------------------------------
			local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
			local ox,oy,oz = U.Rotate3D(0,0,150, 0,math.rad(eay),0)
			colobj[e] = IntersectAll(ex,ey+35,ez,ex+ox,ey+oy+35,ez+oz,g_Entity[e]['obj'])
			if colobj[e] == nil then colobj[e] = 0 end
			if colobj[e] > 0 then
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ang = math.rad(math.random(-10,10))
				dist[e] = (npc_control[e].roam_range/90)
				destx[e] = startx[e] + math.cos(ang) * dist[e]
				desty[e] = starty[e]
				destz[e] = startz[e] + math.sin(ang) * dist[e]
				avoidance[e] = 1
			end
			----------------------------------------------------------------
			RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
			local result = RDIsWithinMesh(destx[e],desty[e],destz[e])
			if result == 0 then dist[e] = dist[e]-50 end
			pointcount[e] = RDGetPathPointCount()
			if pointcount[e] > 0 then
				if npc_control[e].npc_tilting == 1 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),1,0) end
				if npc_control[e].npc_tilting == 2 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),0,0) end
			end
			if scare[e] == 0 then
				ModulateSpeed(e,npc_control[e].npc_anim_speed)
				SetAnimationName(e,npc_control[e].walk_animation)
				SetEntityMoveSpeed(e,npc_control[e].npc_move_speed)
				LoopAnimation(e)
				LoopSound(e,0)
				svolume_last[e] = 0
				wandonce[e] = 1
				hurtonce[e] = 0
			end
			if scare[e] == 1 then
				ModulateSpeed(e,npc_control[e].npc_anim_speed)
				SetAnimationName(e,npc_control[e].run_animation)
				SetEntityMoveSpeed(e,npc_control[e].npc_run_speed)
				LoopAnimation(e)
				PlaySound(e,0)
				svolume_last[e] = 0
				wandonce[e] = 1
				hurtonce[e] = 0
			end
		end
		RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
		MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e))

		if npc_control[e].force_move == 1 then
			local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
			local ox,oy,oz = U.Rotate3D(0,0,150, 0,math.rad(eay),0)
			colobj[e] = IntersectAll(ex,ey+35,ez,ex+ox,ey+oy+35,ez+oz,g_Entity[e]['obj'])
			if colobj[e] > 0 then RotateY(e,math.random(90,240)) end
			if scare[e] == 0 then
				ModulateSpeed(e,npc_control[e].npc_anim_speed)
				MoveForward(e,GetEntityMoveSpeed(e)/2)
			end
			if scare[e] == 1 then
				if scareonce[e] == 0 then
					RotateY(e,math.random(100,240))
					scareonce[e] = 1
				end
				ModulateSpeed(e,npc_control[e].npc_anim_speed*2)
				MoveForward(e,GetEntityMoveSpeed(e)*2)
			end
			if CheckDistance(e) > npc_control[e].roam_range or colobj[e] > 0 then
				RotateY(e,math.random(45,240))
				state_choice[e] = math.random(1,2)
				if state_choice[e] == 1 then
					state[e] = "idle"
					idle_delay[e] = g_Time + npc_control[e].idle_time
					idlemonce[e] = 0
					state_choice[e] = 0
					wandonce[e] = 0
					hurtonce[e] = 0
					scareonce[e] = 0
					avoidance[e] = 0
					scare[e] = 0
				end
			end
		end
		if GetPlayerDistance(e) <= npc_control[e].sense_range/2 and allegiance[e] == 2 and npc_control[e].npc_can_flee == 1 then
			scare[e] = 1
			RotateY(e,math.random(90,240))
			if scareonce[e] == 0 then
				wandonce[e] = 0
				scareonce[e] = 1
				avoidance[e] = 0
				state[e] = "idle"
			end
		end
		if GetPlayerDistance(e) <= npc_control[e].sense_range/2 and allegiance[e] == 2 and npc_control[e].npc_can_flee == 2 then
			RotateY(e,math.random(90,240))
		end

		if avoidance[e] == 1 and g_Time > root_time[e] then
			aggro[e] = 1
			state[e] = "attack"
		end

		if GetFlatDistance(e,destx[e],destz[e]) <= 150 then
			idlemonce[e] = 0
			state_choice[e] = 0
			wandonce[e] = 0
			animonce[e] = 0
			hurtonce[e] = 0
			scareonce[e] = 0
			scare[e] = 0
			avoidance[e] = 0
			state[e] = "idle"
			idle_delay[e] = g_Time + npc_control[e].idle_time
		end

		if GetPlayerDistance(e) < 100 then RotateY(e,math.random(1,240)) end  -----------------------------------
	else
		MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e))
	end

	if state[e] == "flag_pathing" then
		if closestflag[e] < 1 then
			patrolmode[e] = 0
			for a = 0, 9 do
				local connectede = GetEntityRelationshipID(e,a)
				if connectede > 0 then
					if GetEntityMarkerMode(connectede) == 11 then
						patrolmode[e] = 1
						closestflag[e] = connectede
						previousflag[e] = connectede
						break
					end
				end
			end
			if patrolmode[e] > 0 then
				local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
				local tx,ty,tz = GetEntityPosAng(closestflag[e])
				destx[e] = tx
				desty[e] = ty
				destz[e] = tz
				state[e] = "patrol_flags"
			else
				patrolmode[e] = 0
				state[e] = "idle"
				idle_delay[e] = g_Time + npc_control[e].idle_time
			end
		end
	end

	if state[e] == "patrol_flags" then
		svolume[e] = (2000-GetPlayerDistance(e))/10
		SetSound(e,svolume_last[e])
		SetSoundVolume(svolume[e])
		if patrolonce[e] == 0 then
			ModulateSpeed(e,npc_control[e].npc_anim_speed)
			SetAnimationName(e,npc_control[e].walk_animation)
			LoopAnimation(e)
			LoopSound(e,0)
			svolume_last[e] = 0
			patrolonce[e] = 1
		end
		local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
		RDFindPath(ex,ey,ez, destx[e],desty[e],destz[e])
		pointcountp[e] = RDGetPathPointCount()
		if pointcountp[e] > 0 then
			if npc_control[e].npc_tilting == 1 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),1,0) end
			if npc_control[e].npc_tilting == 2 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),0,0) end
		end
		if npc_control[e].force_move == 1 then MoveForward(e,GetEntityMoveSpeed(e)/2) end
		MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),0)
		if GetFlatDistanceFromEntity(e,closestflag[e]) < 200 then
			destx[e],desty[e],destz[e] = GetNextFlag(e,g_Entity[e])
		end
		if GetPlayerDistance(e) < 100 then RotateY(e,math.random(1,240)) end
	end

	if state[e] == "lastflag" then
		if actiononce[e] == 0 then
			SetAnimationName(e,npc_control[e].lastflag_animation)
			LoopAnimation(e)
			actiononce[e] = 1
		end
		if actiononce[e] == 2 then
			SetAnimationName(e,npc_control[e].idle1_animation)
			LoopAnimation(e)
			actiononce[e] = 2
		end
		if g_Time > lastflag_delay[e] then
			if actiononce[e] == 1 or actiononce[e] == 2 then
				SetAnimationName(e,npc_control[e].walk_animation)
				LoopAnimation(e)
				patrolmode[e] = 1
				if npc_control[e].lastflag_loop == 1 then actiononce[e] = 0 end
				if npc_control[e].lastflag_loop == 2 then actiononce[e] = 2 end
				if previousflag[e] == closestflag[e] then state[e] = "roam" end
				if previousflag[e] ~= closestflag[e] then state[e] = "patrol_flags" end
			end
		end
	end

	if state[e] == "die" then
		if animonce[e] == 0 then
			anim_var[e] = math.random(1,2)  -- Death random animation variation
			if anim_var[e] == 1 then SetAnimationName(e,npc_control[e].death1_animation) end
			if anim_var[e] == 2 then SetAnimationName(e,npc_control[e].death2_animation) end
			ModulateSpeed(e,npc_control[e].npc_anim_speed)
			PlayAnimation(e)			
			animonce[e] = 1
		end
		if g_GibsEnabled == 1 then
			SetEntityHealth(e,1)
		else
			SetEntityHealth(e,999)
		end
		if g_Entity[e]['animating'] == 0 or g_Entity[e]['health'] >= 1 then
			CollisionOff(e)
			animonce[e] = 1
			StopSound(e,0)
			StopSound(e,1)
			StopSound(e,2)
			StopSound(e,3)
			if npc_control[e].on_death == 2 then SetEntityActivated(deathifusedno[e],1) end
			state[e] = "dead"			
		end
	end
	if state[e] == "dead" then		
		SetPreExitValue(e,1)
	end

	--- Check Health and Hurt Animation -------------------------------------

	if g_Time > regen[e] and g_Entity[e]['health'] > 1000 and g_Entity[e]['health'] < init_health[e] then
		SetEntityHealth(e,g_Entity[e]['health']+1)
		regen[e] = g_Time + 2000
	end

	if g_Entity[e]['health'] < 1000 and state[e] ~= "die" then
		local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
		destx[e] = ex
		desty[e] = ey
		destz[e] = ez
		RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
		pointcount[e] = RDGetPathPointCount()
		SetEntityMoveSpeed(e,0)
		animonce[e] = 0
		StopSound(e,0)
		PlaySound(e,3)
		svolume_last[e] = 3
		state[e] = "die"
	end

	if g_Entity[e]['health'] > 1000 then
		if g_Entity[e]['health'] < start_health[e] then
			SetEntityMoveSpeed(e,0)
			if hurtonce[e] == 0 then
				if shooting[e] == 1 then npc_control[e].attack_range = npc_control[e].attack_range + 1 end
				if g_Entity[e]['animating'] == 0 then MoveForward(e,-5) end
				StopAnimation(e)
				SetAnimationName(e,npc_control[e].hurt_animation)
				StopSound(e,0)
				PlaySound(e,2)
				svolume_last[e] = 2
				PlayAnimation(e)
				LookAtPlayer(e)
				hurtonce[e] = 1
				allegiance[e] = 0
			end
			if shooting[e] == 1 then
				npc_control[e].attack_range = npc_control[e].attack_range - 1
			end
			if GetObjectAnimationFinished(e) == 0 then g_Entity[e]['animating'] = 0	end
			if GetObjectAnimationFinished(e) == 1 then g_Entity[e]['animating'] = 1 end
			if g_Entity[e]['animating'] == 1 then
				start_health[e] = g_Entity[e]['health']
				SetEntityHealth(e,start_health[e])
				attkonce[e] = 0
				animonce[e] = 0
				wandonce[e] = 0
				idlemonce[e] = 0
				senseonce[e] = 0
				anim_var[e] = 0
				if aggro[e] == 1 then state_choice[e] = 2 end
				if aggro[e] == 0 then state_choice[e] = math.random(1,2) end
				if state_choice[e] == 1 then
					aggro[e] = 0
					scare[e] = 1
					hurtonce[e] = 0
					if npc_control[e].npc_can_roam == 1 then state[e] = "roam" end
					if npc_control[e].npc_can_roam == 2 then state[e] = "idle" end
					state_choice[e] = 0
				end
				if state_choice[e] == 2 then
					if allegiance[e] > 0 then
						aggro[e] = 0
						scare[e] = 1
						hurtonce[e] = 0
						state[e] = "roam"
					end
					if allegiance[e] == 0 then
						aggro[e] = 1
						scare[e] = 0
						state[e] ="pursue"
						pathdelay[e] = g_Time + 10
						animonce[e] = 0
					end
					state_choice[e] = 0
				end
			end
		end
	end
	--Diagnostic text -----------------------------------
	if npc_control[e].diagnostics == 1 or (g_ShowObjectDebugVisuals == 1 and GetPlayerDistance(e) < 500) then
		local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
		Text(2,30,3,"Entity "..e..": " ..name1[e])
		if allegiance[e] == 0 then Text(2,32,3,"Allegiance : Enemy") end
		if allegiance[e] == 1 then Text(2,32,3,"Allegiance : Ally") end
		if allegiance[e] == 2 then Text(2,32,3,"Allegiance : Neutral") end
		Text(2,34,3,"Anim Speed   : " ..GetAnimationSpeed(e))
		Text(2,36,3,"Move Speed   : " ..GetEntityMoveSpeed(e))
		Text(2,38,3,"Turn Speed   : " ..GetEntityTurnSpeed(e))
		Text(2,40,3,"Current State: " ..state[e])
		Text(2,42,3,"Aggro: " ..aggro[e])
		Text(2,44,3,"Scared: " ..scare[e])
		Text(2,46,3,"Dest Flag #  : " ..closestflag[e])
		Text(2,48,3,"Entity PosX  : " ..ex)
		Text(2,50,3,"Entity PosZ  : " ..ez)
		Text(2,52,3,"Destination X: " ..destx[e])
		Text(2,54,3,"Destination Z: " ..destz[e])
		Text(2,56,3,"Current Health : " ..g_Entity[e]['health']-1000)
		Text(2,58,3,"Roaming Range : " ..npc_control[e].roam_range)
		if plrwithinmesh[e] == 0 then Text(2,60,3,"Player OUTSIDE Navmesh") end
		if plrwithinmesh[e] == 1 then Text(2,60,3,"Player WITHIN Navmesh") end
		Text(2,62,3,"Combat Music : audiobank\\user\\" ..name1[e].. ".wav")
		if GetEntityAnimationNameExistAndPlaying(e,npc_control[e].attack1_animation) > 0 then
			Text(2,64,3,"---------------------------------")
			Text(2,66,3,"Attack 1")
			Text(2,68,3,"Hit Frame1: " ..frameadjust1[e])
			Text(2,70,3,"Current Frame: " ..g_Entity[e]['frame'])
		end
		if GetEntityAnimationNameExistAndPlaying(e,npc_control[e].attack2_animation) > 0 then
			Text(2,64,3,"---------------------------------")
			Text(2,66,3,"Attack 2")
			Text(2,68,3,"Hit Frame2: " ..frameadjust2[e])
			Text(2,70,3,"Current Frame: " ..g_Entity[e]['frame'])
		end
		if GetEntityAnimationNameExistAndPlaying(e,npc_control[e].attack3_animation) > 0 then
			Text(2,64,3,"---------------------------------")
			Text(2,66,3,"Attack 3")
			Text(2,68,3,"Hit Frame3: " ..frameadjust3[e])
			Text(2,70,3,"Current Frame: " ..g_Entity[e]['frame'])
		end
	end
end

function npc_control_preexit(e)
	SetPreExitValue(e,1) -- keep calling preexit until the death animation ends
	if g_Entity[e]['animating'] == 0 then
		SetEntityHealth(e,0)
		SetPreExitValue(e,2) -- when we set this to two, we instruct the engine to stop calling preexit and go to _exit
		SwitchScript(e,"no_behavior_selected.lua")
	end	
end

function npc_control_exit(e)
end