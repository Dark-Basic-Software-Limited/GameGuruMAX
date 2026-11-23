-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Flying Insect v9 by Necrym59
-- All flying insect animations have two animations
-- DESCRIPTION: Applied to insect? Always active ON
-- DESCRIPTION: [@INSECT_TYPE=1(1=Butterfly, 2=Bee, 3=Wasp, 4=Dragonfly)]
-- DESCRIPTION: [@TEMPERAMENT=1(1=Benign, 2=Aggressive)]
-- DESCRIPTION: [FLIGHT_BOUNDARY=500(1,2000)]
-- DESCRIPTION: [ATTACK_RANGE=80(50,500)]
-- DESCRIPTION: [ATTACK_INTERVAL=250(1,500)]
-- DESCRIPTION: [@FLIGHT_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [@ATTACK_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: <Sound0> Insect Flying sound
-- DESCRIPTION: <Sound1> Insect Attacking sound

local insect = {}
local insect_type = {}
local temperament = {}
local flight_boundary = {}
local attack_range = {}
local attack_interval = {}
local flight_animation = {}
local attack_animation = {}

local insect_state = {}
local rotation_movement = {}
local vertical_movement = {}
local forward_movement = {}	
local darting_movement = {}
local hover_movement = {}
local pattern_time = {}
local attack_delay = {}
local death = {}
local start_animation = {}
local x_position = {}
local y_position = {}
local z_position = {}
local insect_position = {}
local insect_sound = {}	
local terrainheight = {}
local flight_boundary_state = {}
local doonce = {}
local status = {}

function flying_insect_properties(e, insect_type, temperament, flight_boundary, attack_range, attack_interval, flight_animation, attack_animation)
	insect[e].insect_type = insect_type
	insect[e].temperament = temperament
	insect[e].flight_boundary = flight_boundary
	insect[e].attack_range = attack_range
	insect[e].attack_interval = attack_interval
	insect[e].flight_animation = "=" .. tostring(flight_animation)
	insect[e].attack_animation = "=" .. tostring(attack_animation)	
end

function flying_insect_init_name(e)
	insect[e] = {}
	insect[e].insect_type = 1
	insect[e].temperament = 1
	insect[e].flight_boundary = 500
	insect[e].attack_range = 80
	insect[e].attack_interval = 100	
	insect[e].flight_animation = "fly"
	insect[e].attack_animation = "attack"
	
	insect_state[e] = 1
	rotation_movement[e] = 0	
	vertical_movement[e] = 0
	forward_movement[e] = 0	
	darting_movement[e] = 0
	hover_movement[e] = 0
	pattern_time[e] = math.huge
	start_animation[e] = 0
	x_position[e] = 0
	z_position[e] = 0
	y_position[e] = 0
	insect_position[e] = 0
	insect_sound[e] = 0
	attack_delay[e] = 300
	death[e] = 0
	terrainheight[e] = nil
	flight_boundary_state[e] = "in_bounds"
	doonce[e] = 0
	status[e] = "init"	
end
    
function flying_insect_main(e)

	if status[e] == "init" then	
		if insect[e].flight_animation == "" then insect[e].flight_animation = "fly" end
		if insect[e].attack_animation == "" then insect[e].attack_animation = "attack" end
		x_position[e] = g_Entity[e]['x']
		y_position[e] = g_Entity[e]['y']
		z_position[e] = g_Entity[e]['z']
		GravityOff(e)
		CollisionOff(e)
		terrainheight[e] = GetTerrainHeight(g_Entity[e]['x'],g_Entity[e]['z'])
		pattern_time[e] = g_Time + 3000
		if g_Entity[e]['health'] <= 1 then SetEntityHealth(e,g_Entity[e]['health']+10) end
		status[e] = "endinit"
	end	
	-- Flying Patterns -------------------------------
	if insect[e].insect_type == 1 then -- Butterfly
		rotation_movement[e] = math.random(-160,200)
		vertical_movement[e] = math.random(-50,50)
		forward_movement[e] = 50
		if g_Time > pattern_time[e] then
			darting_movement[e] = math.random(0,0)
			hover_movement[e] = math.random(0,0)
			pattern_time[e] = g_Time + 3000
		end
		ModulateSpeed(e,2.0)			
	end
	if insect[e].insect_type == 2 then -- Bee
		rotation_movement[e] = math.random(-170,200)
		vertical_movement[e] = math.random(-20,20)
		forward_movement[e] = 20
		if g_Time > pattern_time[e] then
			darting_movement[e] = math.random(0,0)
			hover_movement[e] = math.random(0,0)
			pattern_time[e] = g_Time + 3000
		end
		ModulateSpeed(e,1.5)
	end
	if insect[e].insect_type == 3 then -- Wasp
		rotation_movement[e] = math.random(-160,220)
		vertical_movement[e] = math.random(-15,15)
		forward_movement[e] = 25
		if g_Time > pattern_time[e] then
			darting_movement[e] = math.random(0,0)
			hover_movement[e] = math.random(0,1)
			pattern_time[e] = g_Time + 3000
		end
		ModulateSpeed(e,1.5)
	end
	if insect[e].insect_type == 4 then -- Dragonfly
		rotation_movement[e] = math.random(-160,220)
		forward_movement[e] = 30
		if g_Time > pattern_time[e] then
			vertical_movement[e] = math.random(-1,1)
			darting_movement[e] = math.random(0,50)
			hover_movement[e] = math.random(0,1)
			pattern_time[e] = g_Time + 3000
		end
		ModulateSpeed(e,1.5)
	end
	-- Sound --------------------------------------
	if g_Entity[e]['health'] > 1 then LoopSound(e,0) end
	-- Timers -----------------------------------		
	if InsectDistance(e) > insect[e].flight_boundary then
		flight_boundary_state[e] = "out_of_bounds"
	elseif InsectDistance(e) < insect[e].flight_boundary then
		flight_boundary_state[e] = "in_bounds"
	end
	
	if g_Entity[e]['animating'] == 0 and start_animation[e] == 1 then
		SetAnimationName(e,insect[e].flight_animation)
		PlayAnimation(e)
		g_Entity[e]['animating'] = 1
	end
	-- Flying -------------------------------------		
	if insect_state[e] == 1 and flight_boundary_state[e] ~= "out_of_bounds" then
		if hover_movement[e] == 0 then MoveForward(e,forward_movement[e]) end
		if hover_movement[e] == 1 then MoveForward(e,0) end	
		if darting_movement[e] > 40 then
			MoveForward(e,100)
			ModulateSpeed(e,2.5)
		end		
		MoveUp(e,vertical_movement[e])
		if g_Entity[e]['y'] < (terrainheight[e] + 100) and g_Entity[e]['y'] > (terrainheight[e] + 10) then	
			MoveUp(e,vertical_movement[e])
		elseif g_Entity[e]['y'] > (terrainheight[e] + 50) then
			MoveUp(e,-vertical_movement[e])
		elseif g_Entity[e]['y'] < (terrainheight[e] + 10) then
			MoveUp(e,20)		
		end
		RotateY(e,rotation_movement[e])		
	end	
	-- Attacking -------------------------------------	
	if insect_state[e] == 2 then
		RotateToPlayer(e)
		if g_Entity[e]['y'] > g_PlayerPosY then
			CollisionOff(e)
			MoveUp(e,-20) 
			CollisionOn(e)
		elseif g_Entity[e]['y'] < (terrainheight[e]+50) then
			CollisionOff(e)
			MoveUp(e,40)
			CollisionOn(e)
		end	
		RotateToPlayer(e)
		if GetPlayerDistance(e) > 10 then 
			CollisionOff(e)
			MoveForward(e,forward_movement[e]*4)
			CollisionOn(e)
		end				
		if GetTimer(e) > attack_delay[e] then
			if doonce[e] == 0 then
				SetAnimationName(e,insect[e].attack_animation)
				LoopAnimation(e)
				doonce[e] = 1
			end
			PlaySound(e,1)
			HurtPlayer(e,1)
			attack_delay[e] = GetTimer(e) + insect[e].attack_interval
		end
	end
	-- flight_boundary -------------------------------------	
	if flight_boundary_state[e] == "out_of_bounds" then
		RotateY(e,math.random(160,200))
		MoveForward(e,forward_movement[e]/2)
		MoveUp(e,vertical_movement[e])
	end
	-- Temperament Check -----------------------------
	if GetPlayerDistance(e) > insect[e].attack_range and insect[e].temperament == 1 then insect_state[e] = 1 end
	if GetPlayerDistance(e) < insect[e].attack_range and insect[e].temperament == 2 then insect_state[e] = 2 end
	if GetPlayerDistance(e) > insect[e].attack_range and insect[e].temperament == 2 then				
		doonce[e] = 0
		if insect_state[e] == 2 and doonce[e] == 0 then
			SetAnimationName(e,insect[e].flight_animation)
			LoopAnimation(e)			
			doonce[e] = 1
		end
		doonce[e] = 0
		insect_state[e] = 1
		status[e] = "init"
	end	
	if g_Entity[e]['health'] < 2 then 
		doonce[e] = 0
		if insect_state[e] == 2 and doonce[e] == 0 then
			SetAnimationName(e,insect[e].flight_animation)
			LoopAnimation(e)
			doonce[e] = 1
		end
		doonce[e] = 0		
		insect_state[e] = 1		
		status[e] = "init"
	end
	-- Dying -------------------------------------	
	if g_Entity[e]['health'] <= 1 then
		insect_state[e] = 0		
		StopAnimation(e)
		GravityOn(e)
		StopSound(e,0)
		g_Entity[e]['health'] = 0
		if g_Entity[e]['y'] > terrainheight[e] then
			MoveUp(e,-18)
		end
		status[e] = "init"
	end
end
	   
function InsectDistance(e)
	insectDX = (g_Entity[e]['x'] - x_position[e])
	insectDY = (g_Entity[e]['y'] - y_position[e])
	insectDZ = (g_Entity[e]['z'] - z_position[e])
	if math.abs(insectDY) > 100 then insectDY = insectDY * 4 end
	return math.sqrt(math.abs(insectDX*insectDX)+math.abs(insectDY*insectDY)+math.abs(insectDZ*insectDZ));
end 