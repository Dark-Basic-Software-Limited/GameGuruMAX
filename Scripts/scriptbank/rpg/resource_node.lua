-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Resource Node v24 by Necrym59 and Lee
-- DESCRIPTION: Allows to use this object as a resource node to give the player the selected resource item.
-- DESCRIPTION: [@NODE_TYPE=1(1=Growth, 2=Extraction)]
-- DESCRIPTION: [NODE_TOOL_ACTUAL_NAME$=""]
-- DESCRIPTION: [NODE_RESPAWN_TIME=0(0,100)] Minutes (if 0 then destroyed)
-- DESCRIPTION: [NODE_RESPAWNS=1(1,20)] number of resource node respawns
-- DESCRIPTION: [NODE_RESOURCE_QUANTITY=3(1,10)]
-- DESCRIPTION: [RESOURCE_SPAWN_TIME=1(0,100)] Minutes
-- DESCRIPTION: [RESOURCE_SPAWN_SPREAD=20(0,300)] distance spread
-- DESCRIPTION: [RESOURCE_ENTITY_NAME$=""]
-- DESCRIPTION: [NODE_USE_RANGE=100(0,200)]
-- DESCRIPTION: [NODE_USE_PROMPT$="Extract Resources"]
-- DESCRIPTION: [NODE_TOOL_PROMPT$="Tool required to extract Resources"]
-- DESCRIPTION: [!NODE_SCALER=1]
-- DESCRIPTION: [!HIDE_NODE=0]
-- DESCRIPTION: [!RESOURCE_SCALING=0]
-- DESCRIPTION: <Sound0> for harvest/extraction sound

local P = require "scriptbank\\physlib"
local lower = string.lower

local resnode						= {}
local node_type 					= {}
local node_tool_name				= {}
local node_respawn_time				= {}
local node_respawns					= {}
local node_resource_quantity		= {}
local resource_spawn_time			= {}
local resource_spawn_spread			= {}
local resource_entity_name			= {}
local node_use_range				= {}
local node_use_prompt				= {}
local node_tool_prompt				= {}
local node_scaler					= {}
local hide_node						= {}
local resource_scaling				= {}
local resource_entity_no			= {}

local newposx		= {}
local newposy		= {}
local newposz		= {}
local newEntn		= {}
local cntEntn		= {}
local status		= {}
local doonce		= {}
local playonce		= {}
local nodelist		= {}
local created		= {}
local spawntimer	= {}
local nodetimer		= {}
local nodecount		= {}
local spawning		= {}
local scaler		= {}
local setupnode		= {}
local starthealth	= {}
local healthcheck	= {}
local terrainheight	= {}
local surfaceheight	= {}
local nodewidth		= {}
local nodeheight	= {}
local wait			= {}

function resource_node_properties(e, node_type, node_tool_actual_name, node_respawn_time, node_respawns, node_resource_quantity, resource_spawn_time, resource_spawn_spread, resource_entity_name, node_use_range,  node_use_prompt, node_tool_prompt, node_scaler, hide_node, resource_scaling)
	resnode[e].node_type = node_type
	resnode[e].node_tool_name = string.lower(string.gsub(node_tool_actual_name or "", "/", "\\"))
	resnode[e].node_respawn_time = node_respawn_time
	resnode[e].node_respawns = node_respawns or 1	
	resnode[e].node_resource_quantity = node_resource_quantity
	resnode[e].resource_spawn_time = resource_spawn_time
	resnode[e].resource_spawn_spread = resource_spawn_spread or 0	
	resnode[e].resource_entity_name = lower(resource_entity_name) or ""
	resnode[e].node_use_range = node_use_range
	resnode[e].node_use_prompt = node_use_prompt
	resnode[e].node_tool_prompt	= node_tool_prompt
	resnode[e].node_scaler = node_scaler or 1
	resnode[e].hide_node = hide_node or 0
	resnode[e].resource_scaling = resource_scaling or 0
	resnode[e].resource_entity_no = 0
end

function resource_node_init(e)
	resnode[e] = {}
	resnode[e].node_type = 1
	resnode[e].node_tool_name = ""
	resnode[e].node_respawn_time = 1
	resnode[e].node_respawns = 1	
	resnode[e].node_resource_quantity = 1
	resnode[e].resource_spawn_time = 1	
	resnode[e].resource_spawn_spread = 0	
	resnode[e].resource_entity_name = ""
	resnode[e].node_use_range = 100
	resnode[e].node_use_prompt = "Extract Resources"
	resnode[e].node_tool_prompt	= "Tool required to extract Resources"
	resnode[e].node_scaler = 1
	resnode[e].hide_node = 0
	resnode[e].resource_scaling = 0	
	resnode[e].resource_entity_no = 0	

	newEntn[e] = 0
	cntEntn[e] = 0
	spawntimer[e] = 0
	nodetimer[e] = math.huge	
	spawning[e] = 0
	nodecount[e] = 0
	doonce[e] = 0
	playonce[e] = 0
	scaler[e] = 100	
	setupnode[e] = 0
	created[e] = 0
	starthealth[e] = 500
	terrainheight[e] = 0
	surfaceheight[e] = 0
	nodewidth[e] = 0
	nodeheight[e] = 0	
	healthcheck[e] = starthealth[e]
	wait[e] = math.huge	
	SetEntityAlwaysActive(e,1)
	status[e] = "init"
end

function resource_node_main(e)
	if status[e] == "init" then
		if resnode[e].node_tool_name == "" then resnode[e].node_tool_name = "Any" end
		if resnode[e].node_type == 1 then		
			if resnode[e].hide_node == 0 then
				CollisionOn(e)
			end	
			if resnode[e].hide_node == 1 then
				CollisionOff(e)
				Hide(e)
			end
			spawntimer[e] = g_Time + 2000
			spawning[e] = 1			
			status[e] = "create_resource"
		end
		if resnode[e].node_resource_quantity > 10 then resnode[e].node_resource_quantity = 10 end
		if resnode[e].resource_spawn_time > 60 then resnode[e].resource_spawn_time = 60 end
		if resnode[e].resource_entity_no == 0 then
			for ee = 1, g_EntityElementMax do
				if ee ~= nil and g_Entity[ee] ~= nil then
					if lower(GetEntityName(ee)) == lower(resnode[e].resource_entity_name) then
						resnode[e].resource_entity_no = ee
						SetEntityAlwaysActive(resnode[e].resource_entity_no,1)
						CollisionOff(resnode[e].resource_entity_no)
						Hide(resnode[e].resource_entity_no)
						break
					end
				end
			end			
		end
		if GetEntityVisibility(resnode[e].resource_entity_no) == 1 then
			CollisionOff(resnode[e].resource_entity_no)
			Hide(resnode[e].resource_entity_no)
		end
		local Ent = g_Entity[e]
		local dims = P.GetObjectDimensions(Ent.obj)
		nodewidth[e] = dims.w
		nodeheight[e] = dims.h
		status[e] = "start"
	end
	
	local PlayerDist = GetPlayerDistance(e)	

	if status[e] == "start" then
		if resnode[e].node_type == 1 then			
			if spawning[e] == 0 then
				if resnode[e].resource_spawn_time == 0 then
					spawntimer[e] = g_Time + (1 * 1000)*2
					spawning[e] = 1
				end			
				if resnode[e].resource_spawn_time > 0 then
					spawntimer[e] = g_Time + (resnode[e].resource_spawn_time * 1000)*60
					spawning[e] = 1
				end	
			end
			if created[e] == resnode[e].node_resource_quantity then
				if resnode[e].node_respawn_time == 0 then					
					Destroy(e)
				end
				if resnode[e].node_respawn_time > 0 and spawning[e] == 1 then
					if nodecount[e] < resnode[e].node_respawns then
						nodetimer[e] = g_Time + (resnode[e].node_respawn_time * 1000)*60
						spawning[e] = 2
					end
					if nodecount[e] == resnode[e].node_respawns then
						Hide(e)
						Destroy(e)
					end						
				end
			end
		end
		if resnode[e].node_type == 2 then
			if setupnode[e] == 0 then				
				SetEntityHealth(e,starthealth[e])
				scaler[e] = 100
				Scale(e,scaler[e])
				setupnode[e] = 1
			end
			if g_Entity[e]['health'] <= 0 then PromptLocal(e,"") end
			if PlayerDist < resnode[e].node_use_range and g_Entity[e]['health'] > 0 then
				if resnode[e].node_tool_name ~= g_PlayerGunName and resnode[e].node_resource_quantity > 0 then PromptLocal(e,resnode[e].node_tool_prompt) end
				if resnode[e].node_tool_name == g_PlayerGunName or resnode[e].node_tool_name == "Any" and created[e] < resnode[e].node_resource_quantity then PromptLocal(e,resnode[e].node_use_prompt) end
			end	
			if resnode[e].node_tool_name == g_PlayerGunName or resnode[e].node_tool_name == "Any" then
				if g_Entity[e]['health'] < healthcheck[e] then				
					local drop = math.random(1,2)
					if drop == 2 then
						if resnode[e].node_scaler == 1 then
							CollisionOff(e)
							scaler[e] = scaler[e] - (scaler[e]/resnode[e].node_resource_quantity)
							Scale(e,scaler[e])							
							CollisionOn(e)
						end
						status[e] = "create_resource"
						PlaySound(e,0)						
						healthcheck[e] = g_Entity[e]['health']
						if resnode[e].node_tool_name == g_PlayerGunName or resnode[e].node_tool_name == "Any" then
							SetEntityHealth(e,healthcheck[e])
						else
							SetEntityHealth(e,starthealth[e])
						end
					end				
				end				
				if g_Entity[e]['health'] <= 0 then
					PromptLocal(e,"")
					if created[e] < resnode[e].node_resource_quantity then
						status[e] = "create_resource"
						healthcheck[e] = 1						
						SetEntityHealth(e,healthcheck[e])
					end
				end
				if created[e] == resnode[e].node_resource_quantity then
					PromptLocal(e,"")
					healthcheck[e] = 0
					SetEntityHealth(e,healthcheck[e])
					if resnode[e].node_respawn_time == 0 then						
						PerformLogicConnections(e)
						CollisionOff(e)
						Hide(e)
						Destroy(e)
					end
					if resnode[e].node_respawn_time > 0 and spawning[e] == 0 then
						if nodecount[e] < resnode[e].node_respawns then
							nodetimer[e] = g_Time + (resnode[e].node_respawn_time * 1000)*60
							CollisionOff(e)
							Hide(e)
							spawning[e] = 2
						end
						if nodecount[e] == resnode[e].node_respawns then
							Hide(e)
							Destroy(e)
						end	
					end	
				end	
			end			
		end
	end	

	if status[e] == "create_resource" and resnode[e].resource_entity_no ~= 0 then
		newposx[e] = g_Entity[e]['x'] + nodewidth[e]
		newposy[e] = g_Entity[e]['y'] + nodeheight[e]
		newposz[e] = g_Entity[e]['z'] + nodewidth[e]
		terrainheight[e] = GetTerrainHeight(newposx[e],newposz[e])
		surfaceheight[e] = GetSurfaceHeight(newposx[e],newposy[e],newposz[e])
		if doonce[e] == 0 and created[e] < resnode[e].node_resource_quantity then
			local etoclone = resnode[e].resource_entity_no
			newEntn[e] = SpawnNewEntity(etoclone)
			if resnode[e].resource_scaling == 1 then 
				Scale(newEntn[e],math.random(40,120))
			end	
			Show(newEntn[e])
			GravityOff(newEntn[e])
			CollisionOff(newEntn[e])
			if resnode[e].node_type == 1 then
				if resnode[e].resource_spawn_spread == 0 then 
					local newya = math.random(0,360)
					terrainheight[e] = GetTerrainHeight(newposx[e],newposz[e])
					surfaceheight[e] = GetSurfaceHeight(newposx[e],newposy[e],newposz[e])
					if surfaceheight[e] > terrainheight[e] then
						newposy[e] = surfaceheight[e]
					else
						newposy[e] = terrainheight[e]
					end
					ResetPosition(newEntn[e],newposx[e],newposy[e],newposz[e])
				end	
				if resnode[e].resource_spawn_spread > 0 then 
					local newya = math.random(0,360)
					newposx[e] = g_Entity[e]['x'] + math.cos(newya) * math.random(-resnode[e].resource_spawn_spread,resnode[e].resource_spawn_spread)
					newposz[e] = g_Entity[e]['z'] + math.sin(newya) * math.random(-resnode[e].resource_spawn_spread,resnode[e].resource_spawn_spread)
					terrainheight[e] = GetTerrainHeight(newposx[e],newposz[e])
					surfaceheight[e] = GetSurfaceHeight(newposx[e],newposy[e],newposz[e])
					if surfaceheight[e] > terrainheight[e] then
						newposy[e] = surfaceheight[e]
					else
						newposy[e] = terrainheight[e]
					end
					ResetPosition(newEntn[e],newposx[e],newposy[e],newposz[e])
				end
			end
			if resnode[e].node_type == 2 then
				local newya = math.random(0,360)
				newposx[e] = g_Entity[e]['x'] + math.cos(newya) * math.random(-resnode[e].resource_spawn_spread,resnode[e].resource_spawn_spread)
				newposz[e] = g_Entity[e]['z'] + math.sin(newya) * math.random(-resnode[e].resource_spawn_spread,resnode[e].resource_spawn_spread)
				terrainheight[e] = GetTerrainHeight(newposx[e],newposz[e])
				surfaceheight[e] = GetSurfaceHeight(newposx[e],newposy[e],newposz[e])
				if surfaceheight[e] > terrainheight[e] then
					newposy[e] = surfaceheight[e] + math.random(10,30)
				else
					newposy[e] = terrainheight[e] + math.random(10,30)
				end
				ResetPosition(newEntn[e],newposx[e],newposy[e],newposz[e])
			end			
			CollisionOn(newEntn[e])
			GravityOn(newEntn[e])
			cntEntn[e] = cntEntn[e] + 1
			created[e] = created[e] + 1
			wait[e] = g_Time + 600			
			doonce[e] = 1
		end
		if g_Time > wait[e] then status[e] = "resource_created" end
	end
	
	if status[e] == "resource_created" then
		doonce[e] = 0
		status[e] = "start"	
	end	
	
	if g_Time > spawntimer[e] and spawning[e] == 1 then
		status[e] = "create_resource"
		spawning[e] = 0
	end
	if g_Time > nodetimer[e] and spawning[e] == 2 then
		nodecount[e] = nodecount[e] + 1
		SetEntityHealth(e,starthealth[e])
		healthcheck[e] = starthealth[e]
		Scale(e,100)
		scaler[e] = 100
		created[e] = 0		
		if resnode[e].node_type == 1 then
			if resnode[e].hide_node == 1 then 
				CollisionOff(e)
				Hide(e)
			end	
			spawning[e] = 1
			status[e] = "create_resource"
		end		
		if resnode[e].node_type == 2 then
			CollisionOn(e)
			Show(e)
			spawning[e] = 0
			setupnode[e] = 0
			status[e] = "start"
		end
	end
end