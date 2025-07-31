-- Quest Switch v7 by Necrym59
-- DESCRIPTION: This object will be treated as a quest activated switch object for activating other objects or game elements.
-- DESCRIPTION: [@QUEST_ACTIVATION=1(0=QuestList)]
-- DESCRIPTION: [USE_RANGE=90(1,200)]
-- DESCRIPTION: [ENABLED_TEXT$="E To Turn Switch ON"]
-- DESCRIPTION: [DISABLED_TEXT$="Disabled"]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)] Use emmisive color for shape option
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ON_ANIMATION$=1(0=AnimSetList)] Select ON animation (Default=ON)
-- DESCRIPTION: [@OFF_ANIMATION$=2(0=AnimSetList)] Select OFF animation (Default=OFF)
-- DESCRIPTION: [SWITCH_STATE!=0] switch state to decide if the switch is initially OFF or ON.
-- DESCRIPTION: [AFFECT_ENTITY$=""] Name of entity to affect
-- DESCRIPTION: [@AFFECT_MODE=0(0=None, 1=Hide, 2=Show, 3=Destroy)]
-- DESCRIPTION: <Sound0> when quest_switch activates
-- DESCRIPTION: <Sound1> when quest_switch deactivates

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_ActivateQuestComplete = {}

local lower = string.lower
local quest_switch 		= {}
local quest_activation	= {}
local use_range 		= {}
local enabled_text 		= {}
local disabled_text 	= {}
local item_highlight	= {}
local highlight_icon	= {}
local prompt_display 	= {}
local on_animation 		= {}
local off_animation 	= {}
local switch_state		= {}
local affect_entity		= {}
local affect_mode		= {}

local status 			= {}
local switched			= {}
local questtitle		= {}
local questtype			= {}
local tEnt 				= {}
local selectobj 		= {}
local doonce			= {}
local afobjectno		= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}

function quest_switch_properties(e, quest_activation, use_range, enabled_text, disabled_text, item_highlight, highlight_icon_imagefile, prompt_display, on_animation, off_animation, switch_state, affect_entity, affect_mode)
	quest_switch[e].quest_activation = quest_activation
	quest_switch[e].use_range = use_range
	quest_switch[e].enabled_text = enabled_text
	quest_switch[e].disabled_text = disabled_text	
	quest_switch[e].item_highlight = item_highlight or 0
	quest_switch[e].highlight_icon = highlight_icon_imagefile	
	quest_switch[e].prompt_display = prompt_display or 1
	quest_switch[e].on_animation = "=" .. tostring(on_animation)	
	quest_switch[e].off_animation = "=" .. tostring(off_animation)
	quest_switch[e].switch_state = switch_state or 0
	quest_switch[e].affect_entity = lower(affect_entity)
	quest_switch[e].affect_mode	= affect_mode or 0
end 

function quest_switch_init(e)
	quest_switch[e] = {}
	quest_switch[e].quest_activation = 0	
	quest_switch[e].use_range = 90	
	quest_switch[e].enabled_text = "To Turn quest_switch ON"
	quest_switch[e].disabled_text = "To Turn quest_switch OFF"
	quest_switch[e].item_highlight = 0
	quest_switch[e].highlight_icon = "imagebank\\icons\\hand.png"
	quest_switch[e].prompt_display = 1
	quest_switch[e].on_animation = ""
	quest_switch[e].off_animation = ""
	quest_switch[e].switch_state = 0
	quest_switch[e].affect_entity = ""
	quest_switch[e].affect_mode	= 0
	
	tEnt[e] = 0
	selectobj[e] = 0
	g_ActivateQuestComplete = 0
	switched[e] = 0
	questtitle[e] = ""
	questtype[e] = ""
	doonce[e] = 0
	afobjectno[e] = 0
	g_tEnt = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function quest_switch_main(e)
	if status[e] == "init" then
		if quest_switch[e].affect_entity > "" then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == quest_switch[e].affect_entity then
						afobjectno[e] = n
						if quest_switch[e].affect_mode == 2 then
							CollisionOff(n)
							Hide(n)
						end
						break
					end
				end
			end
		end	
		if quest_switch[e].switch_state == 0 then
			SetAnimationName(e,"off")
			PlayAnimation(e)
		end
		if quest_switch[e].switch_state == 1 then
			SetAnimationName(e,"on")
			PlayAnimation(e)
		end
		if quest_switch[e].item_highlight == 3 and quest_switch[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(quest_switch[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(quest_switch[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(quest_switch[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	questtitle[e] = GetCollectionQuestAttribute((quest_switch[e].quest_activation)-1,"title")
	questtype[e] = GetCollectionQuestAttribute((quest_switch[e].quest_activation)-1,"type")
	
	if PlayerDist < quest_switch[e].use_range and questtype[e] == "activate" and questtitle[e] ~= g_UserGlobalQuestTitleActive then
		if quest_switch[e].disabled_text ~= "" then
			if quest_switch[e].prompt_display == 1 then PromptLocal(e,quest_switch[e].disabled_text) end
			if quest_switch[e].prompt_display == 2 then Prompt(quest_switch[e].disabled_text) end	
		end
	end
	if PlayerDist < quest_switch[e].use_range and questtitle[e] == g_UserGlobalQuestTitleActive then
		--pinpoint select object--
		module_misclib.pinpoint(e,quest_switch[e].use_range,quest_switch[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end
	if PlayerDist < quest_switch[e].use_range and tEnt[e] == e then
		if g_Entity[e].activated == 0 and switched[e] == 0 then
			if quest_switch[e].enabled_text ~= "" then
				if quest_switch[e].prompt_display == 1 then PromptLocal(e,quest_switch[e].enabled_text) end
				if quest_switch[e].prompt_display == 2 then Prompt(quest_switch[e].enabled_text) end	
			end
			if g_KeyPressE == 1 and doonce[e] == 0 then	
				SetAnimationName(e,"on")
				PlayAnimation(e)
				PerformLogicConnections(e)
				ActivateIfUsed(e)				
				if afobjectno[e] ~= 0 then
					if quest_switch[e].affect_mode == 1 then
						CollisionOff(afobjectno[e])
						Hide(afobjectno[e])
					end
					if quest_switch[e].affect_mode == 2 then
						CollisionOn(afobjectno[e])
						Show(afobjectno[e])
					end
					if quest_switch[e].affect_mode == 3 then
						Destroy(afobjectno[e])
					end
				end
				g_ActivateQuestComplete = 1				
				switched[e] = 1
				doonce[e] = 1
				SwitchScript(e,"no_behavior_selected.lua")
			end
		end
	end
end