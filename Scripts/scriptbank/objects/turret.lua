-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Turret v8 by Necrym59
-- DESCRIPTION: Allows the use of gun-turret mode using a designated weapon.
-- DESCRIPTION: Attach to an object used as the gun turret placeholder.
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [PROMPT_TEXT$="E to Use, Q to Release"]
-- DESCRIPTION: [HORIZONTAL_VIEW_LIMIT=45(1,180)] the players horizontal view angle range limit
-- DESCRIPTION: [VERTICAL_VIEW_LIMIT=45(1,90)] the players vertical view angle range limit
-- DESCRIPTION: [@WEAPON_NAME$=-1(0=AnyWeaponList)] The weapon to use
-- DESCRIPTION: [WEAPON_AMMO=500(0,1000)] Weapons Ammo
-- DESCRIPTION: [USE_CROSSHAIR!=0]
-- DESCRIPTION: [CROSSHAIR_IMAGEFILE$="imagebank\\crosshairs\\crosshair.dds"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> - Turret attached sound
-- DESCRIPTION: <Sound1> - Turret released sound

local module_misclib = require "scriptbank\\module_misclib"
g_tEnt = {}

local turret				= {}
local prompt_text			= {}
local horizontal_view_limit = {}
local vertical_view_limit 	= {}
local weapon_name			= {}
local weapon_ammo			= {}
local use_crosshair			= {}
local crosshair				= {}
local prompt_display		= {}
local item_highlight		= {}
local highlight_icon		= {}

local hl_icon 		= {}
local hl_imgwidth	= {}
local hl_imgheight	= {}
local doonce		= {}
local sp_crosshair	= {}
local sp_imgwidth	= {}
local sp_imgheight	= {}
local status		= {}
local tEnt 			= {}
local selectobj 	= {}
local freezeangy	= {}
local last_gun		= {}
local ammoleft		= {}

function turret_properties(e, use_range, prompt_text, horizontal_view_limit, vertical_view_limit, weapon_name, weapon_ammo, use_crosshair, crosshair_imagefile, prompt_display, item_highlight, highlight_icon_imagefile)
	turret[e].use_range = use_range
	turret[e].prompt_text = prompt_text
	turret[e].horizontal_view_limit = horizontal_view_limit
	turret[e].vertical_view_limit = vertical_view_limit
	turret[e].weapon_name = tostring(GetWeaponName(weapon_name-1))
	turret[e].weapon_ammo = weapon_ammo
	turret[e].use_crosshair = use_crosshair or 0	
	turret[e].crosshair = crosshair_imagefile
	turret[e].prompt_display = prompt_display
	turret[e].item_highlight = item_highlight
	turret[e].highlight_icon = highlight_icon_imagefile
	
end

function turret_init(e)
	turret[e] = {}
	turret[e].use_range = 80
	turret[e].prompt_text = "E to Use, Q to Release"
	turret[e].prompt_display = 1
	turret[e].horizontal_view_limit = 45
	turret[e].vertical_view_limit = 45
	turret[e].weapon_name = ""
	turret[e].weapon_ammo = 500
	turret[e].use_crosshair = 0
	turret[e].crosshair = ""
	turret[e].prompt_display = 1
	turret[e].item_highlight = 0
	turret[e].highlight_icon = "imagebank\\icons\\hand.png"

	doonce[e] = 0
	status[e] = "init"
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	freezeangy[e] = 0
	ammoleft[e] = -1
end

function turret_main(e)
	local PlayerDist = GetPlayerDistance(e)
	if status[e] == "init" then
		if turret[e].item_highlight == 3 and turret[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(turret[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(turret[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(turret[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		if turret[e].use_crosshair == 1 then 
			if turret[e].crosshair > "" then
				sp_crosshair[e] = CreateSprite(LoadImage(turret[e].crosshair))
				sp_imgwidth[e] = GetImageWidth(LoadImage(turret[e].crosshair))
				sp_imgheight[e] = GetImageHeight(LoadImage(turret[e].crosshair))
				SetSpriteSize(sp_crosshair[e],-1,-1)
				SetSpriteDepth(sp_crosshair[e],100)
				SetSpritePosition(sp_crosshair[e],500,500)
				SetSpriteOffset(sp_crosshair[e],sp_imgwidth[e]/2.0, sp_imgheight[e]/2.0)
				SetSpriteColor(sp_crosshair[e],255,255,255,200)			
			end
		end				
		SetActivated(e,1)
		status[e] = "unattached"
	end

	if g_Entity[e]['activated'] == 1 then		
		if status[e] == "unattached" then
			if PlayerDist < turret[e].use_range then
				--pinpoint select object--
				module_misclib.pinpoint(e,turret[e].use_range,turret[e].item_highlight,hl_icon[e])
				tEnt[e] = g_tEnt
				--end pinpoint select object--
				if PlayerDist < turret[e].use_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
					if turret[e]. prompt_display == 1 then TextCenterOnX(50,54,1,turret[e].prompt_text) end
					if turret[e]. prompt_display == 2 then Prompt(turret[e].prompt_text) end
					if g_KeyPressE == 1 then						
						if doonce[e] == 0 then
							Hide(e)
							PlaySound(e,0)							
							PerformLogicConnections(e)
							freezeangy[e] = g_PlayerAngY
							SetGamePlayerControlFinalCameraAngley(freezeangy[e])
							last_gun[e] = g_PlayerGunName							
						
							--Add Temporary Turret Weapon and Ammo to Player -----------------------------
							AddPlayerWeapon(turret[e].weapon_name)
							if ammoleft[e] > -1 then turret[e].weapon_ammo = ammoleft[e] end							
							--ActivateIfUsed(turret[e].weapon_name)
							local WeaponID = GetWeaponID(turret[e].weapon_name)
							SetWeaponSlot(9,WeaponID,WeaponID)
							ChangePlayerWeapon(turret[e].weapon_name)
							for index = 1, 10, 1 do								
								GetWeaponSlot(index)
								SetWeaponAmmo(index,turret[e].weapon_ammo)
								SetGamePlayerStateFiringMode(2)
							end							
							------------------------------------------------------------------------------
							if turret[e].use_crosshair == 1 then
								SetSpritePosition(sp_crosshair[e],50,50)
								SetGamePlayerStateRightMouseHold(2)
								SetFreezePosition(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
								TransportToFreezePositionOnly()								
							end		
							status[e] = "attached"
							doonce[e] = 1
						end						
					end
				end
			end
		end
		if status[e] == "attached" then
			if turret[e].use_crosshair == 1 then SetGamePlayerStateRightMouseHold(2) end
			SetFreezePosition(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
			TransportToFreezePositionOnly()
			-- Set Horizontal View Limit
			if GetGamePlayerControlFinalCameraAngley()>turret[e].horizontal_view_limit+freezeangy[e] then SetGamePlayerControlFinalCameraAngley(turret[e].horizontal_view_limit+freezeangy[e]) end
			if GetGamePlayerControlFinalCameraAngley()<-turret[e].horizontal_view_limit+freezeangy[e] then SetGamePlayerControlFinalCameraAngley(-turret[e].horizontal_view_limit+freezeangy[e]) end
			--Set Vertical View Limit
			if (GetGamePlayerStateCamAngleX()<-turret[e].vertical_view_limit) then SetGamePlayerStateCamAngleX(-turret[e].vertical_view_limit) end
			if (GetGamePlayerStateCamAngleX()>turret[e].vertical_view_limit) then SetGamePlayerStateCamAngleX(turret[e].vertical_view_limit) end
			if g_KeyPressQ == 1 then
				PlaySound(e,1)
				if turret[e].use_crosshair == 1 then SetSpritePosition(sp_crosshair[e],500,500) end				
				SetGamePlayerStateRightMouseHold(0)
				ammoleft[e] = GetWeaponAmmo(9)
				RemovePlayerWeapon(9)
				ChangePlayerWeapon(last_gun[e])				
				Show(e)
				doonce[e] = 0
				status[e] = "unattached"
			end			
		end
	end
end

function turret_exit(e)
end
