-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Code Switch v8 By Necrym59
-- DESCRIPTION: This object will be treated as a code switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$ = "E to enter code"]
-- DESCRIPTION: [USE_RANGE=70(0,100)]
-- DESCRIPTION: [SWITCH_CODE=123(001,999)]
-- DESCRIPTION: [READOUT_X=50(1,100)]
-- DESCRIPTION: [READOUT_Y=15(1,100)]
-- DESCRIPTION: [USEAGE_TEXT$ = "Q-Select, Z-Change or enter Numbers, X-Submit"]
-- DESCRIPTION: [INCORRECT_TEXT$="Code is Incorrect"]
-- DESCRIPTION: [CORRECT_TEXT$="Access Granted"]
-- DESCRIPTION: [@SWITCH_STATUS=1(1=Locked, 2=Unlocked)]
-- DESCRIPTION: [@FAIL_ALARM=1(1=None, 2=Three Attempts)]
-- DESCRIPTION: [ALARM_RANGE=1000(0,3000)]
-- DESCRIPTION: [ALARM_RESET=10(0,30)] Seconds
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: Play the audio <Sound0> when entering the codes.
-- DESCRIPTION: Play the audio <Sound1> when the code is entered correctly.
-- DESCRIPTION: Play the audio <Sound2> when the code is entered incorrect.
-- DESCRIPTION: Play the audio <Sound3> for the alarm sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

g_codeswitch 			= {}
local prompt_text 		= {}
local use_range			= {}
local switch_code		= {}
local useage_text		= {}
local incorrect_text	= {}
local correct_text		= {}
local entered_code		= {}
local switch_status		= {}
local fail_alarm		= {}
local alarm_range		= {}
local alarm_reset		= {}
local prompt_display	= {}
local item_highlight	= {}
local highlight_icon	= {}

local codebar = {}
local codeinput_1 = {}
local codeinput_2 = {}
local codeinput_3 = {}
local readout_x = {}
local readout_y = {}
local readout_spacing = {}
local codepad = {}
local switchsprite = {}
local incorrectcount = {}
local wait = {}
local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}
local tEnt = {}
local played = {}
local keypause = {}
local keypressed = {}

function code_switch_properties(e, prompt_text, use_range, switch_code, readout_x, readout_y, useage_text, incorrect_text, correct_text, switch_status, fail_alarm, alarm_range, alarm_reset, prompt_display, item_highlight, highlight_icon_imagefile)
	g_codeswitch[e]['prompt_text'] = prompt_text
	g_codeswitch[e]['use_range'] = use_range
	g_codeswitch[e]['switch_code'] = switch_code
	g_codeswitch[e]['readout_x'] = readout_x
	g_codeswitch[e]['readout_y'] = readout_y
	g_codeswitch[e]['useage_text'] = useage_text	
	g_codeswitch[e]['incorrect_text'] = incorrect_text
	g_codeswitch[e]['correct_text'] = correct_text
	g_codeswitch[e]['switch_status'] = switch_status
	g_codeswitch[e]['fail_alarm'] = fail_alarm
	g_codeswitch[e]['alarm_range'] = alarm_range
	g_codeswitch[e]['alarm_reset'] = alarm_reset
	g_codeswitch[e]['prompt_display']  = prompt_display
	g_codeswitch[e]['item_highlight']  = item_highlight
	g_codeswitch[e]['highlight_icon']  = highlight_icon_imagefile	
end 

function code_switch_init(e)
	g_codeswitch[e] = {}
	g_codeswitch[e]['prompt_text'] = "to enter code"
	g_codeswitch[e]['use_range'] = 70
	g_codeswitch[e]['switch_code'] = 001
	g_codeswitch[e]['readout_x'] = 0
	g_codeswitch[e]['readout_y'] = 0
	g_codeswitch[e]['useage_text'] = "Q-Select, Z-Change or enter Numbers, X-Submit"
	g_codeswitch[e]['incorrect_text'] = "Code is Incorrect"
	g_codeswitch[e]['correct_text'] = "Access Granted"
	g_codeswitch[e]['switch_status'] = 1
	g_codeswitch[e]['fail_alarm'] = 1
	g_codeswitch[e]['alarm_range'] = 1000
	g_codeswitch[e]['alarm_reset'] = 15
	g_codeswitch[e]['prompt_display']  = 1
	g_codeswitch[e]['item_highlight']  = 0
	g_codeswitch[e]['highlight_icon']  = "imagebank\\icons\\hand.png"
	
	readout_spacing[e] = 0
	entered_code[e] = 0
	codebar[e] = 1
	codeinput_1[e] = 0
	codeinput_2[e] = 0
	codeinput_3[e] = 0
	incorrectcount[e] = 0
	keypause[e] = 0
	keypressed[e] = 0
	wait[e] = math.huge
	played[e] = 0
	codepad[e] = "inactive"
	status[e] = "init"
	tEnt[e] = 0
	g_tEnt = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function code_switch_main(e)
	
	if status[e] == "init" then 
		if g_codeswitch[e]['item_highlight'] == 3 and g_codeswitch[e]['highlight_icon'] ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(g_codeswitch[e]['highlight_icon']))
			hl_imgwidth[e] = GetImageWidth(LoadImage(g_codeswitch[e]['highlight_icon']))
			hl_imgheight[e] = GetImageHeight(LoadImage(g_codeswitch[e]['highlight_icon']))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		if GetDeviceWidth() < 2560 then readout_spacing[e] = 3 end
		if GetDeviceWidth() >= 2560 then readout_spacing[e] = 2 end
		if GetDeviceWidth() >= 3840 then readout_spacing[e] = 1 end
		status[e]= "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < g_codeswitch[e]['use_range'] then
		if PlayerDist < g_codeswitch[e]['use_range'] and codepad[e] ~= "active" then
		--pinpoint select object--
		module_misclib.pinpoint(e,g_codeswitch[e]['use_range'],g_codeswitch[e]['item_highlight'],hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		end
		
		if PlayerDist < g_codeswitch[e]['use_range'] and tEnt[e] == e and GetEntityVisibility(e) == 1 then		
			if g_codeswitch[e]['switch_status'] == 1 and codepad[e] == "inactive" then  --Locked
				if g_codeswitch[e]['prompt_display'] == 1 then PromptLocal(e,g_codeswitch[e]['prompt_text']) end
				if g_codeswitch[e]['prompt_display'] == 2 then Prompt(g_codeswitch[e]['prompt_text'],3) end					
				if g_KeyPressE == 1 then 
					codepad[e] = "active"
					PlaySound(e,0)
				end
			end
		end
		
		if codepad[e] == "active" then
			local key = GetKeyPressed(e,g_InKey)
			if key == "q" and codebar[e] > 0 then		
				PlaySound(e,0)					
				played[e] = 1
				codebar[e] = codebar[e] + 1	
				if codebar[e] > 3 then codebar[e] = 1 end				
			end			
			if codebar[e] == 1 then 
				if key == "1" or key == "2" or key == "3" or key == "4" or key == "1" or key == "5" or key == "6" or key == "7" or key == "8" or key == "9" or key == "0" then codeinput_1[e] = key	end
			end
			if codebar[e] == 2 then 
				if key == "1" or key == "2" or key == "3" or key == "4" or key == "1" or key == "5" or key == "6" or key == "7" or key == "8" or key == "9" or key == "0" then codeinput_2[e] = key end
			end	
			if codebar[e] == 3 then 
				if key == "1" or key == "2" or key == "3" or key == "4" or key == "1" or key == "5" or key == "6" or key == "7" or key == "8" or key == "9" or key == "0" then codeinput_3[e] = key end
			end				
			if key == "z" and codebar[e] == 1 then
				PlaySound(e,0)
				codeinput_1[e] = codeinput_1[e] + 1				
				if codeinput_1[e] > 9 then codeinput_1[e] = 0 end
			end
			if key == "z" and codebar[e] == 2 then
				PlaySound(e,0)
				codeinput_2[e] = codeinput_2[e] + 1
				if codeinput_2[e] > 9 then codeinput_2[e] = 0 end	
			end
			if key == "z" and codebar[e] == 3 then
				PlaySound(e,0)
				codeinput_3[e] = codeinput_3[e] + 1
				if codeinput_3[e] > 9 then codeinput_3[e] = 0 end
			end			
			TextCenterOnXColor(g_codeswitch[e]['readout_x'],g_codeswitch[e]['readout_y']-2,3,g_codeswitch[e]['useage_text'],255,255,255) --white
			if codebar[e] == 1 then
				TextCenterOnXColor(g_codeswitch[e]['readout_x']-readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_1[e],0,255,0)
				TextCenterOnXColor(g_codeswitch[e]['readout_x'],g_codeswitch[e]['readout_y'],5,codeinput_2[e],255,255,255)
				TextCenterOnXColor(g_codeswitch[e]['readout_x']+readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_3[e],255,255,255)
			end
			if codebar[e] == 2 then
				TextCenterOnXColor(g_codeswitch[e]['readout_x']-readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_1[e],255,255,255)
				TextCenterOnXColor(g_codeswitch[e]['readout_x'],g_codeswitch[e]['readout_y'],5,codeinput_2[e],0,255,0)
				TextCenterOnXColor(g_codeswitch[e]['readout_x']+readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_3[e],255,255,255)
			end
			if codebar[e] == 3 then
				TextCenterOnXColor(g_codeswitch[e]['readout_x']-readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_1[e],255,255,255)
				TextCenterOnXColor(g_codeswitch[e]['readout_x'],g_codeswitch[e]['readout_y'],5,codeinput_2[e],255,255,255)
				TextCenterOnXColor(g_codeswitch[e]['readout_x']+readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_3[e],0,255,0)
			end
			entered_code[e] =(""..codeinput_1[e]..""..codeinput_2[e]..""..codeinput_3[e])
			if key == "x" then				
				if entered_code[e]-g_codeswitch[e]['switch_code'] == 0 then
					codepad[e] = "unlocked"
					SetActivatedWithMP(e,201)
					if g_codeswitch[e]['prompt_display'] == 1 then PromptLocal(e,g_codeswitch[e]['correct_text']) end
					if g_codeswitch[e]['prompt_display'] == 2 then Prompt(g_codeswitch[e]['correct_text']) end	
					StopSound(e,3)
					PlaySound(e,1)
					PerformLogicConnections(e)
					ActivateIfUsed(e)					
					SetAnimationName(e,"on")
					PlayAnimation(e)
					codeinput_1[e] = 0
					codeinput_2[e] = 0
					codeinput_3[e] = 0
					keypressed[e] = 1
					keypause[e] = g_Time + 2000
				else
					if g_codeswitch[e]['prompt_display'] == 1 then PromptLocal(e,g_codeswitch[e]['incorrect_text']) end
					if g_codeswitch[e]['prompt_display'] == 2 then Prompt(g_codeswitch[e]['incorrect_text']) end
					PlaySound(e,2)
					incorrectcount[e] = incorrectcount[e] + 1
					if g_codeswitch[e]['fail_alarm'] == 2 and incorrectcount[e] == 3 then
						wait[e] = g_Time + (g_codeswitch[e]['alarm_reset']*1000)
						codepad[e] = "alarm"
					end
				end				
			end
			played[e] = 0		
		end
		
		if g_codeswitch[e]['switch_status'] == 2 or codepad[e] == "unlocked" and keypressed[e] == 0 then --Unlocked
			if g_codeswitch[e]['prompt_display'] == 1 then PromptLocal(e,g_codeswitch[e]['correct_text']) end
			if g_codeswitch[e]['prompt_display'] == 2 then Prompt(g_codeswitch[e]['correct_text']) end
			if g_KeyPressE == 1 then
				SetActivatedWithMP(e,201)			
				PlaySound(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)
				PerformLogicConnections(e)
				keypressed[e] = 1
				keypause[e] = g_Time + 2000
			end	
		end		
	end
	
	if codepad[e] == "alarm" then
		LoopSound(e,3)
		MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_codeswitch[e]['alarm_range'],1,e)	
		if g_Time > wait[e] then
			StopSound(e,3)
			incorrectcount[e] = 0
			codepad[e] = "active"
		end	
	end	
	if g_Time > keypause[e] then
		keypressed[e] = 0
	end	
end

function GetKeyPressed(e, key, ignorecase)
    key = key or ""
    ignorecase = ignorecase or false 
    lastpressed = lastpressed or {}
    e = e or 0
    lastpressed[e] = lastpressed[e] or {}
    local inkey = g_InKey
    if ignorecase then
        key = string.lower(key)
        inkey = string.lower(g_InKey)
    end
    local waspressed
    if inkey == key and lastpressed[e] ~= g_InKey then
        waspressed = g_InKey 
    else
        waspressed = "false"
    end
    lastpressed[e] = g_InKey
    return waspressed
end
