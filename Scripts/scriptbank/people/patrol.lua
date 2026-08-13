-- Patrol v3
-- DESCRIPTION: Character will patrol using flags that are nearest or connected, and has options to [!AllowHeadshot]
-- DESCRIPTION: [!AutoLoopSound] will trigger sound zero to loop
-- DESCRIPTION: <Sound0> - for looping sound
master_interpreter_core = require "scriptbank\\masterinterpreter"

g_patrol = {}
g_patrol_behavior = {}
g_patrol_behavior_count = 0

local svolume = {}

function patrol_init_file(e,scriptfile)
	g_patrol[e] = {}
	g_patrol[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
	g_patrol_behavior_count = master_interpreter_core.masterinterpreter_load (g_patrol[e], g_patrol_behavior )
	patrol_properties(e,1)
end

function patrol_properties(e,allowheadshot,autoloopsound)
	g_patrol[e]['allowheadshot'] = allowheadshot
	g_patrol[e]['autoloopsound'] = autoloopsound or 0
	master_interpreter_core.masterinterpreter_restart (g_patrol[e], g_Entity[e])
end

function patrol_main(e)
	if g_patrol[e] ~= nil and g_patrol_behavior_count > 0 then
		if g_patrol[e]['autoloopsound'] == 1 then
			LoopSound(e,0)
			PlayerDist = GetPlayerDistance(e)
			svolume = (1000-GetPlayerDistance(e))/10
			SetSoundVolume(svolume)	
		end
		local returnvalue = master_interpreter_core.masterinterpreter (g_patrol_behavior, g_patrol_behavior_count, e, g_patrol[e], g_Entity[e])
		if returnvalue >= 0 then
			g_patrol_behavior_count = returnvalue
		else
			local rapiddamageprocessingcount = math.abs(returnvalue)
			for rapiddamageprocessing = 1, rapiddamageprocessingcount do
				local returnvalue = master_interpreter_core.masterinterpreter (g_patrol_behavior, g_patrol_behavior_count, e, g_patrol[e], g_Entity[e])
				if returnvalue >= 0 then
					g_patrol_behavior_count = returnvalue
				end
			end
		end
	end
end

