-- Objective Monitor v2
-- DESCRIPTION: Apply to an entity set as IsObjective + AlwaysVisible and when the player moves within
-- DESCRIPTION: [RANGE=100(1,3000)] will destroy the entity.
-- DESCRIPTION: [MESSAGE$="Objective Reached"] displayed when reached.
-- DESCRIPTION: [@TRIGGER=1(1=Off, 2=On)] linked or IfUsed entities
-- DESCRIPTION: [@VISIBILITY=1(1=Visible,2=Hidden)]

local objective		= {}
local range 		= {}
local message		= {}
local trigger		= {}
local visibility	= {}
local status		= {}

function objective_monitor_properties(e,range, message, trigger, visibility)
	objective[e].range = range
	objective[e].message = message	
	objective[e].trigger = trigger
	objective[e].visibility = visibility
end 

function objective_monitor_init(e)
	objective[e] = {}
	objective[e].range = 100
	objective[e].message = "Objective Reached"
	objective[e].trigger = 1
	objective[e].visibility = 1
	
	status[e] = "init"
	SetEntityAlwaysActive(e,1)
end

function objective_monitor_main(e)
	if status[e] == "init" then
		if objective[e].visibility == 1 then Show(e) end
		if objective[e].visibility == 2 then
			CollisionOff(e)
			Hide(e)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < objective[e].range then
		if GetEntityObjective(e) == 3 then
			PromptDuration(objective[e].message,2000)
			SetEntityObjective(e,0)			
			if objective[e].trigger == 2 then
				PerformLogicConnections(e)
				ActivateIfUsed(e)
			end
			Destroy(e)
		end	
	end
end
