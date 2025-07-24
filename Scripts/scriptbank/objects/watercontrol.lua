-- Water Control v16 - by Necrym59
-- DESCRIPTION: Control water height when activated by switch or a zone.
-- DESCRIPTION: Attach to an object and set to Physics=ON/OFF. Link to a switch or zone.
-- DESCRIPTION: [PROMPT_TEXT$="Don't drown"]
-- DESCRIPTION: [@ACTION=1(1=Raise, 2=Lower)] the water level
-- DESCRIPTION: [ADJUSTMENT_AMOUNT=80]
-- DESCRIPTION: [#ADJUSTMENT_SPEED=0.02(0,5)]
-- DESCRIPTION: [NPC_DROWNING!=0] If on will drown all NPC's under water level.
-- DESCRIPTION: [LEVEL_OVERIDE!=0] If set on will start level from water start level.
-- DESCRIPTION: [WATER_START_LEVEL=0(-1000,1000)] Optional Water Start Level
-- DESCRIPTION: [CHANGE_COLOR!=0] If set on will change water to rgb values set.
-- DESCRIPTION: [WATER_COLOR_R=0(0,255)]
-- DESCRIPTION: [WATER_COLOR_G=0(0,255)]
-- DESCRIPTION: [WATER_COLOR_B=0(0,255)]
-- DESCRIPTION: <Sound0> played when raising or lowering

local g_waterControlMasterE = 0
local watercontrol = {}
local prompt_text = {}
local action = {}
local adjustment_amount = {}
local adjustment_speed = {}
local npc_drowning = {}
local level_overide = {}
local water_start_level ={}
local change_color = {}
local water_color_r = {}
local water_color_g = {}
local water_color_b = {}
local current_adjust = {}

local status = {}
local state = {}
local doonce = {}
local switch = {}
local whhold = {}

function watercontrol_properties(e, prompt_text, action, adjustment_amount, adjustment_speed, npc_drowning, level_overide, water_start_level, change_color, water_color_r, water_color_g, water_color_b)
	watercontrol[e].prompt_text = prompt_text
	watercontrol[e].action = action or 1
	watercontrol[e].adjustment_amount = adjustment_amount or 80
	watercontrol[e].adjustment_speed = adjustment_speed or 1
	watercontrol[e].npc_drowning = npc_drowning or 0
	watercontrol[e].level_overide = level_overide or 0
	watercontrol[e].water_start_level = water_start_level
	watercontrol[e].change_color = change_color or 0
	watercontrol[e].water_color_r = water_color_r
	watercontrol[e].water_color_g = water_color_g
	watercontrol[e].water_color_b = water_color_b
end 

function watercontrol_init(e)
	watercontrol[e] = {}
	watercontrol[e].prompt_text = "Don't drown"
	watercontrol[e].action = 1
	watercontrol[e].adjustment_amount = 80
	watercontrol[e].adjustment_speed = 1
	watercontrol[e].npc_drowning = 0
	watercontrol[e].level_overide = 0
	watercontrol[e].water_start_level = 0
	watercontrol[e].change_color = 0	
	watercontrol[e].water_color_r = 0
	watercontrol[e].water_color_g = 0
	watercontrol[e].water_color_b = 0
	
	status[e] = "init"
	state[e] = 0
	switch[e] = 0	
	doonce[e] = 0
	current_adjust[e] = 0
	SetEntityAlwaysActive(e,1)
end

function watercontrol_main(e)
	if status[e] == "init" then		
		whhold[e] = watercontrol[e].adjustment_amount
		if watercontrol[e].level_overide == 0 then watercontrol[e].water_start_level = GetWaterHeight() end	
		if watercontrol[e].level_overide == 1 then SetWaterHeight(watercontrol[e].water_start_level) end
		status[e] = "end"
	end
	
	if g_Entity[e]['activated'] == 1 then		
		if state[e] == 0 then
			state[e] = 1
			Prompt(watercontrol[e].prompt_text)
			PlaySound(e,0)
			PerformLogicConnections(e)
			if watercontrol[e].action == 1 then watercontrol[e].adjustment_amount = (GetWaterHeight() + watercontrol[e].adjustment_amount) end
			if watercontrol[e].action == 2 then watercontrol[e].adjustment_amount = (GetWaterHeight() - watercontrol[e].adjustment_amount) end
			if g_waterControlMasterE ~= e then
				SetEntityActivated(g_waterControlMasterE,0)
				g_waterControlMasterE = 0
			end
			g_waterControlMasterE = e
		end
		if g_waterControlMasterE == e then	
			current_adjust[e] = GetWaterHeight()
			if watercontrol[e].action == 1 then -- Raise Water Height				
				if watercontrol[e].npc_drowning == 0 then
					for a = 1, g_EntityElementMax do
						if a ~= nil and g_Entity[a] ~= nil then
							local allegiance = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)
							if allegiance == 0 or allegiance == 1 or allegiance == 2 then
								SetEntityUnderwaterMode(a,1)
							end
						end
					end
				end	
				if watercontrol[e].change_color == 1 and doonce[e] == 0 then
					SetWaterColor(watercontrol[e].water_color_r, watercontrol[e].water_color_g, watercontrol[e].water_color_b)
					doonce[e] = 1
				end
				if current_adjust[e] < watercontrol[e].adjustment_amount then				
					current_adjust[e] = current_adjust[e] + watercontrol[e].adjustment_speed
					SetWaterHeight(current_adjust[e])
					ShowWater()
					if current_adjust[e] >= watercontrol[e].adjustment_amount then
						current_adjust[e] = watercontrol[e].adjustment_amount
						SetWaterHeight(current_adjust[e])
						SetEntityActivated(e,0)
						watercontrol[e].action = 2
						watercontrol[e].adjustment_amount = whhold[e]
						state[e] = 0
					end
				end				
			end
			if watercontrol[e].action == 2 then -- Lower Water Height
				if current_adjust[e] > watercontrol[e].adjustment_amount then
					current_adjust[e] = current_adjust[e] - watercontrol[e].adjustment_speed
					SetWaterHeight(current_adjust[e])
					ShowWater()
					if current_adjust[e] <= watercontrol[e].adjustment_amount then
						current_adjust[e] = watercontrol[e].adjustment_amount
						SetWaterHeight(current_adjust[e])
						SetEntityActivated(e,0)
						watercontrol[e].action = 1
						watercontrol[e].adjustment_amount = whhold[e]
						state[e] = 0
					end
				end				
			end
			if current_adjust[e] <= watercontrol[e].adjustment_amount then
				if watercontrol[e].change_color == 1 and doonce[e] == 0 then
					SetWaterColor(watercontrol[e].water_color_r, watercontrol[e].water_color_g, watercontrol[e].water_color_b)
					doonce[e] = 1
				end
			end	
		end	
	end
end
