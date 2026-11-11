-- Disturbance v1 by Necrym59
-- DESCRIPTION: The attached object will detect Player and process a selected action and sound
-- DESCRIPTION: [SENSE_RANGE=90(1,1000)]
-- DESCRIPTION: [SENSE_TEXT$="Detected"]
-- DESCRIPTION: [@ACTION_TYPE=1(1=None, 2=Alert)]
-- DESCRIPTION: [ALERT_RANGE=100(10,3000)]
-- DESCRIPTION: <Sound0> - Disturbance Sound

local disturbance = {}
local sense_range = {}
local sense_text = {}
local action_type = {}
local alert_range = {}

local doonce = {}
local played = {}
local status = {}

function disturbance_properties(e,sense_range, sense_text, action_type, alert_range)
	disturbance[e].sense_range = sense_range
	disturbance[e].sense_text = sense_text
	disturbance[e].action_type = action_type
	disturbance[e].alert_range = alert_range
end

function disturbance_init(e)
	disturbance[e] = {}
	disturbance[e].sense_range = 90
	disturbance[e].sense_text = "Detected"
	disturbance[e].action_type = 1
	disturbance[e].alert_range = 100	
	doonce[e] = 0
	played[e] = 0
	status[e] = "init"
end

function disturbance_main(e)

	if status[e] == "init" then		
	end
	
	if GetPlayerDistance(e) < disturbance[e].sense_range then		
		if disturbance[e].action_type == 1 then	--None
			if doonce[e] == 0 then
				Prompt(disturbance[e].sense_text)
				PerformLogicConnections(e)
				ActivateIfUsed(e)						
				doonce[e] = 1			
			end
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end				
		end	
		if disturbance[e].action_type == 2 then	--Alert nearby enemies
			if doonce[e] == 0 then
				Prompt(disturbance[e].sense_text)
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				doonce[e] = 1			
			end
			MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,disturbance[e].alert_range,1,-1)
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end			
		end		
	else	
		doonce[e] = 0
		played[e] = 0		
	end	
end		