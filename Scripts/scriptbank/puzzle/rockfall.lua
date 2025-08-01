-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Rockfall v5 - by Necrym59 
-- DESCRIPTION: A Rockfall activated from range or a trigger zone. Always Active=ON. Physics=ON, Gravity=ON, Collision=BOX
-- DESCRIPTION: [PROMPT_TEXT$="Rock Fall"]
-- DESCRIPTION: [ACTIVATION_RANGE=800(1,2000)]
-- DESCRIPTION: [EVENT_DURATION=6(1,10)] in seconds
-- DESCRIPTION: [HIT_DAMAGE=10(1,500)]
-- DESCRIPTION: [HIT_RADIUS=50(1,500)]
-- DESCRIPTION: [START_HEIGHT=10(0,1000)]
-- DESCRIPTION: [@GROUND_SHAKE=1(1=Off, 2=On)],
-- DESCRIPTION: [@HIDE_ROCK=1(1=Off, 2=On)]
-- DESCRIPTION: <Sound0> Rockfall loop

local P = require "scriptbank\\physlib"
local U = require "scriptbank\\utillib"

local rockfall 			= {}
local prompt_text 		= {}
local activation_range 	= {}
local event_duration 	= {}
local hit_damage		= {}
local hit_radius 		= {}
local start_height		= {}
local ground_shake		= {}
local hide_rock			= {}
local status 			= {}
local rock_hit 			= {}
local height 			= {}
local rockfall_time 	= {}
local rocks				= {}
local startx			= {}
local starty			= {}
local startz			= {}
local checktimer		= {}
	
function rockfall_properties(e, prompt_text, activation_range, event_duration, hit_damage, hit_radius, start_height, ground_shake, hide_rock)
	rockfall[e] = g_Entity[e]
	rockfall[e].prompt_text = prompt_text
	rockfall[e].activation_range = activation_range
	rockfall[e].event_duration = event_duration
	rockfall[e].hit_damage	= hit_damage
	rockfall[e].hit_radius = hit_radius
	rockfall[e].start_height = start_height
	rockfall[e].ground_shake = ground_shake
	rockfall[e].hide_rock = hide_rock
end

function rockfall_init(e)
	rockfall[e] = g_Entity[e]
	rockfall[e].prompt_text = "Rock Fall"	
	rockfall[e].activation_range = 800
	rockfall[e].event_duration = 6
	rockfall[e].hit_damage = 10
	rockfall[e].hit_radius = 50
	rockfall[e].start_height = 300
	rockfall[e].ground_shake = 1
	rockfall[e].hide_rock = 1
	rockfall_time[e] = 0
	status[e] = "init"			
	rock_hit[e] = 0	
	GravityOff(e)
	CollisionOff(e)
	rocks[e] = 0
	g_Time = 0
	checktimer[e] = 0
end

function rockfall_main(e)
	rockfall[e] = g_Entity[e]

	if status[e] == "init" then
		if rockfall[e].hide_rock == 2 then Hide(e) end			
		SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'] + rockfall[e].start_height, g_Entity[e]['z'])
		ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'] + rockfall[e].start_height, g_Entity[e]['z'])
		status[e] = "start_event"
	end
	
	if GetPlayerDistance(e) < rockfall[e].activation_range then g_Entity[e]['activated'] = 1 end
	
	if g_Entity[e]['activated'] == 1 then	
		if status[e] == "start_event" then					
			PromptDuration(rockfall[e].prompt_text,3000)
			rockfall_time[e] = g_Time + (rockfall[e].event_duration*1000)
			StartTimer(e)
			Show(e)		
			status[e] = "rockfall"
			GravityOn(e)
			CollisionOn(e)
			PushObject(g_Entity[e]['obj'], math.random(1,1000), math.random(1,188), math.random(1,810/2), 118, 111, 110 )
		end			
		if status[e] == "rockfall" and g_PlayerHealth > 0 then			
			LoopSound(e,0)			
			if GetPlayerDistance(e) < rockfall[e].hit_radius and rock_hit[e] == 0 then
				HurtPlayer(-1,rockfall[e].hit_damage)
				ForcePlayer(0,3)
				rock_hit[e] = 1
			end
			if g_Time > checktimer[e] then
				for _, v in pairs(U.ClosestEntities(80,math.huge,g_Entity[e]['x'],g_Entity[e]['z'])) do
					if GetEntityAllegiance(v) > -1 then
						if g_Entity[v]['health'] > 0 then
							SetEntityHealth(v,g_Entity[v]['health']-rockfall[e].hit_damage)							
						end
						checktimer[e] = g_Time + 250
					end
				end
			end	
			if rockfall[e].ground_shake == 2 then
				if GamePlayerControlSetShakeTrauma ~= nil then
					if g_Time < rockfall_time[e] then
						GamePlayerControlSetShakeTrauma(13.9)
						GamePlayerControlSetShakePeriod(0.2)
					end
					if g_Time > rockfall_time[e] then
						GamePlayerControlSetShakeTrauma(0.0)
						GamePlayerControlSetShakePeriod(0.0)					
					end
				end
			end
		end		
	end		
end
