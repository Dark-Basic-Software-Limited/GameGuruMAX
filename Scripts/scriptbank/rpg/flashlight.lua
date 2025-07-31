-- Flashlight v34: by Necrym59
-- DESCRIPTION: Will give the player a Flashlight.
-- DESCRIPTION: [PICKUP_TEXT$="E to pickup"]
-- DESCRIPTION: [PICKUP_RANGE=100(1,200)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate, 3=Already Carrying)]
-- DESCRIPTION: [@USEAGE_STYLE=1(1=Normal, 2=Specific Weapon)]
-- DESCRIPTION: [@USEAGE_WEAPON$=-1(0=AnyWeaponList)] Specific Weapon Tool to use flashlight
-- DESCRIPTION: [USEAGE_TEXT$="F to use flashlight"]
-- DESCRIPTION: [FLASHLIGHT_RANGE=350(1,1000)]
-- DESCRIPTION: [FLASHLIGHT_RADIUS=40(1,80)]
-- DESCRIPTION: [FLASHLIGHT_R=255(0,255)]
-- DESCRIPTION: [FLASHLIGHT_G=255(0,255)]
-- DESCRIPTION: [FLASHLIGHT_B=255(0,255)]
-- DESCRIPTION: [!FLASHLIGHT_SHADOWS=1]
-- DESCRIPTION: [BATTERY_LEVEL=100(0,100)]
-- DESCRIPTION: [#BATTERY_DRAIN=0.01(0.00,5)]
-- DESCRIPTION: [#BATTERY_RECHARGE=0.01(0.00,1)]
-- DESCRIPTION: [@BATTERY_INDICATOR=1(1=Bottom, 2=Top, 3=None)]
-- DESCRIPTION: [INDICATOR_TEXT$="BATTERY LEVEL:"]
-- DESCRIPTION: [@LIGHT_ACTIVATION=1(1=Off, 2=On Pickup)]
-- DESCRIPTION: [@PICKUP_TRIGGER=1(1=Off, 2=On)]
-- DESCRIPTION: [@DEPLETION_TRIGGER=1(1=None, 2=ActivateIfUsed, 3=Lose Game)]
-- DESCRIPTION: [@UltravioletMode=0(0=Off, 1=On)]
-- DESCRIPTION: [@LightRangeKill=0(0=Off, 1=On)] will kill entity within light range and radius.
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg:MyBatteryEnergy
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for Pickup sound
-- DESCRIPTION: <Sound1> for switching on/off
-- DESCRIPTION: <Sound2> for battery low

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_batteryenergy = {}

local flashlight 			= {}
local pickup_text 			= {}
local pickup_range 			= {}
local pickup_style 			= {}
local useage_style 			= {}
local useage_weapon 		= {}
local useage_text 			= {}
local flashlight_range		= {}
local flashlight_radius		= {}
local flashlight_r			= {}
local flashlight_g			= {}
local flashlight_b			= {}
local flashlight_shadows	= {}
local battery_level	 		= {}
local battery_drain 		= {}
local battery_recharge 		= {}
local battery_indicator		= {}
local indicator_text		= {}
local light_activation		= {}
local pickup_trigger		= {}
local depletion_trigger		= {}
local ultravioletmode		= {}
local lightrangekill		= {}
local user_global_affected	= {}
local prompt_display 		= {}
local item_highlight 		= {}
local highlight_icon 		= {}

local hl_icon 			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
local have_flashlight 	= {}
local drain_level		= {}
local status 			= {}
local doonce			= {}
local played			= {}
local flashswitch		= {}
local currentvalue		= {}
local tEnt				= {}
local selectobj			= {}
local nearestEnt		= {}
local spottedEnt		= {}
local spottedDist		= {}
local spottedTran		= {}
local cleanuptime		= {}
local can_use			= {}

function flashlight_properties(e, pickup_text, pickup_range, pickup_style, useage_style, useage_weapon, useage_text, flashlight_range, flashlight_radius, flashlight_r, flashlight_g, flashlight_b, flashlight_shadows, battery_level, battery_drain, battery_recharge, battery_indicator, indicator_text, light_activation, pickup_trigger, depletion_trigger, ultravioletmode, lightrangekill, user_global_affected, prompt_display, item_highlight, highlight_icon_imagefile)
	flashlight[e].pickup_text = pickup_text
	flashlight[e].pickup_range = pickup_range
	flashlight[e].pickup_style = pickup_style
	flashlight[e].useage_style = useage_style
	flashlight[e].useage_weapon = tostring(GetWeaponName(useage_weapon-1))
	flashlight[e].useage_text = useage_text
	flashlight[e].flashlight_range = flashlight_range
	flashlight[e].flashlight_radius = flashlight_radius
	flashlight[e].flashlight_r = flashlight_r
	flashlight[e].flashlight_g = flashlight_g
	flashlight[e].flashlight_b = flashlight_b
	flashlight[e].flashlight_shadows = flashlight_shadows
	flashlight[e].battery_level = battery_level
	flashlight[e].battery_drain = battery_drain
	flashlight[e].battery_recharge = battery_recharge
	flashlight[e].battery_indicator = battery_indicator
	flashlight[e].indicator_text = indicator_text
	flashlight[e].light_activation = light_activation
	flashlight[e].pickup_trigger = pickup_trigger
	flashlight[e].depletion_trigger = depletion_trigger
	flashlight[e].ultravioletmode = ultravioletmode or 0
	flashlight[e].lightrangekill = lightrangekill or 0
	flashlight[e].user_global_affected = user_global_affected or ""
	flashlight[e].prompt_display = prompt_display
	flashlight[e].item_highlight = item_highlight
	flashlight[e].highlight_icon = highlight_icon_imagefile
end

function flashlight_init(e)
	flashlight[e] = {}
	flashlight[e].pickup_text = "E to pickup"
	flashlight[e].pickup_range = 100
	flashlight[e].pickup_style = 1
	flashlight[e].useage_style = 1
	flashlight[e].useage_weapon = ""	
	flashlight[e].useage_text = "F to use"
	flashlight[e].flashlight_range = 350
	flashlight[e].flashlight_radius = 40
	flashlight[e].flashlight_r = 255
	flashlight[e].flashlight_g = 255
	flashlight[e].flashlight_b = 255
	flashlight[e].flashlight_shadows = 1
	flashlight[e].battery_level = 100
	flashlight[e].battery_drain = 0.01
	flashlight[e].battery_recharge = 0.01
	flashlight[e].battery_indicator = 1
	flashlight[e].indicator_text = "BATTERY LEVEL:"
	flashlight[e].light_activation = 1
	flashlight[e].pickup_trigger = 1
	flashlight[e].depletion_trigger = 1
	flashlight[e].ultravioletmode =	0
	flashlight[e].lightrangekill = 0
	flashlight[e].user_global_affected = ""
	flashlight[e].prompt_display = 1
	flashlight[e].item_highlight = 0
	flashlight[e].highlight_icon = "imagebank\\icons\\pickup.png"

	have_flashlight[e] = 0
	drain_level[e] = 0
	doonce[e] = 0
	status[e] = 'init'
	tEnt[e] = 0
	g_tEnt = 0
	played[e] = 0
	currentvalue[e] = 0
	flashswitch[e] = 0
	cleanuptime[e] = 0
	selectobj[e] = 0
	SetFlashLightKeyEnabled(0)
	SetEntityAlwaysActive(e,1)
	g_batteryenergy = 0
	can_use[e] = 0
end

function flashlight_main(e)

	local PlayerDist = GetPlayerDistance(e)

	if status[e] == 'init' then
		if flashlight[e].item_highlight == 3 and flashlight[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(flashlight[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(flashlight[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(flashlight[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		SetGamePlayerStateFlashlightRange(flashlight[e].flashlight_range)
		SetGamePlayerStateFlashlightRadius(flashlight[e].flashlight_radius)
		SetGamePlayerStateFlashlightColorR(flashlight[e].flashlight_r)
		SetGamePlayerStateFlashlightColorG(flashlight[e].flashlight_g)
		SetGamePlayerStateFlashlightColorB(flashlight[e].flashlight_b)
		SetGamePlayerStateFlashlightCastShadow(flashlight[e].flashlight_shadows)
		if flashlight[e].battery_level > 100 then flashlight[e].battery_level = 100 end
		have_flashlight[e] = 0
		status[e] ='end'
	end
	
	if flashlight[e].useage_style == 1 then can_use[e] = 1 end
	if flashlight[e].useage_style == 2 then
		CurrentlyHeldWeaponID = GetPlayerWeaponID()	
		if CurrentlyHeldWeaponID ~= GetWeaponID(flashlight[e].useage_weapon) then 
			if status[e] == 'ON' then
				SetGamePlayerStateFlashlightControl(0.0)
				status[e] = 'OFF'
			end
			can_use[e] = 0
			SetFlashLightKeyEnabled(0)
		end
		if CurrentlyHeldWeaponID == GetWeaponID(flashlight[e].useage_weapon) then 
			can_use[e] = 1
		end		
	end
	
	if have_flashlight[e] == 0 then
		if flashlight[e].pickup_style == 1 and PlayerDist < flashlight[e].pickup_range then
			PromptDuration(flashlight[e].useage_text,2000)
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			Hide(e)
			CollisionOff(e)
			Hide(e)
			ResetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
			have_flashlight[e] = 1
			if flashlight[e].user_global_affected > "" then _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] = currentvalue[e] + flashlight[e].battery_level end
			if flashlight[e].pickup_trigger == 2 then
				SetActivatedWithMP(e,201)
				PerformLogicConnections(e)
			end
			tEnt[e] = 0
		end
		if flashlight[e].pickup_style == 2 and PlayerDist < flashlight[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,flashlight[e].pickup_range,flashlight[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < flashlight[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if flashlight[e].prompt_display == 1 then PromptLocal(e, flashlight[e].pickup_text) end
			if flashlight[e].prompt_display == 2 then Prompt(flashlight[e].pickup_text) end
			if g_KeyPressE == 1 then
				PromptDuration(flashlight[e].useage_text,2000)
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				Hide(e)
				CollisionOff(e)
				ResetPosition(e,0,-5000,0)
				have_flashlight[e] = 1
				if flashlight[e].user_global_affected > "" then _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] = currentvalue[e] + flashlight[e].battery_level end
				if flashlight[e].pickup_trigger == 2 then
					SetActivatedWithMP(e,201)
					PerformLogicConnections(e)
				end
			end
		end
		if flashlight[e].pickup_style == 3 then
			Hide(e)
			CollisionOff(e)
			ResetPosition(e,0,-5000,0)
			have_flashlight[e] = 1
			if flashlight[e].user_global_affected > "" then _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] = currentvalue[e] + flashlight[e].battery_level end
		end
	end

	if can_use[e] == 1 then
		if have_flashlight[e] == 1 then
			ResetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
			SetGamePlayerStatePlrKeyForceKeystate(0)
			if CG_GetActiveCamera == nil or CG_GetActiveCamera() == nil then  -- allows for Cine Guru interaction
				SetFlashLightKeyEnabled(1)
				if GetInKey() == "f" or GetInKey() == "F" then
					if (GetGamePlayerStateFlashlightControl() >= 0.89) then
						status[e] = 'ON'
						if flashswitch[e] == 0 then
							PlaySound(e,1)
							flashswitch[e] = 1
						end
					end
					if (GetGamePlayerStateFlashlightControl() <= 0.89) then
						status[e] = 'OFF'
						if flashswitch[e] == 1 then
							PlaySound(e,1)
							flashswitch[e] = 0
						end
					end
				end
				if flashlight[e].light_activation == 2 and doonce[e] == 0 then
					SetGamePlayerStatePlrKeyForceKeystate(33)
					status[e] = 'ON'
					doonce[e] = 1
				end
			elseif status[e] == 'ON' then
				SetFlashLightKeyEnabled(1)
				SetGamePlayerStatePlrKeyForceKeystate(33)
				status[e] = 'OFF'
			else
				SetFlashLightKeyEnabled(0)
			end
		end
		
		if status[e] == 'ON' then
			if flashlight[e].battery_level > 0 then
				flashlight[e].battery_level = flashlight[e].battery_level - (flashlight[e].battery_drain/10)
				if flashlight[e].user_global_affected > "" then _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] = flashlight[e].battery_level end
				if flashlight[e].battery_indicator == 1 then TextCenterOnXColor(50,95,3,flashlight[e].indicator_text.. " " ..math.floor(flashlight[e].battery_level).. "%",100,255,100) end
				if flashlight[e].battery_indicator == 2 then TextCenterOnXColor(50,5,3,flashlight[e].indicator_text.. " " ..math.floor(flashlight[e].battery_level).. "%",100,255,100) end
			end
			if flashlight[e].battery_level > 9 and flashlight[e].battery_level <= 10 then PlaySound(e,2) end
			if flashlight[e].battery_level > 1 and flashlight[e].battery_level < 2 then
				SetFlashLight(math.random(8))
				SetGamePlayerStateFlashlightRange(math.random(flashlight[e].flashlight_range-10,flashlight[e].flashlight_range))
				SetGamePlayerStateFlashlightRadius(math.random(flashlight[e].flashlight_radius-10,flashlight[e].flashlight_radius))
			end
			if flashlight[e].battery_level < 1 then
				SetGamePlayerStateFlashlightControl(0.0)
				if flashlight[e].depletion_trigger == 2 then ActivateIfUsed(e) end
				if flashlight[e].depletion_trigger == 3 then LoseGame() end
				status[e] = 'OFF'
			end
			------------------------------------------------------------------------------------------------------------------------
			if flashlight[e].ultravioletmode ==	1 then
				nearestEnt[e] = U.ClosestEntToPos(g_PlayerPosX, g_PlayerPosZ,flashlight[e].flashlight_range/1.5)
				local allegiance = GetEntityAllegiance(nearestEnt[e])
				if nearestEnt[e] ~= nil and nearestEnt[e] > 0 and g_Entity[nearestEnt[e]]['animating'] == 1 and allegiance == 0 then
					spottedEnt[e] = nearestEnt[e]
					spottedDist[e] = GetPlayerDistance(spottedEnt[e])
					spottedTran[e] = GetEntityTransparency(spottedEnt[e])
					if spottedTran[e] ~= 0 then SetEntityBaseColor(spottedEnt[e],155,0,155) end
					if spottedDist[e] < flashlight[e].flashlight_range/1.5 then
						if PlayerLooking(spottedEnt[e],spottedDist[e],80) == 1 then
							Show(spottedEnt[e])
							SetEntityBaseAlpha(spottedEnt[e],100-spottedDist[e])
						else
							SetEntityBaseAlpha(spottedEnt[e],0)
							Hide(spottedEnt[e])
						end
					end
					if spottedDist[e] > flashlight[e].flashlight_range/1.5 then
						SetEntityBaseAlpha(spottedEnt[e],0)
						SetEntityBaseColor(spottedEnt[e],255,255,255)
					end
				else
					spottedEnt[e] = 0
					nearestEnt[e] = 0
				end
			end
			------------------------------------------------------------------------------------------------------------------------
			if flashlight[e].lightrangekill == 1 then
				nearestEnt[e] = U.ClosestEntToPos(g_PlayerPosX, g_PlayerPosZ,(flashlight[e].flashlight_range/1.5)+GetGamePlayerStateFlashlightRadius())
				local allegiance = GetEntityAllegiance(nearestEnt[e])
				if nearestEnt[e] ~= nil and nearestEnt[e] > 0 and g_Entity[nearestEnt[e]]['animating'] == 1 and g_Entity[nearestEnt[e]]['health'] >= 0 and allegiance == 0 then
					spottedEnt[e] = nearestEnt[e]
					if PlayerLooking(spottedEnt[e],flashlight[e].flashlight_range/1.5,GetGamePlayerStateFlashlightRadius()/1.5) == 1 then
						SetEntityHealth(spottedEnt[e],0)
					end
				else
					spottedEnt[e] = 0
					nearestEnt[e] = 0
				end
			end
			------------------------------------------------------------------------------------------------------------------------
		end
		if status[e] == 'OFF' then
			flashlight[e].battery_level = flashlight[e].battery_level + (flashlight[e].battery_recharge/20)
			if flashlight[e].battery_level > 100 then flashlight[e].battery_level = 100 end
			if flashlight[e].user_global_affected > "" then
				_G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] = flashlight[e].battery_level
				if _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] > 100 then _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] = 100 end
			end
		end
		if g_batteryenergy > 0 then
			flashlight[e].battery_level = flashlight[e].battery_level + g_batteryenergy
			if flashlight[e].battery_level > 100 then flashlight[e].battery_level = 100 end
			if flashlight[e].user_global_affected > "" then
				if _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] = currentvalue[e] + flashlight[e].battery_level
				if _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] > 100 then _G["g_UserGlobal['"..flashlight[e].user_global_affected.."']"] = 100 end
			end
			g_batteryenergy = 0
		end
	end	
end

function PlayerLooking(e,dis,v)
	if g_Entity[e] ~= nil then
		if dis == nil then
			dis = 3000
		end
		if v == nil then
			v = 0.5
		end
		if GetPlayerDistance(e) <= dis then
			local destx = g_Entity[e]['x'] - g_PlayerPosX
			local destz = g_Entity[e]['z'] - g_PlayerPosZ
			local angle = math.atan2(destx,destz)
			angle = angle * (180.0 / math.pi)
			if angle <= 0 then
				angle = 360 + angle
			elseif angle > 360 then
				angle = angle - 360
			end
			while g_PlayerAngY < 0 or g_PlayerAngY > 360 do
				if g_PlayerAngY <= 0 then
					g_PlayerAngY = 360 + g_PlayerAngY
				elseif g_PlayerAngY > 360 then
					g_PlayerAngY = g_PlayerAngY - 360
				end
			end
			local L = angle - v
			local R = angle + v
			if L <= 0 then
				L = 360 + L
			elseif L > 360 then
				L = L - 360
			end
			if R <= 0 then
				R = 360 + R
			elseif R > 360 then
				R = R - 360
			end
			if (L < R and math.abs(g_PlayerAngY) > L and math.abs(g_PlayerAngY) < R) then
				return 1
			elseif (L > R and (math.abs(g_PlayerAngY) > L or math.abs(g_PlayerAngY) < R)) then
				return 1
			else
				return 0
			end
		else
			return 0
		end
	end
end