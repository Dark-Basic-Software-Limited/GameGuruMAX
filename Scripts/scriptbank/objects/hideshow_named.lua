-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- HideShow_Named v15
-- DESCRIPTION: Will Hide or Show a named object when this object is activated from a linked Trigger Zone or Switch
-- DESCRIPTION: Attach to any object set Always ON
-- DESCRIPTION: [OBJECT_NAME$=""] to Hide/Show
-- DESCRIPTION: [PROMPT_TEXT$="What was that"]
-- DESCRIPTION: [@MODE=1(1=Hide, 2=Show, 3=Fade-Hide, 4=Fade-Show, 5=Alternating Hide/Show)] the mode of action	
-- DESCRIPTION: [DELAY=0(0,30)] in seconds, fade fade_speed [#FADE_SPEED=0.02(0.00,10.00)]
-- DESCRIPTION: <Sound0> played when hiding
-- DESCRIPTION: <Sound1> played when showing

local lower = string.lower

local hsobject 		= {}
local prompt_text 	= {}
local mode			= {}
local delay			= {}
local status		= {}
local objEnt		= {}
local hs_time		= {}
local fade_speed	= {}
local current_level = {}
local played		= {}
local doonce		= {}
local toggle 		= {}
	
function hideshow_named_properties(e, object_name, prompt_text, mode, delay, fade_speed)
	hsobject[e].object_name = lower(object_name)
	hsobject[e].prompt_text = prompt_text
	hsobject[e].mode = mode	
	hsobject[e].delay = delay
	hsobject[e].fade_speed = fade_speed
end
 
function hideshow_named_init(e)
	hsobject[e] = {}
	hsobject[e].object_name = ""	
	hsobject[e].prompt_text = ""
	hsobject[e].mode = 1
	hsobject[e].delay = 1
	hsobject[e].fade_speed = 1
	status[e] = "init"
	hs_time[e] = 0
	played[e] = 0
	doonce[e] = 0
	objEnt[e] = 0
	toggle[e] = 0
end
 
function hideshow_named_main(e)
	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == hsobject[e].object_name then					
					objEnt[e] = n
					SetEntityAlwaysActive(n,1)
					current_level[e] = GetEntityBaseAlpha(n)	
					SetEntityTransparency(n,1)
					toggle[e] = GetEntityVisibility(n)
					Spawn(n)
				end
			end
		end		
		if hsobject[e].mode == 3 then current_level[e] = 100 end
		if hsobject[e].mode == 4 then current_level[e] = 0 end				
		
		if hsobject[e].mode == 1 or hsobject[e].mode == 3 then			
			Show(objEnt[e])
			CollisionOn(objEnt[e])
		end
		if hsobject[e].mode == 2 or hsobject[e].mode == 4 then
			Hide(objEnt[e])
			CollisionOff(objEnt[e])
		end
		--
		if hsobject[e].mode == 5 and toggle[e] == 0 then
			Hide(objEnt[e])
			CollisionOff(objEnt[e])
			toggle[e] = 0
		end
		if hsobject[e].mode == 5 and toggle[e] == 1 then
			Show(objEnt[e])
			CollisionOn(objEnt[e])
			toggle[e] = 1
		end
		--
		StartTimer(e)		
		status[e] = "endinit"
	end	
	if g_Entity[e]['activated'] == 0 then
		doonce[e] = 0
		played[e] = 0
	end	
		
	if g_Entity[e]['activated'] == 1 then		
		if hs_time[e] == 0 then
			hs_time[e] = GetTimer(e) + (hsobject[e].delay * 1000)			
		end
		
		if hsobject[e].mode == 1 then	--Hide--	
			if GetTimer(e) >= hs_time[e] then
				GravityOff(objEnt[e])
				CollisionOff(objEnt[e])
				Hide(objEnt[e])
				if doonce[e] == 0 then
					PromptDuration(hsobject[e].prompt_text,2000)
					doonce[e] = 1
				end
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				g_Entity[e]['activated'] = 0
			end
		end
		if hsobject[e].mode == 2 then	--Show-- 				
			if GetTimer(e) >= hs_time[e] then
				GravityOn(objEnt[e])
				CollisionOn(objEnt[e])
				Show(objEnt[e])
				if doonce[e] == 0 then
					PromptDuration(hsobject[e].prompt_text,2000)
					doonce[e] = 1
				end	
				if played[e] == 0 then
					PlaySound(e,1)
					played[e] = 1
				end				
				g_Entity[e]['activated'] = 0
			end
		end
		if hsobject[e].mode == 3 then	--Fade Hide--	
			if GetTimer(e) >= hs_time[e] then
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end			
				if current_level[e] > 0 then				
					SetEntityBaseAlpha(objEnt[e],current_level[e])
					current_level[e] = current_level[e] - hsobject[e].fade_speed
				end
				if current_level[e] <= 0 then
					current_level[e] = 0
					SetEntityBaseAlpha(objEnt[e],current_level[e])
					Hide(objEnt[e])
					GravityOff(objEnt[e])
					CollisionOff(objEnt[e])
					g_Entity[e]['activated'] = 0
				end
				if doonce[e] == 0 then
					PromptDuration(hsobject[e].prompt_text,2000)
					doonce[e] = 1
				end
			end
		end		
		if hsobject[e].mode == 4 then	--Fade Show--
			if GetTimer(e) >= hs_time[e] then
				Show(objEnt[e])
				if played[e] == 0 then
					PlaySound(e,1)
					played[e] = 1
				end				
				if current_level[e] < 100 then				
					SetEntityBaseAlpha(objEnt[e],current_level[e])
					current_level[e] = current_level[e] + hsobject[e].fade_speed					
				end
				if current_level[e] >= 100 then
					current_level[e] = 100
					SetEntityBaseAlpha(objEnt[e],current_level[e])
					CollisionOn(objEnt[e])
					GravityOn(objEnt[e])					
					g_Entity[e]['activated'] = 0
				end
				if doonce[e] == 0 then
					PromptDuration(hsobject[e].prompt_text,2000)
					doonce[e] = 1
				end
			end
		end	
		if hsobject[e].mode == 5 and toggle[e] == 1 then --If Shown then Hide--
			if GetTimer(e) >= hs_time[e] then
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				if doonce[e] == 0 then
					GravityOff(objEnt[e])
					CollisionOff(objEnt[e])
					Hide(objEnt[e])
					PromptDuration(hsobject[e].prompt_text,2000)
					toggle[e] = 0
					doonce[e] = 1
					SetActivated(e,0)
					g_Entity[e]['activated'] = 0					
				end
			end
		end
		if hsobject[e].mode == 5 and toggle[e] == 0 then --If Hidden then Show--
			if GetTimer(e) >= hs_time[e] then
				if doonce[e] == 0 then
					GravityOn(objEnt[e])
					CollisionOn(objEnt[e])
					Show(objEnt[e])
					PromptDuration(hsobject[e].prompt_text,2000)
					toggle[e] = 1					
					doonce[e] = 1
					SetActivated(e,0)					
					g_Entity[e]['activated'] = 0					
				end	
				if played[e] == 0 then
					PlaySound(e,1)
					played[e] = 1
				end
			end
		end			
	end
end 
 
function hideshow_named_exit(e) 
end