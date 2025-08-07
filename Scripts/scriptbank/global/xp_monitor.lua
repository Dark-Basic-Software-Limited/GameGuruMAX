-- XP MONITOR v1 by Necrym59
-- DESCRIPTION: This global behavior will monitor npc's and award an xp addition or deduction on the following conditions
-- DESCRIPTION: [ENEMY_KILL_ADD=50]
-- DESCRIPTION: [ALLY_KILL_DEDUCT=50]
-- DESCRIPTION: [NEUTRAL_KILL_ADD=0]
-- DESCRIPTION: [NEUTRAL_KILL_DEDUCT=0]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$="MyXP"(0=globallist)] User global to be affected (eg: MyXP)
-- DESCRIPTION: [KILL_PROMPT!=0]

local xp_monitor				= {}
local prompt_text 				= {}
local enemy_kill_add			= {}
local ally_kill_deduct			= {}
local neutral_kill_add			= {}
local neutral_kill_deduct		= {}
local user_global_affected		= {}
local kill_prompt				= {}

local status		= {}
local currentvalue	= {}
local entitykilled	= {}

function xp_monitor_properties(e, enemy_kill_add, ally_kill_deduct, neutral_kill_add, neutral_kill_deduct, user_global_affected, kill_prompt)
	xp_monitor[e].enemy_kill_add = enemy_kill_add	
	xp_monitor[e].ally_kill_deduct = ally_kill_deduct
	xp_monitor[e].neutral_kill_add = neutral_kill_add
	xp_monitor[e].neutral_kill_deduct = neutral_kill_deduct	
	xp_monitor[e].user_global_affected = user_global_affected
	xp_monitor[e].kill_prompt = kill_prompt or 0
end

function xp_monitor_init(e)
	xp_monitor[e] = {}
	xp_monitor[e].enemy_kill_add = 50	
	xp_monitor[e].ally_kill_deduct = 50
	xp_monitor[e].neutral_kill_add = 0
	xp_monitor[e].neutral_kill_deduct = 0		
	xp_monitor[e].user_global_affected = "MyXP"
	xp_monitor[e].kill_prompt = 0

	status[e] = "init"
	currentvalue[e] = 0
	entitykilled = nil
	SetEntityAlwaysActive(e,1)
end

function xp_monitor_main(e)

	if status[e] == "init" then
		status[e] = "monitor"
	end

	if status[e] == "monitor" then
		entitykilled = GetNearestEntityDestroyed(0)
		if entitykilled > 0 then
			local allegiance = GetEntityAllegiance(entitykilled) -- get the allegiance value for this object (-1-none, 0-enemy, 1-ally, 2-neutral)
			if allegiance == 0 then
			if xp_monitor[e].kill_prompt == 1 then PromptDuration("ENEMY KILLED",2000) end
				if xp_monitor[e].enemy_kill_add ~= 0 then
					if xp_monitor[e].user_global_affected ~= "" then
						if _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] = currentvalue[e] + xp_monitor[e].enemy_kill_add						
					end
					entitykilled = 0
				end
			end
			if allegiance == 1 then
				if xp_monitor[e].kill_prompt == 1 then PromptDuration("ALLY KILLED",2000) end
				if xp_monitor[e].ally_kill_deduct ~= 0 then
					if xp_monitor[e].user_global_affected ~= "" then
						if _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] = currentvalue[e] - xp_monitor[e].ally_kill_deduct
					end
					entitykilled = 0
				end			
			end
			if allegiance == 2 then
				if xp_monitor[e].kill_prompt == 1 then PromptDuration("NEUTRAL KILLED",2000) end
				if xp_monitor[e].neutral_kill_add ~= 0 then
					if xp_monitor[e].user_global_affected ~= "" then
						if _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] = currentvalue[e] + xp_monitor[e].neutral_kill_add
					end
					entitykilled = 0
				end
				if xp_monitor[e].neutral_kill_deduct ~= 0 then
					if xp_monitor[e].user_global_affected ~= "" then
						if _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] = currentvalue[e] - xp_monitor[e].neutral_kill_deduct
					end
					entitykilled = 0
				end
			end
			if _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] <= 0 then _G["g_UserGlobal['"..xp_monitor[e].user_global_affected.."']"] = 0 end
		end		
	end	
end
