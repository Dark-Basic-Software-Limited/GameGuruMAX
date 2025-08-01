-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Binoculars v15 by Necrym59
-- DESCRIPTION: The Binocular object will give the player Binoculars?
-- DESCRIPTION: Set the [PICKUP_TEXT$="E to Pickup"] and [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: Set the [USEAGE_TEXT$="Hold B to use"]
-- DESCRIPTION: Set the [#MIN_ZOOM=-10(-20,1)], [MAX_ZOOM=20(1,30)], [ZOOM_SPEED=1(1,5)]
-- DESCRIPTION: Set the binocular [IMAGEFILE$="imagebank\\misc\\testimages\\binocs.png"]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local binoculars 		= {}
local pickup_text 		= {}
local pickup_range 		= {}
local useage_text 		= {}
local min_zoom			= {}
local max_zoom			= {}
local zoom_speed 		= {}
local screen_image		= {}
local item_highlight 	= {}
local highlight_icon 	= {}

local binocularssp 	= {}
local start_wheel 	= {}
local mod 			= {}
local last_gun		= {}
local gunstatus		= {}
local tEnt			= {}
local selectobj		= {}
local hl_icon 		= {}
local hl_imgwidth 	= {}
local hl_imgheight 	= {}
local status		= {}
	
function binoculars_properties(e, pickup_text, pickup_range, useage_text, min_zoom, max_zoom, zoom_speed, screen_image, item_highlight, highlight_icon_imagefile)
	binoculars[e].pickup_text = pickup_text
	binoculars[e].pickup_range = pickup_range
	binoculars[e].useage_text = useage_text
	binoculars[e].min_zoom = min_zoom	
	binoculars[e].max_zoom = max_zoom
	binoculars[e].zoom_speed = zoom_speed
	binoculars[e].screen_image = imagefile or screen_image
	binoculars[e].item_highlight = item_highlight
	binoculars[e].highlight_icon = highlight_icon_imagefile	
end 
	
	
function binoculars_init(e)
	binoculars[e] = {}
	binoculars[e].pickup_text = "E to Pickup"
	binoculars[e].pickup_range = 80
	binoculars[e].useage_text = "Hold B to use"
	binoculars[e].min_zoom = -20
	binoculars[e].max_zoom = 60
	binoculars[e].zoom_speed = 1
	binoculars[e].screen_image ="imagebank\\misc\\testimages\\binocs.png"
	binoculars[e].item_highlight = 0
	binoculars[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	have_binoculars = 0
	start_wheel = 0 
	mod = g_PlayerFOV
	fov = g_PlayerFOV
	last_gun = g_PlayerGunName
	gunstatus = 0
	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	status[e] = "init"
end
 
function binoculars_main(e)

	if status[e] == "init" then
		if binoculars[e].item_highlight == 3 and binoculars[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(binoculars[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(binoculars[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(binoculars[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		binocularssp = CreateSprite(LoadImage(binoculars[e].screen_image))
		SetSpriteSize(binocularssp,100,100)
		SetSpritePosition(binocularssp,200,200)
		fov = g_PlayerFOV
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	if fov == nil then fov = g_PlayerFOV end 	
	
	if have_binoculars == 0 then
		if PlayerDist < binoculars[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,binoculars[e].pickup_range,binoculars[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < binoculars[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			PromptLocal(e,binoculars[e].pickup_text)
			if g_KeyPressE == 1 then
				have_binoculars = 1					
				PromptDuration(binoculars[e].useage_text,3000)
				PlaySound(e,0)
				Hide(e)
				CollisionOff(e)
				ActivateIfUsed(e)
			end
		end
	end
		
	if have_binoculars == 1 then
		SetPosition(e,g_PlayerPosX,g_PlayerPosY+250,g_PlayerPosZ)		
		PromptLocal(e,"")
		if g_Scancode == 48 then --Hold B Key to use
			
			if g_PlayerGunID > 0 then
				SetPlayerWeapons(0)
				gunstatus = 1
			end							
			PasteSpritePosition(binocularssp,0,0)
			if g_MouseWheel < 0 then 
				mod = mod - binoculars[e].zoom_speed		
			elseif g_MouseWheel > 0 then 
				mod = mod + binoculars[e].zoom_speed	
			end	
			if mod < binoculars[e].min_zoom then 
				mod = binoculars[e].min_zoom
			elseif mod > binoculars[e].max_zoom then 
				mod = binoculars[e].max_zoom
			end
			SetPlayerFOV(fov-mod)
			Prompt("Magnification Factor: " ..math.ceil(mod))
		else
			if gunstatus == 1 then
				ChangePlayerWeapon(last_gun)
				SetPlayerWeapons(1)
				SetPlayerFOV(fov)
				gunstatus = 0
			end
			start_wheel = g_MouseWheel			
			if mod > 0 then SetPlayerFOV(fov) end
			if mod < 0 then	SetPlayerFOV(fov) end			
		end
		if g_Scancode == 0 then
			mod = 0
			SetPlayerFOV(fov)	
		end
	end
end