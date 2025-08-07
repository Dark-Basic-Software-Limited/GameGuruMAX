-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Object Collider v7 
-- DESCRIPTION: The attached object will detect collisions with object and/or terrain.
-- DESCRIPTION: [TERRAIN_DISTANCE=10(0,100)]
-- DESCRIPTION: [#COLLISION_FORCE=0.05(0.00,0.1)]
-- DESCRIPTION: [@DETECTION_TYPE=2(1=Terrain Collision, 2=Object Collision, 3=Terrain+Object Collision)]
-- DESCRIPTION: [@DETECTION_TRIGGER=1(1=Off, 2=On)] to activate logic linked or IfUsed Entities
-- DESCRIPTION: [@FORCE_BOUNCE=1(1=Off, 2=On)] to bounce object
-- DESCRIPTION: [@DESTROY_COLLIDER=1(1=Off, 2=On)] to delete the colliding object
-- DESCRIPTION: [#DESTROY_DELAY=0.5(0.0,5.0)] delay time to deletion
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] User Global that will be affected by a collision
-- DESCRIPTION: [AFFECTED_VALUE=10(0,100)]
-- DESCRIPTION: [@VALUE_PROCESS=1(1=Add, 2=Deduct)]
-- DESCRIPTION: <Sound0> plays on collision with terrain
-- DESCRIPTION: <Sound1> plays collision with object

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"

local collider 				= {}
local terrain_distance 		= {}
local collision_force 		= {}
local detection_type 		= {}
local detection_trigger 	= {}
local force_bounce			= {}
local destroy_collider		= {}
local destroy_delay			= {}
local user_global_affected	= {}
local affected_value		= {}
local value_process			= {}

local terColPos		= {}
local checkInterval	= {}
local checkTimer	= {}
local initialise	= {}
local timeNow 		= {}
local playonce 		= {}
local playtime 		= {}
local doonce 		= {}
local collisionlist	= {}
local collidedEnt	= {}
local collidetimer	= {}
local currentvalue	= {}
local resettimer	= {}

function object_collider_properties(e,terrain_distance, collision_force, detection_type, detection_trigger, force_bounce, destroy_collider, destroy_delay, user_global_affected, affected_value, value_process)
	collider[e].terrain_distance = terrain_distance
	collider[e].collision_force = collision_force
	collider[e].detection_type = detection_type or 2
	collider[e].detection_trigger = detection_trigger or 1
	collider[e].force_bounce = force_bounce or 1
	collider[e].destroy_collider = destroy_collider or 1
	collider[e].destroy_delay = destroy_delay
	collider[e].user_global_affected = user_global_affected
	collider[e].affected_value = affected_value
	collider[e].value_process = value_process
end

function object_collider_init(e)
	collider[e] = {}
	collider[e].terrain_distance = 10
	collider[e].collision_force = 0.05
	collider[e].detection_type = 2
	collider[e].detection_trigger =	1
	collider[e].force_bounce = 1
	collider[e].destroy_collider = 1
	collider[e].destroy_delay = 1.0	
	collider[e].user_global_affected = ""
	collider[e].affected_value = 0
	collider[e].value_process = 1
	
	checkInterval[e] = 100
	terColPos[e] = 0
	timeNow[e] = math.huge
	checkTimer[e] = nil
	playonce[e] = 0
	doonce[e] = 0
	collisionlist[e] = {}
	collidedEnt[e] = 0
	collidetimer[e] = math.huge	
	resettimer[e] = 0
	SetEntityAlwaysActive(e,1)
	P.AddEntityCollisionCheck(e)
end

function object_collider_main(e)
	
	if checkTimer[e] == nil then 
		checkTimer[e] = g_Time + 1000 
		initialise[e] = true
		terColPos[e] = { x = 0, y = 0, z = 0 }				
	elseif 
		g_Time > checkTimer[e] then
			initialise[e] = false
			checkTimer[e] = g_Time + 50
			CheckForCollision(e)
	elseif
		initialise[e] then
			CheckForCollision(e,true)
	end
	if collidedEnt[e] > 0 then
		if collider[e].destroy_collider == 2 then
			if g_Time > collidetimer[e] then 
				Destroy(collidedEnt[e])
				collidedEnt[e] = 0
				collidetimer[e] = math.huge
			end
		end	
	end	
end

function CheckForCollision(e,checkOnly)

	if 0 < P.GetEntityTerrainNumCollisions(e) then		
		collisionlist[e] = P.GetEntityTerrainCollisionDetails(e)
		tP = terColPos[e]
		for _, v in ipairs(collisionlist[e]) do
			if not U.CloserThan( tP.x, tP.y, tP.z, v.x, v.y, v.z, collider[e].terrain_distance ) then
				if not checkOnly then
					if playonce[e] == 0 then
						PlaySoundIfSilent(e,0)
						playonce[e] = 1
					end
					if collider[e].detection_type == 1 or collider[e].detection_type == 3 then
						if doonce[e] == 0 then						
							if collider[e].detection_trigger == 2 then
								PerformLogicConnections(e)
								ActivateIfUsed(e)
							end
							if collider[e].user_global_affected ~= "" then
								if _G["g_UserGlobal['"..collider[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..collider[e].user_global_affected.."']"] end
								if collider[e].value_process == 1 then --add				
									_G["g_UserGlobal['"..collider[e].user_global_affected.."']"] = currentvalue[e] + collider[e].affected_value
								end
								if collider[e].value_process == 2 then --deduct				
									_G["g_UserGlobal['"..collider[e].user_global_affected.."']"] = currentvalue[e] - collider[e].affected_value
								end							
								if collider[e].force_bounce == 2 then
									PushObject(g_Entity[e]['obj'],math.random(0,0.01), math.random(0.1,1.9), math.random(0,0.01), math.random(0,1),  math.random(0,3),  math.random(0,1) )	
								end
							end	
							resettimer[e] = g_Time + 300
							doonce[e] = 1
						end						
					end
				end
			end
			terColPos[e] = { x = v.x, y = v.y, z = v.z }
		end
	end
	
	if 0 < P.GetEntityObjectNumCollisions(e) then
		collisionlist[e] = P.GetEntityObjectCollisionDetails(e)
		for _, v in ipairs(collisionlist[e]) do
			if v.f > collider[e].collision_force/10 and not checkOnly then
				PlaySound(e,1)				
				collidedEnt[e] = P.ObjectToEntity(v.obj)
				collidetimer[e]	= g_Time + (collider[e].destroy_delay * 1000)		
				if collider[e].detection_type == 2 or collider[e].detection_type == 3 then
					if doonce[e] == 0 then
						if collider[e].detection_trigger == 2 then
							PerformLogicConnections(e)
							ActivateIfUsed(e)
						end
						if collider[e].user_global_affected ~= "" then
							if _G["g_UserGlobal['"..collider[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..collider[e].user_global_affected.."']"] end
							if collider[e].value_process == 1 then --add				
								_G["g_UserGlobal['"..collider[e].user_global_affected.."']"] = currentvalue[e] + collider[e].affected_value
							end
							if collider[e].value_process == 2 then --deduct				
								_G["g_UserGlobal['"..collider[e].user_global_affected.."']"] = currentvalue[e] - collider[e].affected_value
							end
						end	
						resettimer[e] = g_Time + 300
						doonce[e] = 1
					end
					if collider[e].force_bounce == 2 then
						PushObject(g_Entity[e]['obj'],math.random(0,0.01), math.random(0.1,1.9), math.random(0,0.01), math.random(0,1),  math.random(0,3),  math.random(0,1) )						
					end										
				end					
				break				
			end			
		end		
	end
	
	if g_Time >= resettimer[e] then
		doonce[e] = 0	
	end
end