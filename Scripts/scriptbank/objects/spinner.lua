-- Spinner v5 by Necrym59
-- DESCRIPTION: When activated by a switch or zone or set Is-Active, spins an object on its selected axis at the spin rate in clockwise or anticlockwise direction.
-- DESCRIPTION: Set Physics=ON, Gravity=ON. IsImobile=YES
-- DESCRIPTION: [#Acceleration=0.01(0.01,5.00)].
-- DESCRIPTION: [MaxSpinSpeed=10(1,1000)].
-- DESCRIPTION: [AntiClockwise!=0].
-- DESCRIPTION: [x_axis!=0]
-- DESCRIPTION: [y_axis!=1]
-- DESCRIPTION: [z_axis!=0]
-- DESCRIPTION: [DamageRange=1(1,1000)]
-- DESCRIPTION: [DamageAmount=1(1,1000)]
-- DESCRIPTION: [DamageTimer=500(0,2000)] in milliseconds
-- DESCRIPTION: [@HEALTH_AFFECTED=1(1=Yes, No)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyGlobal
-- DESCRIPTION: [IS_ACTIVE!=0] if off use a switch or zone to activate.
-- DESCRIPTION: <Sound0> - Loop Effect Sound
-- DESCRIPTION: <Sound1> - Hit Effect Sound

local spinner 				= {}
local acceleration			= {}
local maxspinspeed			= {}
local anticlockwise			= {}
local x_axis				= {}
local y_axis				= {}
local z_axis				= {}
local damagerange			= {}
local damageamount			= {}
local damagetimer			= {}
local health_affected		= {}
local user_global_affected	= {}
local is_active				= {}

local status 			= {}
local played			= {}
local current_level 	= {}
local currentvalue		= {}

function spinner_properties(e, acceleration, maxspinspeed, anticlockwise, x_axis, y_axis, z_axis, damagerange, damageamount, damagetimer, health_affected, user_global_affected, is_active)
	spinner[e].acceleration = acceleration
	spinner[e].maxspinspeed = maxspinspeed
	spinner[e].anticlockwise = anticlockwise
	spinner[e].x_axis = x_axis
	spinner[e].y_axis = y_axis
	spinner[e].z_axis = z_axis
	spinner[e].damagerange = damagerange
	spinner[e].damageamount = damageamount
	spinner[e].damagetimer = damagetimer
	spinner[e].health_affected = health_affected
	spinner[e].user_global_affected = user_global_affected
	spinner[e].is_active = is_active or 0
end

function spinner_init(e)
	spinner[e] = {}
	spinner[e].acceleration = 1
	spinner[e].maxspinspeed = 1
	spinner[e].anticlockwise = 0
	spinner[e].x_axis = 0
	spinner[e].y_axis = 1
	spinner[e].z_axis = 0
	spinner[e].damagerange = 1
	spinner[e].damageamount = 1
	spinner[e].damagetimer = 500
	spinner[e].health_affected = 1
	spinner[e].user_global_affected = ""
	spinner[e].is_active = 0

	status[e] = "init"
	current_level[e] = 0
	played[e] = 0
	currentvalue[e] = 0
	CollisionOff(e)
end

function spinner_main(e)

	if status[e] == "init" then
		if spinner[e].anticlockwise == 0 then current_level[e] = 0 end
		if spinner[e].anticlockwise == 1 then spinner[e].maxspinspeed = (spinner[e].maxspinspeed - spinner[e].maxspinspeed*2) end
		if spinner[e].is_active == 1 then SetActivated(e,1) end
		if spinner[e].is_active == 0 then SetActivated(e,0) end
		StartTimer(e)
		status[e] = "endinit"
	end

	if g_Entity[e].activated == 1 then
		if played[e] == 0 then
			LoopSound(e,0)
			played[e] = 1
		end
		if spinner[e].anticlockwise == 0 then
			if current_level[e] < spinner[e].maxspinspeed then current_level[e] = current_level[e] + spinner[e].acceleration end
		end
		if spinner[e].anticlockwise == 1 then
			if current_level[e] >= spinner[e].maxspinspeed then	current_level[e] = current_level[e] - spinner[e].acceleration end
		end
		if spinner[e].x_axis == 1 then
			CollisionOff(e)
			RotateX(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)
		end
		if spinner[e].y_axis == 1 then
			CollisionOff(e)
			RotateY(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)
		end
		if spinner[e].z_axis == 1 then
			CollisionOff(e)
			RotateZ(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)
		end
		if GetTimer(e) > spinner[e].damagetimer then
			if GetPlayerDistance(e) <= spinner[e].damagerange and current_level[e] >= spinner[e].maxspinspeed/2 then
				if spinner[e].health_affected == 1 then
					PlaySound(e,1)
					HurtPlayer(-1,spinner[e].damageamount)					
				end
				if  spinner[e].user_global_affected ~= "" then
					if _G["g_UserGlobal['".. spinner[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['".. spinner[e].user_global_affected.."']"] end
					if currentvalue[e] > 0 then
						_G["g_UserGlobal['".. spinner[e].user_global_affected.."']"] = currentvalue[e] - spinner[e].damageamount
					else
						_G["g_UserGlobal['".. spinner[e].user_global_affected.."']"] = 0
					end
				end
			end
			StartTimer(e)
		end
	end
	if g_Entity[e].activated == 0 then
		if current_level[e] > 0 then current_level[e] = current_level[e] - spinner[e].acceleration end
		if spinner[e].x_axis == 1 then
			CollisionOff(e)
			RotateX(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)
		end
		if spinner[e].y_axis == 1 then
			CollisionOff(e)
			RotateY(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)
		end
		if spinner[e].z_axis == 1 then
			CollisionOff(e)
			RotateZ(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)
		end
		if current_level[e] <= 0 then
			StopSound(e,0)
			played[e] = 0
			g_Entity[e].activated = 0
		end
		if GetTimer(e) > spinner[e].damagetimer then
			if GetPlayerDistance(e) <= spinner[e].damagerange and current_level[e] >= spinner[e].maxspinspeed/2	then
				if spinner[e].health_affected == 1 then
					PlaySound(e,1)
					HurtPlayer(-1,spinner[e].damageamount)					
				end
				if  spinner[e].user_global_affected ~= "" then
					if _G["g_UserGlobal['".. spinner[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['".. spinner[e].user_global_affected.."']"] end
					if currentvalue[e] > 0 then
						_G["g_UserGlobal['".. spinner[e].user_global_affected.."']"] = currentvalue[e] - spinner[e].damageamount
					else
						_G["g_UserGlobal['".. spinner[e].user_global_affected.."']"] = 0
					end
				end
			end
			StartTimer(e)
		end
	end
end