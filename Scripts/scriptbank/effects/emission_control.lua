-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Emission Control v5 by Necrym59
-- DESCRIPTION: Will change the emissive level and/or color of an object.
-- DESCRIPTION: Can also trigger other logic linked or IfUsed entities when activated.
-- DESCRIPTION: Attach to an object and activate from a switch or zone.
-- DESCRIPTION: Set Physics=ON or OFF and IsImmobile=Yes
-- DESCRIPTION: [#OFF_STRENGTH=0.0] Off strength.
-- DESCRIPTION: [#ON_STRENGTH=1000.0] On strength.
-- DESCRIPTION: [FADE_SPEED=50(1,100)] On/Off Fade Speed
-- DESCRIPTION: [R_VALUE=0(0,255)]
-- DESCRIPTION: [G_VALUE=0(0,255)]
-- DESCRIPTION: [B_VALUE=0(0,255)]
-- DESCRIPTION: [@ACTIVATE_LOGIC=1(1=Off, 2=On Activation, 3=On Deactivation, 4=On Activation+Deactivation)]
-- DESCRIPTION: [@MODE=1(1=Triggered Activation, 2=Triggered Fixed Interval+Count, 3=Use Day/Night Cycle)] Mode of activation
-- DESCRIPTION: [#FIXED_INTERVAL=1.0]
-- DESCRIPTION: [FIXED_COUNT=5]
-- DESCRIPTION: [@START_STATE=1(1=Off, 2=On)] Initial starting state
-- DESCRIPTION: <Sound0> Activation sound

g_sunrollposition = {}

local emc = {}
local off_strength 	= {}
local on_strength 	= {}
local fade_speed	= {}
local r_value 		= {}
local g_value 		= {}
local b_value 		= {}
local activate_logic= {}
local mode			= {}
local fixed_interval= {}
local fixed_count	= {}
local start_state 	= {}

local status		= {}
local doonce		= {}
local current_state	= {}
local current_level = {}
local current_tod	= {}
local rwait			= {}
local count			= {}
local played		= {}

function emission_control_properties(e, off_strength, on_strength, fade_speed, r_value, g_value, b_value, activate_logic, mode, fixed_interval, fixed_count, start_state)
	emc[e].off_strength = off_strength
	emc[e].on_strength = on_strength
	emc[e].fade_speed = fade_speed
	emc[e].r_value = r_value
	emc[e].g_value = g_value
	emc[e].b_value = b_value
	emc[e].activate_logic = activate_logic
	emc[e].mode = mode or 1
	emc[e].fixed_interval = fixed_interval
	emc[e].fixed_count = fixed_count
	emc[e].start_state = start_state or 1
end

function emission_control_init(e)
	emc[e] = {}
	emc[e].off_strength = 0
	emc[e].on_strength = 0
	emc[e].fade_speed = 1	
	emc[e].r_value = 0
	emc[e].g_value = 0
	emc[e].b_value = 0
	emc[e].activate_logic = 1
	emc[e].mode = 1
	emc[e].fixed_interval = 1.0
	emc[e].fixed_count = 5	
	emc[e].start_state = 1	

	status[e] = "init"
	doonce[e] = 0
	current_state[e] = ""
	current_level[e] = 0
	current_tod[e] = ""
	rwait[e] = math.huge
	count[e] = 0
	played[e] = 0
	
	SetEntityEmissiveStrength(e,emc[e].off_strength)
	SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
	SetActivated(e,0)
end

function emission_control_main(e)

	if status[e] == "init" then	
		if emc[e].start_state == 1 then
			SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
			SetEntityEmissiveStrength(e,emc[e].off_strength)			
			current_state[e] = "is-off"
		end
		if emc[e].start_state == 2 then		
			SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
			SetEntityEmissiveStrength(e,emc[e].on_strength)		
			current_state[e] = "is-on"
		end
		rwait[e] = g_Time + (emc[e].fixed_interval*1000)
		status[e] = "endinit"
	end	
	
	-- Triggered Activation --
	if emc[e].mode == 1 and g_Entity[e]['activated'] == 1 then	
		if g_Entity[e]['activated'] == 1 and current_state[e] == "is-off" then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end		
			if current_level[e] < emc[e].on_strength then		
				SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
				SetEntityEmissiveStrength(e,current_level[e])
				current_level[e] = current_level[e] + emc[e].fade_speed
			end	
			if current_level[e] >= emc[e].on_strength then
				SetEntityEmissiveStrength(e,emc[e].on_strength)
				if emc[e].activate_logic == 2 or emc[e].activate_logic == 4 then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
				end
				current_state[e] = "is-on"
				played[e] = 0
				SetActivated(e,0)
			end	
		end	

		if g_Entity[e]['activated'] == 1 and current_state[e] == "is-on" then
			if current_level[e] > emc[e].off_strength then		
				SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
				SetEntityEmissiveStrength(e,current_level[e])
				current_level[e] = current_level[e] - emc[e].fade_speed
			end
			if current_level[e] <= emc[e].off_strength then
				SetEntityEmissiveStrength(e,emc[e].off_strength)
				if emc[e].activate_logic == 3 or emc[e].activate_logic == 4 then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
				end	
				current_state[e] = "is-off"
				SetActivated(e,0)
			end
		end	
	end	
	
	-- Triggered Fixed Interval+Count --
	if emc[e].mode == 2 and g_Entity[e]['activated'] == 1 then
		if g_Time > rwait[e] and count[e] < emc[e].fixed_count then
			if g_Entity[e]['activated'] == 1 and current_state[e] == "is-off" then
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				if current_level[e] < emc[e].on_strength then		
					SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
					SetEntityEmissiveStrength(e,current_level[e])
					current_level[e] = current_level[e] + emc[e].fade_speed
				end	
				if current_level[e] >= emc[e].on_strength then
					SetEntityEmissiveStrength(e,emc[e].on_strength)
					if emc[e].activate_logic == 2 or emc[e].activate_logic == 4 then
						PerformLogicConnections(e)
						ActivateIfUsed(e)
					end
					current_state[e] = "is-on"
					rwait[e] = g_Time + (emc[e].fixed_interval*1000)
					count[e] = count[e]+1
					played[e] = 0
				end
			end	
			if g_Entity[e]['activated'] == 1 and current_state[e] == "is-on" then
				if current_level[e] > emc[e].off_strength then		
					SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
					SetEntityEmissiveStrength(e,current_level[e])
					current_level[e] = current_level[e] - emc[e].fade_speed
				end
				if current_level[e] <= emc[e].off_strength then
					SetEntityEmissiveStrength(e,emc[e].off_strength)
					if emc[e].activate_logic == 3 or emc[e].activate_logic == 4 then
						PerformLogicConnections(e)
						ActivateIfUsed(e)
					end	
					current_state[e] = "is-off"
					rwait[e] = g_Time + (emc[e].fixed_interval*1000)
				end				
			end			
		end
		if count[e] >= emc[e].fixed_count then
			SetEntityEmissiveStrength(e,emc[e].off_strength)
			if current_state[e] == "is-on" then PerformLogicConnections(e) end			
			current_state[e] = "is-off"
			count[e] = 0
			SetActivated(e,0)
		end		
	end
	
	-- Use Day/Night Cycle --
	if emc[e].mode == 3 then
		if g_sunrollposition > -90 and g_sunrollposition < 85 then  --Day
			SetActivated(e,1)
			current_level[e] = emc[e].off_strength
			current_tod = "Day"
		end	
		if g_sunrollposition > 85 then  --Night
			SetActivated(e,1)
			current_tod = "Night"
		end	
		if g_Entity[e]['activated'] == 1 and current_state[e] == "is-off" or current_tod ==  "Night" then
			if current_level[e] < emc[e].on_strength then		
				SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
				SetEntityEmissiveStrength(e,current_level[e])
				current_level[e] = current_level[e] + emc[e].fade_speed
			end	
			if current_level[e] >= emc[e].on_strength then
				SetEntityEmissiveStrength(e,emc[e].on_strength)
				if emc[e].activate_logic == 2 or emc[e].activate_logic == 4 then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
				end
				current_state[e] = "is-on"
				SetActivated(e,0)
			end	
		end
		if g_Entity[e]['activated'] == 1 and current_state[e] == "is-on" or current_tod ==  "Day" then
			if current_level[e] > emc[e].off_strength then		
				SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
				SetEntityEmissiveStrength(e,current_level[e])
				current_level[e] = current_level[e] - emc[e].fade_speed
			end
			if current_level[e] <= emc[e].off_strength then
				SetEntityEmissiveStrength(e,emc[e].off_strength)
				if emc[e].activate_logic == 3 or emc[e].activate_logic == 4 then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
				end	
				current_state[e] = "is-off"
				SetActivated(e,0)
			end
		end
	end	
end