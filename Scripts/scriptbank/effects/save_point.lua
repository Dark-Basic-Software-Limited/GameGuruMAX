-- Save Point v12 by Necrym59
-- DESCRIPTION: Saves a save point
-- DESCRIPTION: Attach to an object, switch or zone.
-- DESCRIPTION: [@ACTIVATION=1(1=Ranged, 2=Zone/Switch)]
-- DESCRIPTION: [USE_RANGE=80(1,200)]
-- DESCRIPTION: [PROMPT_MESSAGE$="Press E to save"]
-- DESCRIPTION: [SAVE_MESSAGE$="Saving.."]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\disk.png"]
-- DESCRIPTION: [@ICON_ON_SAVE=1(1=On,2=Off)]
-- DESCRIPTION: <Sound0> when activated

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local savepoint 		= {}
local activation 		= {}
local use_range 		= {}
local save_message 		= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon 	= {}
local icon_on_save		= {}

local played			= {}
local doonce			= {}
local wait				= {}
local animonce			= {}
local tEnt 				= {}
local selectobj 		= {}
local status			= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}

function save_point_properties(e, activation, use_range, prompt_message, save_message, prompt_display, item_highlight, highlight_icon_imagefile, icon_on_save)
	savepoint[e].activation = activation
	savepoint[e].use_range = use_range
	savepoint[e].prompt_message = prompt_message
	savepoint[e].save_message = save_message
	savepoint[e].prompt_display = prompt_display
	savepoint[e].item_highlight = item_highlight
	savepoint[e].highlight_icon = highlight_icon_imagefile	
	savepoint[e].icon_on_save = 1
end

function save_point_init(e)
	savepoint[e] = {}
	savepoint[e].activation = 1
	savepoint[e].use_range = 80
	savepoint[e].prompt_message = "Press E to save"
	savepoint[e].save_message = "Saving.."
	savepoint[e].prompt_display = 1
	savepoint[e].item_highlight = 0
	savepoint[e].highlight_icon = "imagebank\\icons\\disk.png"
	savepoint[e].icon_on_save = 1	
	
	played[e] = 0
	doonce[e] = 0
	animonce[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	wait[e] = math.huge
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function save_point_main(e)

	if status[e] == "init" then
		if savepoint[e].item_highlight == 3 and savepoint[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(savepoint[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(savepoint[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(savepoint[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end			
		status[e] = "activate"
	end

	if status[e] == "activate" then		
		if savepoint[e].activation == 1 then
			local PlayerDist = GetPlayerDistance(e)
			if PlayerDist < savepoint[e].use_range then
				--pinpoint select object--
				module_misclib.pinpoint(e,savepoint[e].use_range,savepoint[e].item_highlight,hl_icon[e])
				tEnt[e] = g_tEnt
				--end pinpoint select object--
			end
			if PlayerDist < savepoint[e].use_range and tEnt[e] == e then
				if savepoint[e].prompt_display == 1 then TextCenterOnX(50,53,3,savepoint[e].prompt_message) end
				if savepoint[e].prompt_display == 2 then Prompt(savepoint[e].prompt_message) end
				if g_KeyPressE == 1 then
					status[e] = "save"
					SetActivated(e,1)
				end
			end
		end
		if savepoint[e].activation == 2 then
			if g_Entity[e].plrinzone == 1 and g_PlayerPosY < g_Entity[e]['y']+100 then
				status[e] = "save"
				SetActivated(e,1)
			end
		end
	end
	
	if g_Entity[e]['activated'] == 1 then			
		if status[e] == "save" then
			if savepoint[e].icon_on_save == 1 then PasteSpritePosition(hl_icon[e],50,50) end
			if savepoint[e].activation == 1 and savepoint[e].prompt_display == 1 then TextCenterOnX(50,53,3,savepoint[e].save_message) end
			if savepoint[e].activation == 2 and savepoint[e].prompt_display == 1 then TextCenterOnX(50,53,3,savepoint[e].save_message) end
			if doonce[e] == 0 then
				if played[e] == 0 then
					if savepoint[e].activation == 1 then
						if savepoint[e].prompt_display == 2 then PromptDuration(savepoint[e].save_message,3000) end
						SetAnimationName(e,"on")
						PlayAnimation(e)
						PlaySound(e,0)
					end
					if savepoint[e].activation == 2 then
						if savepoint[e].prompt_display == 2 then PromptDuration(savepoint[e].save_message,3000) end
						PlaySound(e,0)						
					end
					played[e] = 1
				end
				-- set game slot and name and save --
				strNewSlotName = "Save Point: " .. os.date()
				gamedata = require "titlesbank\\gamedata"
				gamedata.save(8,strNewSlotName)
				-- and also save state of all player attributes (health, weapon, containers, userglobals)
				gamedata.mode(2)
				gamedata.save("0-globals","playerstate")
				gamedata.mode(0)
				---------------------------------------
				ResumeGame()
				PerformLogicConnections(e)
				doonce[e] = 1				
				wait[e] = g_Time + 3000
				animonce[e] = 1
			end
		end
	end
	
	if g_Time > wait[e] then 
		wait[e] = math.huge
		SetActivated(e,0)
	end	
	
	if g_Entity[e]['activated'] == 0 then
		if g_Time > wait[e] then
			doonce[e] = 0
			played[e] = 0
			if animonce[e] == 1 then
				SetAnimationName(e,"off")
				PlayAnimation(e)
				animonce[e] = 0
				PerformLogicConnections(e)
			end
			if savepoint[e].activation == 2 then
				Destroy(e)
			end
			PasteSpritePosition(hl_icon[e],500,500)
			status[e] = "activate"
		end
	end
end