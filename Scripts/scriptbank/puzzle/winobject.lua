-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- WinObject v6: by Necrym59
-- DESCRIPTION: Will win the level by obtaining this object? Set Always Active ON.
-- DESCRIPTION: [RANGE=90(1,200)]
-- DESCRIPTION: [PROMPT$="E to use"]
-- DESCRIPTION: [@END_MODE=1(1=Instant, 2=Video)] End level modes
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after object use.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Sound0> for use
-- DESCRIPTION: <Video Slot> for optional ending video

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local winobject 		= {}
local range 			= {}
local prompt 			= {}
local end_mode			= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon 	= {}
local resetstates		= {}

local doonce			= {}
local tEnt 				= {}
local status			= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
local enddelay			= {}
local ended				= {}
local endvideo			= {}
	
function winobject_properties(e, range, prompt, end_mode, prompt_display, item_highlight, highlight_icon_imagefile, resetstates)
	winobject[e].range = range
	winobject[e].prompt = prompt
	winobject[e].end_mode = end_mode
	winobject[e].prompt_display = prompt_display
	winobject[e].item_highlight = item_highlight or 0
	winobject[e].highlight_icon = highlight_icon_imagefile
	winobject[e].resetstates = resetstates or 0	
end 	
	
function winobject_init(e)
	winobject[e] = {}	
	winobject[e].range = 90	
	winobject[e].prompt = "E to use"
	winobject[e].end_mode = 1	
	winobject[e].prompt_display = 1
	winobject[e].item_highlight = 0
	winobject[e].highlight_icon = "imagebank\\icons\\hand.png"	
	winobject[e].resetstates = 0	
	
	doonce[e] = 0
	g_tEnt = 0
	tEnt[e] = 0
	enddelay[e] = math.huge
	ended[e] = 0
	endvideo[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	status[e] = "init"
end
 
function winobject_main(e)

	if status[e] == "init" then
		if winobject[e].item_highlight == 3 and winobject[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(winobject[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(winobject[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(winobject[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end		

	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < winobject[e].range then
		--pinpoint select object--
		module_misclib.pinpoint(e, winobject[e].range, winobject[e].item_highlight, hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if PlayerDist < winobject[e].range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if winobject[e].prompt_display == 1 then PromptLocal(e,winobject[e].prompt) end
			if winobject[e].prompt_display == 2 then Prompt(winobject[e].prompt) end			
			if g_KeyPressE == 1 then					
				if doonce[e] == 0 then 
					PlaySound(e,0)
					doonce[e] = 1						
				end
				Hide(e)
				CollisionOff(e)
				ended[e] = 1
				enddelay[e] = g_Time + 1000
			end
		end
	end

	if g_Time > enddelay[e] and ended[e] == 1 then	
		if winobject[e].end_mode == 1 then
			JumpToLevelIfUsedEx(e,winobject[e].resetstates)
			Destroy(e)
		end
		
		if winobject[e].end_mode == 2 then
			if endvideo[e] == 0 then
				PromptVideoNoSkip(e,1)
				endvideo[e] = 1
			end
			if endvideo[e] == 1 then
				JumpToLevelIfUsedEx(e,winobject[e].resetstates)					
			end
		end
	end	
end

function winitem_init(e)
end

