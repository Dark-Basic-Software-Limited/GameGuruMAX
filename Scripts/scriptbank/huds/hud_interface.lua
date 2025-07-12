-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hud_Interface v11 by Necrym59 and Lee
-- DESCRIPTION: Will this will enable to use a hud screen as an interface for the attached object.
-- DESCRIPTION: Ensure your Hud Button names match the ones you put in here and are set to "return id to lua".
-- DESCRIPTION: Optionally reduce use range to 1 to allow HUD control anytime the specified HUD is visible.
-- DESCRIPTION: If used by a zone, the specified HUD will show when the player enters the zone.
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [USE_PROMPT$="E to Use"]
-- DESCRIPTION: [@@HUD_SCREEN$=""(0=hudscreenlist)] Eg: HUD Screen 9
-- DESCRIPTION: [HUD_BUTTON1$="Activate Link"]
-- DESCRIPTION: [@HUD_ACTION1=0(0=Link 0,1=Link 1,2=Link 2,3=Link 3,4=Link 4,5=Link 5,6=Link 6,7=Link 7,8=Link 8,9=Link 9,10=Play Audio,11=Destroy Object,12=Activate IfUsed,13=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION1_TEXT$="Link Activated"]
-- DESCRIPTION: [HUD_BUTTON2$="Play Audio"]
-- DESCRIPTION: [@HUD_ACTION2=10(0=Link 0,1=Link 1,2=Link 2,3=Link 3,4=Link 4,5=Link 5,6=Link 6,7=Link 7,8=Link 8,9=Link 9,10=Play Audio,11=Destroy Object,12=Activate IfUsed,13=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION2_TEXT$="Playing Audio"]
-- DESCRIPTION: [HUD_BUTTON3$="Destroy"]
-- DESCRIPTION: [@HUD_ACTION3=11(0=Link 0,1=Link 1,2=Link 2,3=Link 3,4=Link 4,5=Link 5,6=Link 6,7=Link 7,8=Link 8,9=Link 9,10=Play Audio,11=Destroy Object,12=Activate IfUsed,13=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION3_TEXT$="Object Destroyed"]
-- DESCRIPTION: [HUD_BUTTON4$="Activate IfUsed"]
-- DESCRIPTION: [@HUD_ACTION4=12(0=Link 0,1=Link 1,2=Link 2,3=Link 3,4=Link 4,5=Link 5,6=Link 6,7=Link 7,8=Link 8,9=Link 9,10=Play Audio,11=Destroy Object,12=Activate IfUsed,13=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION4_TEXT$="Activating"]
-- DESCRIPTION: [HUD_BUTTON5$="Exit"]
-- DESCRIPTION: [@HUD_ACTION5=13(0=Link 0,1=Link 1,2=Link 2,3=Link 3,4=Link 4,5=Link 5,6=Link 6,7=Link 7,8=Link 8,9=Link 9,10=Play Audio,11=Destroy Object,12=Activate IfUsed,13=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION5_TEXT$="Exiting"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0>  Interface activation
-- DESCRIPTION: <Sound1>  Audiofile sound

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local hud_interface 	= {}
local use_range 		= {}
local use_prompt 		= {}
local hud_screen 		= {}
local hud_button1 		= {}
local hud_action1 		= {}
local hud_action1_text 	= {}
local hud_button2 		= {}
local hud_action2 		= {}
local hud_action2_text 	= {}
local hud_button3 		= {}
local hud_action3 		= {}
local hud_action3_text 	= {}
local hud_button4 		= {}
local hud_action4 		= {}
local hud_action4_text 	= {}
local hud_button5 		= {}
local hud_action5 		= {}
local hud_action5_text 	= {}
local prompt_display	= {}
local item_highlight	= {}
local highlight_icon	= {}

local status			= {}
local tEnt				= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
local pressed			= {}
local actioned			= {}
local current_link		= {}
local playonce			= {}

function hud_interface_properties(e, use_range, use_prompt, hud_screen, hud_button1, hud_action1, hud_action1_text, hud_button2, hud_action2, hud_action2_text, hud_button3, hud_action3, hud_action3_text, hud_button4, hud_action4, hud_action4_text, hud_button5, hud_action5, hud_action5_text, prompt_display, item_highlight, highlight_icon_imagefile)
	hud_interface[e].use_range = use_range
	hud_interface[e].use_prompt = use_prompt
	hud_interface[e].hud_screen = hud_screen
	hud_interface[e].hud_button1 = hud_button1
	hud_interface[e].hud_action1 = hud_action1
	hud_interface[e].hud_action1_text = hud_action1_text
	hud_interface[e].hud_button2 = hud_button2
	hud_interface[e].hud_action2 = hud_action2
	hud_interface[e].hud_action2_text = hud_action2_text
	hud_interface[e].hud_button3 = hud_button3
	hud_interface[e].hud_action3 = hud_action3
	hud_interface[e].hud_action3_text = hud_action3_text
	hud_interface[e].hud_button4 = hud_button4
	hud_interface[e].hud_action4 = hud_action4
	hud_interface[e].hud_action4_text = hud_action4_text
	hud_interface[e].hud_button5 = hud_button5
	hud_interface[e].hud_action5 = hud_action5
	hud_interface[e].hud_action5_text = hud_action5_text
	hud_interface[e].prompt_display = prompt_display
	hud_interface[e].item_highlight = item_highlight
	hud_interface[e].highlight_icon = highlight_icon_imagefile
end

function hud_interface_init(e)
	hud_interface[e] = {}
	hud_interface[e].use_range = 80
	hud_interface[e].use_prompt = "E to use"
	hud_interface[e].hud_screen = "HUD Screen 9"
	hud_interface[e].hud_button1 = "Activate Links"
	hud_interface[e].hud_action1 = 1
	hud_interface[e].hud_action1_text = "Link Activated"
	hud_interface[e].hud_button2 = "Play"
	hud_interface[e].hud_action2 = 2
	hud_interface[e].hud_action2_text = "Playing Audio"
	hud_interface[e].hud_button3 = "Destroy"
	hud_interface[e].hud_action3 = 3
	hud_interface[e].hud_action3_text = "Object Destroyed"
	hud_interface[e].hud_button4 = "Activate IfUsed"
	hud_interface[e].hud_action4 = 4
	hud_interface[e].hud_action4_text = "Activate IfUsed"
	hud_interface[e].hud_button5 = "Exit"
	hud_interface[e].hud_action5 = 5
	hud_interface[e].hud_action5_text = "Exiting"
	hud_interface[e].prompt_display = 1
	hud_interface[e].item_highlight = 0
	hud_interface[e].highlight_icon = "imagebank\\icons\\hand.png"

	status[e] = "init"
	tEnt[e] = 0
	g_tEnt = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	pressed[e] = 0
	actioned[e] = 0
	current_link[e] = 0
	playonce[e] = 0
end

function hud_interface_main(e)

	if status[e] == "init" then
		if hud_interface[e].item_highlight == 3 and hud_interface[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(hud_interface[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(hud_interface[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(hud_interface[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		pressed[e] = 0
		playonce[e] = 0
		actioned[e] = 0
		current_link[e] = 0
		status[e] = "start"
	end

	if status[e] == "start" then
		-- zone behaviour
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+100 then
			-- OPEN HUD
			ScreenToggle(hud_interface[e].hud_screen)
			status[e] = "interface"
		end
		if hud_interface[e].use_range == 1 then
			-- responds instead whenever the HUD screen is visible (i.e opened by a hotkey)
			if GetCurrentScreen() > 0 then
				if GetCurrentScreenName() == hud_interface[e].hud_screen then
					status[e] = "interface"
				end
			end
		else
			local PlayerDist = GetPlayerDistance(e)
			if PlayerDist < hud_interface[e].use_range then
				--pinpoint select object--
				module_misclib.pinpoint(e,hud_interface[e].use_range,hud_interface[e].item_highlight,hl_icon[e])
				tEnt[e] = g_tEnt
				--end pinpoint select object--
			end	
			if PlayerDist < hud_interface[e].use_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
				if hud_interface[e].prompt_display == 1 then PromptLocal(e,hud_interface[e].use_prompt) end
				if hud_interface[e].prompt_display == 2 then Prompt(hud_interface[e].use_prompt) end	
				if g_KeyPressE == 1 and pressed[e] == 0 then
					if playonce[e] == 0 then
						PlaySound(e,0)
						playonce[e] = 1
					end
					pressed[e] = 1
					PromptLocal(e,"")
					-- OPEN HUD
					ScreenToggle(hud_interface[e].hud_screen)
					status[e] = "interface"
				end
			end
		end
	end

	if status[e] == "interface" then
		current_link[e] = 0
		local buttonElementID = DisplayCurrentScreen()
		if buttonElementID >= 0 then
			local buttonElementName = GetScreenElementName(1+buttonElementID)
			if string.len(buttonElementName) > 0 then
				if buttonElementName == hud_interface[e].hud_button1 then
					if hud_interface[e].prompt_display == 1 then PromptLocal(e,hud_interface[e].hud_action1_text) end
					if hud_interface[e].prompt_display == 2 then Prompt(hud_interface[e].hud_action1_text) end
					if hud_interface[e].hud_action1 < 10 then
						current_link[e] = hud_interface[e].hud_action1
						actioned[e] = 1
					end
					if hud_interface[e].hud_action1 == 10 then actioned[e] = 2 end
					if hud_interface[e].hud_action1 == 11 then actioned[e] = 3 end
					if hud_interface[e].hud_action1 == 12 then actioned[e] = 4 end
					if hud_interface[e].hud_action1 == 13 then actioned[e] = 5 end
					status[e] = "action"
				end
				if buttonElementName == hud_interface[e].hud_button2 then
					if hud_interface[e].prompt_display == 1 then PromptLocal(e,hud_interface[e].hud_action2_text) end
					if hud_interface[e].prompt_display == 2 then Prompt(hud_interface[e].hud_action2_text) end
					if hud_interface[e].hud_action1 < 10 then
						current_link[e] = hud_interface[e].hud_action2
						actioned[e] = 1
					end
					if hud_interface[e].hud_action2 == 10 then actioned[e] = 2 end
					if hud_interface[e].hud_action2 == 11 then actioned[e] = 3 end
					if hud_interface[e].hud_action2 == 12 then actioned[e] = 4 end
					if hud_interface[e].hud_action2 == 13 then actioned[e] = 5 end
					status[e] = "action"
				end
				if buttonElementName == hud_interface[e].hud_button3 then
					if hud_interface[e].prompt_display == 1 then PromptLocal(e,hud_interface[e].hud_action3_text) end
					if hud_interface[e].prompt_display == 2 then Prompt(hud_interface[e].hud_action3_text) end
					if hud_interface[e].hud_action3 < 10 then
						current_link[e] = hud_interface[e].hud_action3
						actioned[e] = 1
					end
					if hud_interface[e].hud_action3 == 10 then actioned[e] = 2 end
					if hud_interface[e].hud_action3 == 11 then actioned[e] = 3 end
					if hud_interface[e].hud_action3 == 12 then actioned[e] = 4 end
					if hud_interface[e].hud_action3 == 13 then actioned[e] = 5 end
					status[e] = "action"
				end
				if buttonElementName == hud_interface[e].hud_button4 then
					if hud_interface[e].prompt_display == 1 then PromptLocal(e,hud_interface[e].hud_action4_text) end
					if hud_interface[e].prompt_display == 2 then Prompt(hud_interface[e].hud_action4_text) end
					if hud_interface[e].hud_action4 < 10 then
						current_link[e] = hud_interface[e].hud_action4
						actioned[e] = 1
					end
					if hud_interface[e].hud_action4 == 10 then actioned[e] = 2 end
					if hud_interface[e].hud_action4 == 11 then actioned[e] = 3 end
					if hud_interface[e].hud_action4 == 12 then actioned[e] = 4 end
					if hud_interface[e].hud_action4 == 13 then actioned[e] = 5 end
					status[e] = "action"
				end
				if buttonElementName == hud_interface[e].hud_button5 then
					if hud_interface[e].prompt_display == 1 then PromptLocal(e,hud_interface[e].hud_action5_text) end
					if hud_interface[e].prompt_display == 2 then Prompt(hud_interface[e].hud_action5_text) end
					if hud_interface[e].hud_action5 < 10 then
						current_link[e] = hud_interface[e].hud_action5
						actioned[e] = 1
					end
					if hud_interface[e].hud_action5 == 10 then actioned[e] = 2 end
					if hud_interface[e].hud_action5 == 11 then actioned[e] = 3 end
					if hud_interface[e].hud_action5 == 12 then actioned[e] = 4 end
					if hud_interface[e].hud_action5 == 13 then actioned[e] = 5 end
					status[e] = "action"
				end
			end
		end
	end

	if status[e] == "action" then
		if actioned[e] == 1 then
			PerformLogicConnectionNumber(e,current_link[e])
			actioned[e] = 0
			status[e] = "interface"
		end
		if actioned[e] == 2 then
			PlaySound(e,1)
			actioned[e] = 0
			status[e] = "interface"
		end
		if actioned[e] == 3 then
			ScreenToggle("")
			Hide(e)
			CollisionOff(e)
			Destroy(e)
			actioned[e] = 0
			status[e] = "interface"
		end
		if actioned[e] == 4 then
			ActivateIfUsed(e)
			actioned[e] = 0
			status[e] = "interface"
		end
		if actioned[e] == 5 then
			-- CLOSE HUD
			ScreenToggle("")
			status[e] = "init"
		end
	end
end