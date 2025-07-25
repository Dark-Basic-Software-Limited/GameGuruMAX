-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Switch_Count v10 by Necrym59
-- DESCRIPTION: Will create a switch series to ActivateIfUsed when completed.
-- DESCRIPTION: Activate all [SWITCHES=4] to complete.
-- DESCRIPTION: Switch [@SWITCH_STATE=1(1=On, 2=Off)].
-- DESCRIPTION: Set [USE_RANGE=60 (1,100)] distance
-- DESCRIPTION: Set [USE_PROMPT$="E to use"]
-- DESCRIPTION: Count [MESSAGE_DURATION=2 (1,5)] seconds.
-- DESCRIPTION: Select [@COMPLETION=1(1=End Level, 2=Activate if Used)] controls whether to end level or activate if used object.
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: Play <Sound0> when switch activated
-- DESCRIPTION: Play <Sound1> when completed

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_SwitchesActivated = 0

local switch_count 		= {}
local switches			= {}
local state				= {}
local use_range			= {}
local use_prompt		= {}
local message_duration	= {}
local completion		= {}
local prompt_display	= {}
local item_highlight	= {}
local highlight_icon	= {}

local status 				= {}
local hl_icon 				= {}
local hl_imgwidth			= {}
local hl_imgheight			= {}
local switch_count_active 	= {}
local doonce 				= {}
local tEnt 					= {}
local selectobj 			= {}
local item_highlight		= {}

function switch_count_properties(e, switches, state, use_range, use_prompt, message_duration, completion, prompt_display, item_highlight, highlight_icon_imagefile)
	switch_count[e].switches = switches
	switch_count[e].state = state
	switch_count[e].use_range = use_range
	switch_count[e].use_prompt = use_prompt
	switch_count[e].message_duration = message_duration
	switch_count[e].completion = completion
	switch_count[e].prompt_display = prompt_display
	switch_count[e].item_highlight = item_highlight
	switch_count[e].highlight_icon = highlight_icon_imagefile	
end 

function switch_count_init(e)
	switch_count[e] = {}
	switch_count[e].switches = 4
	switch_count[e].state = 1
	switch_count[e].use_range = 70
	switch_count[e].use_prompt = "E to use"
	switch_count[e].message_duration = 1
	switch_count[e].completion = 1
	switch_count[e].prompt_display = 1
	switch_count[e].item_highlight = 0
	switch_count[e].highlight_icon = "imagebank\\icons\\hand.png"
	
	switch_count_active[e] = 1
	doonce[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	status[e] = "init"	
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0		
	if switch_count[e].state == 1 then
		SetAnimationName(e,"off")
		PlayAnimation(e)
		StopAnimation(e)
		switch_count[e].state = 2
	else
		SetAnimationName(e,"on")
		PlayAnimation(e)
		StopAnimation(e)
		switch_count[e].state = 1
	end
end
	 
function switch_count_main(e)
	if status[e] == "init" then
		if switch_count[e].item_highlight == 3 and switch_count[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(switch_count[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(switch_count[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(switch_count[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end
	
	if switch_count_active[e] == nil then
		switch_count_active[e] = 1
	end
		
	if switch_count_active[e] == 1 then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist <= switch_count[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,switch_count[e].use_range,switch_count[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--			
		end	
		if PlayerDist <= switch_count[e].use_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if switch_count[e].prompt_display == 1 then PromptLocal(e,switch_count[e].use_prompt) end
			if switch_count[e].prompt_display == 2 then Prompt(switch_count[e].use_prompt) end
			if g_KeyPressE == 1 then
				g_SwitchesActivated = g_SwitchesActivated + 1				
				switch_count_active[e] = 0
				if doonce[e] == 0 then 
					SetActivated(e,1)
					if switch_count[e].state == 1 then SetAnimationName(e,"on") end
					if switch_count[e].state == 2 then SetAnimationName(e,"off") end
					PlayAnimation(e)					
					StopAnimation(e)
					PlaySound(e,0)
					PerformLogicConnections(e)
					doonce[e] = 1
				end
				PromptDuration(g_SwitchesActivated.."/"..switch_count[e].switches	.." complete", switch_count[e].message_duration*1000)
			end			
		end
		if g_SwitchesActivated >= switch_count[e].switches then
			PlaySound(e,1)
			if switch_count[e].completion == 1 then JumpToLevelIfUsed(e) end
			if switch_count[e].completion == 2 then ActivateIfUsed(e) end
			g_SwitchesActivated = 0
		end
	end
end
