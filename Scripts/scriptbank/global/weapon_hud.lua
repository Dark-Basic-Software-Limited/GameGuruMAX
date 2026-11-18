-- Weapon Hud v1 by Necrym59
-- DESCRIPTION: Provides a User Hud display when a particular weapon is being used.
-- DESCRIPTION: Attach to an object and set AlwaysActive = ON
-- DESCRIPTION: [@SELECTED_WEAPON$=-1(0=AnyWeaponList)] Specific Weapon/Tool to use 
-- DESCRIPTION: [@@USER_HUD_SCREEN$="In-Game HUD"(0=hudscreenlist)] eg; Weapon HUD 2

g_liveHudScreen			= {}
local weaponhud			= {}
local selected_weapon	= {}
local user_hud_screen	= {}
local status			= {}

function weapon_hud_properties(e, selected_weapon, user_hud_screen)
	weaponhud[e].selected_weapon = tostring(GetWeaponName(selected_weapon-1))	
	weaponhud[e].user_hud_screen = user_hud_screen or "In-Game HUD"
end

function weapon_hud_init(e)
	weaponhud[e] = {}
	weaponhud[e].selected_weapon = ""
	weaponhud[e].user_hud_screen = "In-Game HUD"
	g_liveHudScreen = 0
	status[e] = "init"
end

function weapon_hud_main(e)
	if status[e] == "init" then
		status[e] = "endinit"
	end

	if weaponhud[e].selected_weapon ~= "" then --Has weapon			
		CurrentlyHeldWeaponID = GetPlayerWeaponID()
		if CurrentlyHeldWeaponID == GetWeaponID(weaponhud[e].selected_weapon) then 
			--DISPLAY HUD
			ScreenToggle(weaponhud[e].user_hud_screen)
			g_liveHudScreen = 1
			DisableBoundHudKeys()
		end
		if CurrentlyHeldWeaponID ~= GetWeaponID(weaponhud[e].selected_weapon) then 
			--CLOSE HUD
			ScreenToggle("")
			g_liveHudScreen = 0
			EnableBoundHudKeys()
		end
	end
end
