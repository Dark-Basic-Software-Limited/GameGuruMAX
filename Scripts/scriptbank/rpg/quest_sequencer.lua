-- Quest Sequencer v14 by Necrym59
-- DESCRIPTION: Will sequentially run the next Quest/Mission in order at completion of each.
-- DESCRIPTION: If a quests Quest Status = END_SEQ then the Quest Sequencer itself is destroyed when completed.
-- DESCRIPTION: [QUEST_START_TEXT$="Quest Sequence Activated"]
-- DESCRIPTION: [QUEST_END_TEXT$="Quest Completed"]
-- DESCRIPTION: [@QUEST_READOUT=1(1=Title, 2=Description1)] 
-- DESCRIPTION: [@@READOUT_USER_GLOBAL$=""(0=globallist)]] user global name (eg: MyReadout)
-- DESCRIPTION: [@QuestChoice=1(0=QuestList)]
-- DESCRIPTION: [@SPAWN_QUEST_OBJECT=1(1=On,2=Off)] when quest accepted.
-- DESCRIPTION: [HIDE_OBJECT!=0]
-- DESCRIPTION: <Sound1> when quest completed.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"

g_ActivateQuestComplete 	= {}

local lower = string.lower
local g_quest_sequence		= {}
local quest_prompt			= {}
local quest_endtext			= {}
local quest_readout			= {}
local readout_user_global	= {}
local spawn_quest_object	= {}
local hide_object			= {}
local quest_objno			= {}
local quest_recno			= {}
local quest_quantity		= {}
local check_quantity		= {}
local check_timer			= {}
local play_once				= {}
local doonce				= {}
local promptonce			= {}
local qactivate				= {}
local status				= {}

function quest_sequencer_properties(e, quest_start_text, quest_end_text, quest_readout, readout_user_global, questchoice, spawn_quest_object, hide_object)
	g_quest_sequence[e].questprompt = quest_start_text
	g_quest_sequence[e].questendtext = quest_end_text	
	g_quest_sequence[e].quest_readout = quest_readout or 1
	g_quest_sequence[e].readout_user_global = readout_user_global
	g_quest_sequence[e].questchoice = questchoice
	g_quest_sequence[e].hide_object = hide_object or 0
	g_quest_sequence[e].spawn_quest_object = spawn_quest_object	
	g_quest_sequence[e].questtitle = ""
	g_quest_sequence[e].questtype = ""
	g_quest_sequence[e].questobject = ""
	g_quest_sequence[e].questreceiver = ""
	g_quest_sequence[e].questlevel = ""
	g_quest_sequence[e].questpoints = ""
	g_quest_sequence[e].questvalue = ""
	g_quest_sequence[e].questactivate = ""
	g_quest_sequence[e].queststarted = 0
	g_quest_sequence[e].questquantity = ""
	g_quest_sequence[e].description1 = ""
	g_quest_sequence[e].description2 = ""
	g_quest_sequence[e].description3 =	""
	g_quest_sequence[e].status = ""	
end

function quest_sequencer_init(e)
	g_quest_sequence[e] = {}
	g_quest_sequence[e].questprompt = "Quest Activated"
	g_quest_sequence[e].questendtext = "Quest Completed"
	g_quest_sequence[e].quest_readout = 1	
	g_quest_sequence[e].readout_user_global = ""
	g_quest_sequence[e].questchoice = ""
	g_quest_sequence[e].hide_object = 0	
	g_quest_sequence[e].spawn_quest_object = 1	

	quest_objno[e] = 0
	quest_recno[e] = 0
	quest_quantity[e] = 0
	check_quantity[e] = 0
	check_timer[e] = 0
	play_once[e] = 0
	doonce[e] = 0
	promptonce[e] = 0
	qactivate[e] = 0
	g_ActivateQuestComplete = 0
	status[e] = "init"	
	SetEntityAlwaysActive(e,1)
end

function quest_sequencer_main(e)

	if status[e] == "init" then
		if g_quest_sequence[e].hide_object == 1 then
			CollisionOff(e)
			Hide(e)
		end
		hud0_quest_status = {}
		status[e] = "endinit"
	end

	if g_Entity[e].activated == 1 then
		if promptonce[e] == 0 then
			PromptDuration(g_quest_sequence[e].questprompt,2000)
			promptonce[e] = 1
		end	
		if qactivate[e] == 0 then
			-- set game to this quest and run
			local i = tonumber(g_quest_sequence[e].questchoice)-1
			g_quest_sequence[e].questtitle = GetCollectionQuestAttribute(i,"title")
			g_quest_sequence[e].questtype = GetCollectionQuestAttribute(i,"type")
			g_quest_sequence[e].questobject = GetCollectionQuestAttribute(i,"object")
			g_quest_sequence[e].questreceiver = GetCollectionQuestAttribute(i,"receiver")
			g_quest_sequence[e].questlevel = GetCollectionQuestAttribute(i,"level")
			g_quest_sequence[e].questpoints = GetCollectionQuestAttribute(i,"points")
			g_quest_sequence[e].questvalue = GetCollectionQuestAttribute(i,"value")
			g_quest_sequence[e].questactivate = GetCollectionQuestAttribute(i,"activate")
			g_quest_sequence[e].questquantity = tonumber(GetCollectionQuestAttribute(i,"quantity"))
			g_quest_sequence[e].description1 = GetCollectionQuestAttribute(i,"desc1")
			g_quest_sequence[e].description2 = GetCollectionQuestAttribute(i,"desc2")
			g_quest_sequence[e].description3 =	GetCollectionQuestAttribute(i,"desc3")
			g_quest_sequence[e].status = GetCollectionQuestAttribute(i,"status")
			
			g_UserGlobalQuestTitleActive = g_quest_sequence[e].questtitle
			g_UserGlobalQuestTitleShowing = g_quest_sequence[e].questtitle
			g_UserGlobalQuestTitleShowingObject = g_quest_sequence[e].questobject
			g_UserGlobalQuestTitleShowingObject2 = g_quest_sequence[e].questreceiver
			hud0_quest_status[i] = "active"
			g_quest_sequence[e].queststarted = 1
			for ee = 1, g_EntityElementMax, 1 do
				if e ~= ee then
					if g_Entity[ee] ~= nil then
						if GetEntityName(ee) == g_UserGlobalQuestTitleShowingObject then
							g_UserGlobalQuestTitleActiveE = ee
							break
						end
					end
				end
			end		
			qactivate[e] = 1				
		end	
		
		if g_quest_sequence[e].readout_user_global ~= "" then
			if g_quest_sequence[e].quest_readout == 1 then
				_G["g_UserGlobal['"..g_quest_sequence[e].readout_user_global.."']"] = g_quest_sequence[e].questtitle
			end
			if g_quest_sequence[e].quest_readout == 2 then
				_G["g_UserGlobal['"..g_quest_sequence[e].readout_user_global.."']"] = g_quest_sequence[e].description1
			end
		else
			_G["g_UserGlobal['"..g_quest_sequence[e].readout_user_global.."']"] = ""
		end	
		
		if g_quest_sequence[e].questtitle == g_UserGlobalQuestTitleActive then			
			-- hide in game, we have accepted this one and doing it now
			if g_UserGlobalQuestTitleActiveE > 0 then
				local tquestcomplete = 0
				
				if g_quest_sequence[e].queststarted == 1 then
					if doonce[e] == 0 then
						ActivateIfUsed(e)
						doonce[e] = 1
					end
				end
				
				if quest_objno[e] > 0 then		
					if g_quest_poster[e]['spawn_quest_object'] ~= 0 then
						if GetEntitySpawnAtStart(quest_objno[e]) == 0 then
							Spawn(quest_objno[e])
							g_quest_poster[e]['spawn_quest_object'] = 0
						end
					end
				end				

				if g_quest_sequence[e].questtype == "activate" then
					-- ACTIVATE
					if quest_objno[e] == 0 then						
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_sequence[e].questobject) then
									quest_objno[e] = a									
									break
								end
							end
						end
					end
					if g_ActivateQuestComplete == 1 then tquestcomplete = 1 end
				end	

				if g_quest_sequence[e].questtype == "collect" then
					-- COLLECT
					if quest_objno[e] == 0 then						
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_sequence[e].questobject) then
									quest_objno[e] = a									
									break
								end
							end
						end
					end
					----Complete Collect Single Item Quest ----------------------
					if GetEntityCollected(quest_objno[e]) == 1 and g_quest_sequence[e].questquantity == 1 then					
						tquestcomplete = 1							
					end
					---- Complete Collect Multi Items Quest ----------------------
					if g_quest_sequence[e].questquantity > 1 then
						if g_Time > check_timer[e] then
							check_timer[e] = check_timer[e] + 500  -- check once per 1/2second			
							quest_quantity[e] = 0
							for ee = 1, g_EntityElementMax, 1 do
								if GetEntityCollected(ee) == 1 then
									if lower(GetEntityName(ee)) == lower(g_quest_sequence[e].questobject) then
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
						if tcurrentcount >= g_quest_sequence[e].questquantity then
							tcurrentcount = g_quest_sequence[e].questquantity
							tquestcomplete = 1
						end					
						if quest_quantity[e] > check_quantity[e] then PromptDuration(tcurrentcount.. " of " ..g_quest_sequence[e].questquantity.. " " ..g_quest_sequence[e].questobject.. "'s collected",2000) end
						check_quantity[e] = quest_quantity[e]
					end
				end
				
				if g_quest_sequence[e].questtype == "destroy" then
					-- DESTROY					
					if quest_objno[e] == 0 then
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_sequence[e].questobject) then
								quest_objno[e] = a																
								break
								end
							end
						end
					end						
					if quest_objno[e] > 0 then
						if g_Entity[quest_objno[e]].health == 0 then
							tquestcomplete = 1
						end
					end	
				end
				
				if g_quest_sequence[e].questtype == "deliver" then					
					-- DELIVER
					if quest_objno[e] == 0 then
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_sequence[e].questobject) then
								quest_objno[e] = a								
								break
								end
							end
						end
					end
					if quest_recno[e] == 0 then
						for b = 1, g_EntityElementMax do
							if b ~= nil and g_Entity[b] ~= nil then
								if lower(GetEntityName(b)) == lower(g_quest_sequence[e].questreceiver) then
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
							PromptDuration("Press Q to complete",3000)
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
					PlayNon3DSound(e,1)
					PromptDuration(g_quest_sequence[e].questendtext,2000)
					-- award value
					local myMoney = 0
					if _G["g_UserGlobal.".."MyMoney"..""] ~= nil then myMoney = _G["g_UserGlobal.".."MyMoney"..""] end
					_G["g_UserGlobal.".."MyMoney"..""] = myMoney + tonumber(g_quest_sequence[e].questvalue)
					-- award XP
					local myXP = 0
					if _G["g_UserGlobal.".."MyXP"..""] ~= nil then myXP = _G["g_UserGlobal.".."MyXP"..""] end
					_G["g_UserGlobal.".."MyXP"..""] = myXP + tonumber(g_quest_sequence[e].questpoints)
					-- activate another object
					for ee = 1, g_EntityElementMax do
						if ee ~= nil and g_Entity[ee] ~= nil then
							if GetEntityName(ee) ~= nil then
								if string.lower(GetEntityName(ee)) == string.lower(g_quest_sequence[e].questactivate) then
									SetActivated(ee,1)
									PerformLogicConnections(ee)
									break
								end
							end
						end
					end	
		
					-- end and deactivate or do next quest in list
					if g_quest_sequence[e].status == "END_SEQ" then
						_G["g_UserGlobal['"..g_quest_sequence[e].readout_user_global.."']"] = ""
						g_quest_sequence[e].questchoice = 0
						g_UserGlobalQuestTitleActive = ""
						g_quest_sequence[e].queststarted = 0
						g_UserGlobalQuestTitleActiveObject = ""
						g_UserGlobalQuestTitleActiveObject2 = ""			
						g_UserGlobalQuestTitleActiveE = 0
						g_UserGlobalQuestTitleShowing = ""
						g_UserGlobalQuestTitleShowingObject = ""
						g_UserGlobalQuestTitleShowingObject2 = ""
						hud0_quest_status = {}
						quest_objno[e] = 0
						quest_recno[e] = 0	
						qactivate[e] = 0
						SetActivated(e,0)
						SetEntityAlwaysActive(e,0)
						g_ActivateQuestComplete = 0
						SwitchScript(e,"no_behavior_selected.lua")
						--Destroy(e)
					else
						-- and reset quest
						g_UserGlobalQuestTitleActive = ""
						g_quest_sequence[e].queststarted = 0
						g_UserGlobalQuestTitleActiveObject = ""
						g_UserGlobalQuestTitleActiveObject2 = ""			
						g_UserGlobalQuestTitleActiveE = 0
						g_UserGlobalQuestTitleShowing = ""
						g_UserGlobalQuestTitleShowingObject = ""
						g_UserGlobalQuestTitleShowingObject2 = ""
						hud0_quest_status = {}
						quest_objno[e] = 0
						quest_recno[e] = 0						
						tquestcomplete = 0
						g_ActivateQuestComplete = 0
						g_quest_sequence[e].questchoice = g_quest_sequence[e].questchoice + 1
						qactivate[e] = 0
					end	
				end				
			end
		end		
	end
end

function GetDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[e] ~= 0 and g_Entity[v] ~= nil and g_Entity[v] ~= 0 then
		local disx = g_Entity[e].x - g_Entity[v].x
		local disz = g_Entity[e].z - g_Entity[v].z
		local disy = g_Entity[e].y - g_Entity[v].y
		return math.sqrt(disx^2 + disz^2 + disy^2)
	end
end

