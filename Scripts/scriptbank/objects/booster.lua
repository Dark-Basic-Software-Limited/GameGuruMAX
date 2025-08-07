-- Booster v3 by Necrym 59
-- DESCRIPTION: The object will give the player a booster or deduction if used.
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Boost consumed"]
-- DESCRIPTION: [QUANTITY=10(1,500)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [@BOOST_STYLE=1(1=Health Applied, 2=Stamina Timed, 3=Jumping Timed, 4=Speed Timed, 5=User Global Timed)]
-- DESCRIPTION: [BOOST_TIME=5(0,60)] Seconds
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyMana
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for useage sound.
-- DESCRIPTION: <Sound1> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local booster = {}
local prompt_text = {}
local prompt_if_collectable = {}
local useage_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local effect = {}
local boost_style = {}
local boost_time = {}
local user_global_affected = {}
local prompt_display = {}
local item_highlight = {}
local use_item_now = {}
local item_used = {}
local calcstamina = {}
local calchealth = {}
local calcjump = {}
local calcspeed = {}
local calcglobal = {}
local defaultglobal = {}
local defaultjump = {}
local defaultspeed = {}
local currentvalue = {}
local tEnt = {}
local selectobj = {}
local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}
local booster_timer ={}

function booster_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect, boost_style, boost_time, user_global_affected, prompt_display, item_highlight, highlight_icon_imagefile)
	booster[e].prompt_text = prompt_text
	booster[e].prompt_if_collectable = prompt_if_collectable
	booster[e].useage_text = useage_text
	booster[e].quantity = quantity
	booster[e].pickup_range = pickup_range
	booster[e].pickup_style = pickup_style
	booster[e].effect = effect
	booster[e].boost_style = boost_style
	booster[e].boost_time = boost_time
	booster[e].user_global_affected = user_global_affected
	booster[e].prompt_display = prompt_display
	booster[e].item_highlight = item_highlight
	booster[e].highlight_icon = highlight_icon_imagefile
end

function booster_init(e)
	booster[e] = {}
	booster[e].prompt_text = "E to Use"
	booster[e].prompt_if_collectable = "E to collect"
	booster[e].useage_text = "booster consumed"
	booster[e].quantity = 10
	booster[e].pickup_range = 80
	booster[e].pickup_style = 1
	booster[e].effect = 1
	booster[e].boost_style = 1
	booster[e].boost_time = 1
	booster[e].user_global_affected = "MyMana"
	booster[e].prompt_display = 1
	booster[e].item_highlight = 0
	booster[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	use_item_now[e] = 0
	item_used[e] = 0
	calchealth[e] = 0
	calcstamina[e] = 0
	calcjump[e] = 0
	calcspeed[e] = 0
	calcglobal[e] = 0
	defaultglobal[e] = 0
	defaultjump[e] = SetGamePlayerControlJumpmax
	defaultspeed[e] = GetGamePlayerControlSpeedRatio()
	currentvalue[e] = 0		
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	booster_timer[e] = math.huge
end

function booster_main(e)

	if status[e] == "init" then
		if booster[e].item_highlight == 3 and booster[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(booster[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(booster[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(booster[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		booster_timer[e] = math.huge
		SetGamePlayerControlJumpmax(defaultjump[e])
		status[e] = "endinit"
	end
	
	PlayerDist = GetPlayerDistance(e)
	if booster[e].pickup_style == 1 then
		if PlayerDist < booster[e].pickup_range then
			use_item_now[e] = 1
		end
	end
	if booster[e].pickup_style == 2 and PlayerDist < booster[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,booster[e].pickup_range,booster[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if PlayerDist < booster[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(tEnt[e]) == 0 then
				if booster[e].prompt_display == 1 then PromptLocal(e,booster[e].prompt_text) end
				if booster[e].prompt_display == 2 then Prompt(booster[e].prompt_text) end
				if g_KeyPressE == 1 then
					use_item_now[e] = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if booster[e].prompt_display == 1 then PromptLocal(e,booster[e].prompt_if_collectable) end
				if booster[e].prompt_display == 2 then Prompt(booster[e].prompt_if_collectable) end
				-- if collectable or resource
				if g_KeyPressE == 1 then
					Hide(e)
					CollisionOff(e)
					SetEntityCollected(tEnt[e],1)
					PlaySound(e,1)
				end
			end
		end
	end
	local tusedvalue = GetEntityUsed(e)
	if tusedvalue > 0 then
		-- if this is a resource, it will deplete qty and set used to zero
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1		
	end	
	local addquantity = 0
	if use_item_now[e] == 1 then		
		if booster[e].effect == 1 then addquantity = 1 end
		if booster[e].effect == 2 then addquantity = 2 end
		if item_used[e] == 0 then
			PromptDuration(booster[e].useage_text,3000)
			PlaySound(e,0)
			PerformLogicConnections(e)
			SetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
			booster_timer[e] = g_Time + (booster[e].boost_time*1000)
			CollisionOff(e)
			Hide(e)
			item_used[e] = 1
			-- Set boosts calculations
			if addquantity == 1 then
				if booster[e].boost_style == 1 then
					calchealth[e] = g_PlayerHealth + booster[e].quantity
					if calchealth[e] > g_PlayerStartStrength then calchealth[e] = g_PlayerStartStrength end
				end
				if booster[e].boost_style == 2 then	
					local user_defined_global_current = "MyStamina"
					if _G["g_UserGlobal['"..user_defined_global_current.."']"] == nil then
						PromptDuration("User Globals called 'MyStamina' and 'MyStaminaMax' have not yet been created",3000)				
					end	
					if _G["g_UserGlobal['"..user_defined_global_current.."']"] ~= nil then
						calcstamina[e] = _G["g_UserGlobal['"..user_defined_global_current.."']"] + booster[e].quantity
					end
				end
				if booster[e].boost_style == 3 then	
					calcjump[e] = GetGamePlayerControlJumpmax()+booster[e].quantity
				end	
				if booster[e].boost_style == 4 then	
					calcspeed[e] = defaultspeed[e] + booster[e].quantity/100
				end
				if booster[e].user_global_affected ~= "" then
					if _G["g_UserGlobal['"..booster[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..booster[e].user_global_affected.."']"] end
					defaultglobal[e] = currentvalue[e]
					calcglobal[e] = currentvalue[e] + booster[e].quantity
				end	
			end	
			if addquantity == 2 then
				if booster[e].boost_style == 1 then
					calchealth[e] = g_PlayerHealth - booster[e].quantity
					if calchealth[e] <= 0 then calchealth[e] = 0 end
				end	
				if booster[e].boost_style == 2 then	
					local user_defined_global_current = "MyStamina"
					if _G["g_UserGlobal['"..user_defined_global_current.."']"] == nil then
						PromptDuration("User Globals called 'MyStamina' and 'MyStaminaMax' have not been created",3000)				
					end	
					if _G["g_UserGlobal['"..user_defined_global_current.."']"] ~= nil then
						calcstamina[e] = _G["g_UserGlobal['"..user_defined_global_current.."']"] - booster[e].quantity
					end
				end
				if booster[e].boost_style == 3 then					
					calcjump[e] = GetGamePlayerControlJumpmax()-booster[e].quantity
				end	
				if booster[e].boost_style == 4 then	
					calcspeed[e] = defaultspeed[e] - booster[e].quantity/100
				end
				if booster[e].user_global_affected ~= "" then
					if _G["g_UserGlobal['"..booster[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..booster[e].user_global_affected.."']"] end
					defaultglobal[e] = currentvalue[e]
					calcglobal[e] = currentvalue[e] - booster[e].quantity
				end
			end		
		end		
	end
	-- Process Boosts
	if g_Time < booster_timer[e] and item_used[e] == 1 then
		SetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
		local currentvalue = 0
		if addquantity == 1 then
			if booster[e].boost_style == 1 then				
				SetPlayerHealthCore(calchealth[e])
			end
			if booster[e].boost_style == 2 then
				local user_defined_global_current = "MyStamina"
				if _G["g_UserGlobal['"..user_defined_global_current.."']"] ~= nil then
					_G["g_UserGlobal['"..user_defined_global_current.."']"] = calcstamina[e]
				end				
			end		
			if booster[e].boost_style == 3 then
				SetGamePlayerControlJumpmax(calcjump[e])
			end
			if booster[e].boost_style == 4 then
				SetGamePlayerControlSpeedRatio(calcspeed[e])
			end				
			if booster[e].boost_style == 5 then
				if booster[e].user_global_affected ~= "" then					
					_G["g_UserGlobal['"..booster[e].user_global_affected.."']"] = calcglobal[e]
				end
			end
		end
		if addquantity == 2 then
			if booster[e].boost_style == 1 then
				if calchealth[e] > 0 then SetPlayerHealthCore(calchealth[e]) end
				if calchealth[e] <= 0 then HurtPlayer(-1,g_PlayerHealth) end
			end
			if booster[e].boost_style == 2 then				
				local user_defined_global_current = "MyStamina"
				if _G["g_UserGlobal['"..user_defined_global_current.."']"] ~= nil then
					_G["g_UserGlobal['"..user_defined_global_current.."']"] = calcstamina[e]
				end
			end		
			if booster[e].boost_style == 3 then
				SetGamePlayerControlJumpmax(calcjump[e])
			end
			if booster[e].boost_style == 4 then
				SetGamePlayerControlSpeedRatio(calcspeed[e])
			end				
			if booster[e].boost_style == 5 then	
				if booster[e].user_global_affected ~= "" then					
					_G["g_UserGlobal['"..booster[e].user_global_affected.."']"] = calcglobal[e]
				end
			end	
		end
	end
	-- Reset Defaults and finish
	if g_Time >= booster_timer[e] then
		SetGamePlayerControlSpeedRatio(defaultspeed[e])
		SetGamePlayerControlJumpmax(defaultjump[e])
		if booster[e].user_global_affected ~= "" then					
			_G["g_UserGlobal['"..booster[e].user_global_affected.."']"] = defaultglobal[e]
		end		
		Destroy(e) -- can only destroy resources that are qty zero
	end
end