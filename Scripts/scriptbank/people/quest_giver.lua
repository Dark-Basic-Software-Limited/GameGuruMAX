-- Quest Giver v14 by Necrym59 and Lee
-- DESCRIPTION: When player is within [RANGE=100] distance, will show [QUEST_PROMPT$="Press E to Interact"] 
-- DESCRIPTION: When E is pressed, player will be shown [@@QUEST_SCREEN$="HUD Screen 8"(0=hudscreenlist)]
-- DESCRIPTION: with [@QuestChoice=1(0=QuestList)] and play [SPEECH1$=""]
-- DESCRIPTION: and will play <Sound0> when quest is completed.
-- DESCRIPTION: [@SPAWN_QUEST_OBJECT=1(1=On,2=Off)] when quest accepted.

master_interpreter_core = require "scriptbank\\masterinterpreter"
local U = require "scriptbank\\utillib"
--g_quest_giver_behavior = {}
--g_quest_giver_behavior_count = 0
g_ActivateQuestComplete = {}

local lower = string.lower
local g_quest_giver 		= {}
local IsCharacter			= {}
local quest_objno			= {}
local quest_recno			= {}
local quest_quantity		= {}
local check_quantity		= {}
local check_timer			= {}
local spawn_quest_object	= {}
local play_once				= {}
local doonce				= {}
local anim_once				= {}
local prompt_once			= {}

function quest_giver_init_file(e,scriptfile)
	g_quest_giver[e] = {}
	--a dangerous precident to run BYC and regular LUA alongside each other!
	--g_quest_giver[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
	--g_quest_giver_behavior_count = master_interpreter_core.masterinterpreter_load (g_quest_giver[e],g_quest_giver_behavior )
	quest_giver_properties(e,100,"Press E to Interact","HUD Screen 8","")	
	quest_objno[e] = 0	
	quest_recno[e] = 0
	quest_quantity[e] = 0
	check_quantity[e] = 0
	check_timer[e] = 0
	play_once[e] = 0
	doonce[e] = 0	
	prompt_once[e] = 0
	anim_once[e] = 0
	g_ActivateQuestComplete = 0	
end

function quest_giver_properties(e, range, questprompt, questscreen, questchoice, spawn_quest_object)
	g_quest_giver[e]['range'] = range
	g_quest_giver[e]['questprompt'] = questprompt
	g_quest_giver[e]['questscreen'] = questscreen
	g_quest_giver[e]['questchoice'] = questchoice
	g_quest_giver[e]['spawn_quest_object'] = spawn_quest_object
	g_quest_giver[e]['questtitle'] = ""
	g_quest_giver[e]['questtype'] = ""
	g_quest_giver[e]['questobject'] = ""
	g_quest_giver[e]['questreceiver'] = ""
	g_quest_giver[e]['questlevel'] = ""
	g_quest_giver[e]['questpoints'] = ""
	g_quest_giver[e]['questvalue'] = ""
	g_quest_giver[e]['questactivate'] = ""
	g_quest_giver[e]['queststarted'] = 0
	g_quest_giver[e]['questquantity'] = ""
	g_quest_giver[e]['questendtext'] = "Quest Completed"
	g_quest_giver[e]['ischaracter'] = 1
	--g_quest_giver[e]['name'] = "Quest Giver 57"
	g_quest_giver[e]['name'] = GetEntityName(e)
	--a dangerous precident to run BYC and regular LUA alongside each other!
	--master_interpreter_core.masterinterpreter_restart (g_quest_giver[e], g_Entity[e])
end

function quest_giver_main(e)
	--a dangerous precident to run BYC and regular LUA alongside each other!
	--if g_quest_giver[e] ~= nil and g_quest_giver_behavior_count > 0 then
	--	local returnvalue = master_interpreter_core.masterinterpreter (g_quest_giver_behavior, g_quest_giver_behavior_count, e, g_quest_giver[e], g_Entity[e])
	--    if returnvalue >= 0 then
	--		g_quest_giver_behavior_count = returnvalue
	--	end
	--end
	SetEntityAlwaysActive(e,1)
	if anim_once[e] == 0 then
		SetAnimationName(e,"idle")
		LoopAnimation(e)
		anim_once[e] = 1
	end	
	if g_quest_giver[e]['questtitle'] == "" then
		local totalquests = GetCollectionQuestQuantity()
		if totalquests ~= nil then
			local i = tonumber(g_quest_giver[e]['questchoice'])-1
			if i ~= nil then
				if i > 0 and i <= totalquests then
					g_quest_giver[e]['questtitle'] = GetCollectionQuestAttribute(i,"title")
					g_quest_giver[e]['questtype'] = GetCollectionQuestAttribute(i,"type")
					g_quest_giver[e]['questobject'] = GetCollectionQuestAttribute(i,"object")
					g_quest_giver[e]['questreceiver'] = GetCollectionQuestAttribute(i,"receiver")
					g_quest_giver[e]['questlevel'] = GetCollectionQuestAttribute(i,"level")
					g_quest_giver[e]['questpoints'] = GetCollectionQuestAttribute(i,"points")
					g_quest_giver[e]['questvalue'] = GetCollectionQuestAttribute(i,"value")
					g_quest_giver[e]['questactivate'] = GetCollectionQuestAttribute(i,"activate")
					g_quest_giver[e]['questquantity'] = tonumber(GetCollectionQuestAttribute(i,"quantity"))
				end
			else
				PromptDuration(g_quest_giver[e]['questchoice'],5000)
			end
		end
	end
	
	if g_UserGlobalQuestTitleActive == nil then g_UserGlobalQuestTitleActive = "" end
	if g_UserGlobalQuestTitleActive ~= nil then
		if g_quest_giver[e]['queststarted'] == 0 then
			Show(e)
			for tquestindex = 1, hud0_quest_qty, 1 do
				if GetCollectionQuestAttribute(tquestindex,"title") == g_quest_giver[e]['questtitle'] then
					if hud0_quest_status[tquestindex] == "active" then
						g_quest_giver[e]['queststarted'] = 1
					end
					break
				end
			end
			local PlayerDist = GetPlayerDistance(e)
			if PlayerDist < g_quest_giver[e]['range'] then
				LookAtPlayer(e,10)
				PromptDuration(g_quest_giver[e]['questprompt'],1000)				
				--PromptDuration(g_quest_giver[e]['questprompt'] ,1000)
				if g_KeyPressE == 1 then
					-- set game to this quest
					-- PromptGuruMeditation("PLAY SPEECH AND ANIM WITHIN REGULAR SCRIPT")
					SetAnimationName(e,"idle")					
					LoopAnimation(e)		
					if play_once[e] == 0 then 
						PlaySound(e,1)
						PlaySpeech(e,1)
						play_once[e] = 1
					end											
					g_UserGlobalQuestTitleShowing = g_quest_giver[e]['questtitle']
					g_UserGlobalQuestTitleShowingObject = g_quest_giver[e]['questobject']
					g_UserGlobalQuestTitleShowingObject2 = g_quest_giver[e]['questreceiver']
					ScreenToggle(g_quest_giver[e]['questscreen'])
				end
			end
		else
			if g_quest_giver[e]['ischaracter'] == 0 then Hide(e) end
			for tquestindex = 1, hud0_quest_qty, 1 do
				if GetCollectionQuestAttribute(tquestindex,"title") == g_quest_giver[e]['questtitle'] then
					if hud0_quest_status[tquestindex] == "inactive" then
						g_quest_giver[e]['queststarted'] = 0
					end
					break
				end
			end
		end
		if g_quest_giver[e]['questtitle'] == g_UserGlobalQuestTitleActive then			
			-- hide in game, we have accepted this one and doing it now
			if g_UserGlobalQuestTitleActiveE > 0 then
			
				local tquestcomplete = 0
				if g_quest_giver[e]['queststarted'] == 1 then
					if doonce[e] == 0 then
						ActivateIfUsed(e)
						doonce[e] = 1
					end
				end
				
				if quest_objno[e] > 0 then		
					if g_quest_giver[e]['spawn_quest_object'] ~= 0 then
						if GetEntitySpawnAtStart(quest_objno[e]) == 0 then
							Spawn(quest_objno[e])
							g_quest_giver[e]['spawn_quest_object'] = 0
						end
					end
				end

				if g_quest_giver[e]['questtype'] == "activate" then
					-- ACTIVATE
					if quest_objno[e] == 0 then						
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if g_Entity[a]['y'] > -99999 then
									if lower(GetEntityName(a)) == lower(g_quest_giver[e]['questobject']) then
										quest_objno[e] = a									
										break
									end
								end
							end
						end
					end
					if g_ActivateQuestComplete == 1 then tquestcomplete = 1 end
				end				
				
				if g_quest_giver[e]['questtype'] == "collect" then
					-- COLLECT
					if quest_objno[e] == 0 then						
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if g_Entity[a]['y'] > -99999 then
									if lower(GetEntityName(a)) == lower(g_quest_giver[e]['questobject']) then
										quest_objno[e] = a
										break
									end
								end
							end
						end
					end
					----Complete Collect Single Item Quest ----------------------
					if GetEntityCollected(quest_objno[e]) == 1 and g_quest_giver[e]['questquantity'] == 1 then					
						tquestcomplete = 1							
					end
					---- Complete Collect Multi Items Quest ----------------------
					if g_quest_giver[e]['questquantity'] > 1 then
						if g_Time > check_timer[e] then
							check_timer[e] = check_timer[e] + 500  -- check once per 1/2second			
							quest_quantity[e] = 0
							for ee = 1, g_EntityElementMax, 1 do
								if GetEntityCollected(ee) == 1 then
									if lower(GetEntityName(ee)) == lower(g_quest_giver[e]['questobject']) then
										tqty = GetEntityQuantity(ee)
										if GetEntityCollectable(entityindex) == 2 then
											-- resources spent can be zero
											if tqty < 0 then tqty = 0 end
										else
											-- non resource objects can be minus one
											if tqty < 1 then tqty = 1 end
										end											
										quest_quantity[e] = quest_quantity[e] + tqty
									end
								end
							end
						end						
						tcurrentcount = quest_quantity[e]						
						if tcurrentcount >= g_quest_giver[e]['questquantity'] then
							tcurrentcount = g_quest_giver[e]['questquantity']
							tquestcomplete = 1
						end
						if quest_quantity[e] > check_quantity[e] then PromptDuration(tcurrentcount.. " of " ..g_quest_giver[e]['questquantity'].. " " ..g_quest_giver[e]['questobject'].. "'s collected",2000) end						
						check_quantity[e] = quest_quantity[e]
					end
				end
				
				if g_quest_giver[e]['questtype'] == "destroy" then
					-- DESTROY
					if quest_objno[e] == 0 then
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if g_Entity[a]['y'] > -99999 then
									if lower(GetEntityName(a)) == lower(g_quest_giver[e]['questobject']) then
									quest_objno[e] = a
									break
									end
								end
							end
						end
					end
					if g_Entity[quest_objno[e]]['health'] <= 0 then
						tquestcomplete = 1
					end
				end				
				if g_quest_giver[e]['questtype'] == "deliver" then
					-- DELIVER
					if quest_objno[e] == 0 or nil then
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if g_Entity[a]['y'] > -99999 then
									if lower(GetEntityName(a)) == lower(g_quest_giver[e]['questobject']) then
									quest_objno[e] = a								
									break
									end
								end
							end
						end
					end	
					if g_quest_giver[e]['questreceiver'] == g_quest_giver[e]['questtitle'] or "none" then
						quest_recno[e] = e
					else
						quest_recno[e] = 0
					end
					if quest_recno[e] == 0 or nil then
						for b = 1, g_EntityElementMax do
							if b ~= nil and g_Entity[b] ~= nil then
								if lower(GetEntityName(b)) == lower(g_quest_giver[e]['questreceiver']) then
								quest_recno[e] = b
								break
								end
							end
						end
					end					
					if GetEntityCollected(quest_objno[e]) == 1 then
						Hide(quest_objno[e])
						GravityOff(quest_objno[e])
						CollisionOff(quest_objno[e])
						ResetPosition(quest_objno[e],g_PlayerPosX,g_PlayerPosY+100,g_PlayerPosZ)
						if GetDistance(quest_objno[e],quest_recno[e]) <= 200 then
							PromptDuration("Press Q to complete quest",1000)
							if g_KeyPressQ == 1 then																
								local ox,oy,oz = U.Rotate3D(0,0,30,math.rad(g_PlayerAngX),math.rad(g_PlayerAngY),math.rad(g_PlayerAngZ))
								local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz								
								GravityOn(quest_objno[e])
								ResetPosition(quest_objno[e],forwardposx, forwardposy, forwardposz)
								CollisionOn(quest_objno[e])
								Show(quest_objno[e])								
								tquestcomplete = 1
								SetEntityCollected(quest_objno[e],0)
							end
						end
					end
				end
				if tquestcomplete == 1 then
					SetSoundVolume(100)
					PlayNon3DSound(e,0)
					PromptDuration(g_quest_giver[e]['questendtext'],2000)
					-- award value
					local myMoney = 0
					if _G["g_UserGlobal['".."MyMoney".."']"] ~= nil then myMoney = _G["g_UserGlobal['".."MyMoney".."']"] end
					_G["g_UserGlobal['".."MyMoney".."']"] = myMoney + tonumber(g_quest_giver[e]['questvalue'])
					-- award XP
					local myXP = 0
					if _G["g_UserGlobal['".."MyXP".."']"] ~= nil then myXP = _G["g_UserGlobal['".."MyXP".."']"] end
					_G["g_UserGlobal['".."MyXP".."']"] = myXP + tonumber(g_quest_giver[e]['questpoints'])
					-- activate another object
					for ee = 1, g_EntityElementMax do
						if ee ~= nil and g_Entity[ee] ~= nil then
							if GetEntityName(ee) ~= nil then
								if string.lower(GetEntityName(ee)) == string.lower(g_quest_giver[e]['questactivate']) then
									SetActivated(ee,1)
									PerformLogicConnections(ee)
									break
								end
							end
						end
					end
					-- to complete a quest means to destroy it
					PerformLogicConnections(e)
					--Destroy(e)
					-- and update quest status
					if g_UserGlobalQuestTitleActive ~= nil then
						for tquestindex = 1, hud0_quest_qty, 1 do
							if GetCollectionQuestAttribute(tquestindex,"title") == g_UserGlobalQuestTitleActive then
								hud0_quest_status[tquestindex] = "complete"
								break
							end
						end
					end
					-- and reset quest
					g_UserGlobalQuestTitleActive = ""
					g_UserGlobalQuestTitleActiveObject = ""
					g_UserGlobalQuestTitleActiveE = 0					
					quest_objno[e] = 0
					quest_recno[e] = 0
					g_ActivateQuestComplete = 0					
				end				
			end
		end		
	end
	if GetCurrentScreen() == -1 then play_once[e] = 0 end
end

function GetDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[e] ~= 0 and g_Entity[v] ~= nil and g_Entity[v] ~= 0 then
		local disx = g_Entity[e]['x'] - g_Entity[v]['x']
		local disz = g_Entity[e]['z'] - g_Entity[v]['z']
		local disy = g_Entity[e]['y'] - g_Entity[v]['y']
		return math.sqrt(disx^2 + disz^2 + disy^2)
	end
end

