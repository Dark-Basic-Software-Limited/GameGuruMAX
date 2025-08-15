-- Chest v9
-- DESCRIPTION: When player is within [USE_RANGE=100], show
-- DESCRIPTION: [USE_PROMPT$="Press E to open"] when use key is pressed,
-- DESCRIPTION: will display [@@CHEST_HUD_SCREEN$=""(0=hudscreenlist)] Eg: HUD Screen 6],
-- DESCRIPTION: using [CHEST_CONTAINER$="chestunique"].
-- DESCRIPTION: [CHEST_UNLOCKED!=1] If set off is locked and requires key 
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> for opening chest.
-- DESCRIPTION: <Sound1> for closing chest.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local chest = {}
local use_range = {}
local use_prompt = {}
local chest_hud_screen = {}
local chest_container = {}
local chest_unlocked = {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}

local status = {}
local doonce = {}
local tEnt = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}
local selectobj = {}

function chest_properties(e, use_range, use_prompt, chest_hud_screen, chest_container, chest_unlocked, prompt_display, item_highlight, highlight_icon_imagefile)
	chest[e].use_range = use_range
	chest[e].use_prompt = use_prompt
	chest[e].chest_hud_screen = chest_hud_screen
	chest[e].chest_container = chest_container
	chest[e].chest_unlocked = chest_unlocked or 1
	chest[e].prompt_display = prompt_display or 1
	chest[e].item_highlight = item_highlight
	chest[e].highlight_icon = highlight_icon_imagefile	
end

function chest_init(e)
	chest[e] = {}
	chest[e].use_range = 100
	chest[e].use_prompt = "Press E to open"
	chest[e].chest_hud_screen = "HUD Screen 6"
	chest[e].chest_container = "chestunique"
	chest[e].chest_unlocked = 1	
	chest[e].prompt_display = 1
	chest[e].item_highlight = 0	
	chest[e].highlight_icon = "imagebank\\icons\\hand.png"	
	
	status[e] = "init"
	selectobj[e] = 0
	g_tEnt = 0
	tEnt[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function chest_main(e)

	if status[e] == "init" then
		if chest[e].item_highlight == 3 and chest[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(chest[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(chest[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(chest[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "closed"
	end

	if chest[e].chest_unlocked == 1 then
		chest[e].chest_unlocked = 0
		SetEntityHasKey(e,1)
		g_Entity[e]['haskey'] = 1
	end
	
	if GetCurrentScreen() == -1 and status[e] == "closed" then
		-- in the game
		PlayerDist = GetPlayerDistance(e)
		if PlayerDist < chest[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,chest[e].use_range,chest[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < chest[e].use_range and tEnt[e] == e then
			if chest[e].prompt_display == 1 then PromptLocal(e,chest[e].use_prompt) end
			if chest[e].prompt_display == 2 then Prompt(chest[e].use_prompt) end
			if g_KeyPressE == 1 and chest[e].chest_unlocked == 0 then
				PromptLocal(e,"Locked - Need a key")
			end
			if g_KeyPressE == 1 and g_Entity[e]['haskey'] == 1 then
				SetAnimationName(e,"open")
				PlayAnimation(e)
				PlaySound(e,0)
				if chest[e].chest_container == "chestunique" then
					g_UserGlobalContainer = "chestuniquetolevelase"..tostring(e)
				else
					g_UserGlobalContainer = chest[e].chest_container
				end
				ScreenToggle(chest[e].chest_hud_screen)
				status[e] = "opened"				
			end
		end
	else
		-- in chest screen
	end
	if GetCurrentScreen() == -1 and status[e] == "opened" then
		SetAnimationName(e,"close")
		PlayAnimation(e)
		PlaySound(e,1)
		PerformLogicConnections(e)
		ActivateIfUsed(e)		
		status[e] = "closed"
	end		
end
