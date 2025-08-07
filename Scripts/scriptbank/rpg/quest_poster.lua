-- Quest Poster v18 by Necrym59 and Lee
-- DESCRIPTION: When player is within [RANGE=100] distance, show [QUEST_PROMPT$="Press E to view this quest"] and 
-- DESCRIPTION: when E is pressed, player will be shown the [@@QUEST_SCREEN$="HUD Screen 8"(0=hudscreenlist)].
-- DESCRIPTION: Select the [@QuestChoice=1(0=QuestList)]
-- DESCRIPTION: [@SPAWN_QUEST_OBJECT=1(1=On,2=Off)] when quest accepted.
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> when viewing the quest.
-- DESCRIPTION: <Sound1> when quest completed.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_ActivateQuestComplete = {}

local lower = string.lower
local g_quest_poster 		= {}
local quest_objno			= {}
local quest_recno			= {}
local quest_quantity		= {}
local check_quantity		= {}
local check_timer			= {}
local play_once				= {}
local doonce				= {}
local tEnt 					= {}
local selectobj 			= {}
local spawn_quest_object	= {}
local prompt_display 		= {}
local item_highlight 		= {}
local highlight_icon 		= {}
local status 				= {}
local hl_icon 				= {}
local hl_imgwidth 			= {}
local hl_imgheight 			= {}

function quest_poster_properties(e, range, questprompt, questscreen, questchoice, spawn_quest_object, prompt_display, item_highlight, highlight_icon_imagefile)
	g_quest_poster[e]['range'] = range
	g_quest_poster[e]['questprompt'] = questprompt
	g_quest_poster[e]['questscreen'] = questscreen
	g_quest_poster[e]['questchoice'] = questchoice
	g_quest_poster[e]['spawn_quest_object'] = spawn_quest_object
	g_quest_poster[e]['prompt_display'] = prompt_display
	g_quest_poster[e]['item_highlight'] = item_highlight	
	g_quest_poster[e]['highlight_icon'] = highlight_icon_imagefile
	g_quest_poster[e]['questtitle'] = ""
	g_quest_poster[e]['questtype'] = ""
	g_quest_poster[e]['questobject'] = ""
	g_quest_poster[e]['questreceiver'] = ""
	g_quest_poster[e]['questlevel'] = ""
	g_quest_poster[e]['questpoints'] = ""
	g_quest_poster[e]['questvalue'] = ""
	g_quest_poster[e]['questactivate'] = ""
	g_quest_poster[e]['queststarted'] = 0
	g_quest_poster[e]['questquantity'] = ""
	g_quest_poster[e]['questendtext'] = "Quest Completed"
end

function quest_poster_init(e)
	g_quest_poster[e] = {}
	g_quest_poster[e]['range'] = 100
	g_quest_poster[e]['questprompt'] = "Press E to view this quest"
	g_quest_poster[e]['questscreen'] = "HUD Screen 8"
	g_quest_poster[e]['questchoice'] = ""
	g_quest_poster[e]['spawn_quest_object'] = 1
	g_quest_poster[e]['prompt_display'] = 1
	g_quest_poster[e]['item_highlight'] = 0	
	g_quest_poster[e]['highlight_icon'] = "imagebank\\icons\\pickup.png"

	quest_objno[e] = 0
	quest_recno[e] = 0
	quest_quantity[e] = 0
	check_quantity[e] = 0
	check_timer[e] = 0
	play_once[e] = 0
	doonce[e] = 0
	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
	g_ActivateQuestComplete = 0
	SetEntityAlwaysActive(e,1)
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function quest_poster_main(e)

	if status[e] == "init" then
		if g_quest_poster[e]['item_highlight'] == 3 and g_quest_poster[e]['highlight_icon'] ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(g_quest_poster[e]['highlight_icon']))
			hl_imgwidth[e] = GetImageWidth(LoadImage(g_quest_poster[e]['highlight_icon']))
			hl_imgheight[e] = GetImageHeight(LoadImage(g_quest_poster[e]['highlight_icon']))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	if g_quest_poster[e]['questtitle'] == "" then
		local totalquests = GetCollectionQuestQuantity()
		if totalquests ~= nil then
			local i = tonumber(g_quest_poster[e]['questchoice'])-1
			if i ~= nil then
				if i > 0 and i <= totalquests then
					g_quest_poster[e]['questtitle'] = GetCollectionQuestAttribute(i,"title")
					g_quest_poster[e]['questtype'] = GetCollectionQuestAttribute(i,"type")
					g_quest_poster[e]['questobject'] = GetCollectionQuestAttribute(i,"object")
					g_quest_poster[e]['questreceiver'] = GetCollectionQuestAttribute(i,"receiver")
					g_quest_poster[e]['questlevel'] = GetCollectionQuestAttribute(i,"level")
					g_quest_poster[e]['questpoints'] = GetCollectionQuestAttribute(i,"points")
					g_quest_poster[e]['questvalue'] = GetCollectionQuestAttribute(i,"value")
					g_quest_poster[e]['questactivate'] = GetCollectionQuestAttribute(i,"activate")
					g_quest_poster[e]['questquantity'] = tonumber(GetCollectionQuestAttribute(i,"quantity"))
				end
			else
				PromptDuration(g_quest_poster[e]['questchoice'],5000)
			end
		end
	end
	if g_UserGlobalQuestTitleActive == nil then g_UserGlobalQuestTitleActive = "" end
	if g_UserGlobalQuestTitleActive ~= nil then
		if g_quest_poster[e]['queststarted'] == 0 then
			Show(e)
			for tquestindex = 1, hud0_quest_qty, 1 do
				if GetCollectionQuestAttribute(tquestindex,"title") == g_quest_poster[e]['questtitle'] then
					if hud0_quest_status[tquestindex] == "active" then
						g_quest_poster[e]['queststarted'] = 1
					end
					break
				end
			end
			local PlayerDist = GetPlayerDistance(e)
			if PlayerDist < g_quest_poster[e]['range'] then 
				--pinpoint select object--
				module_misclib.pinpoint(e,g_quest_poster[e]['range'],g_quest_poster[e]['item_highlight'],hl_icon[e])
				tEnt[e] = g_tEnt
				--end pinpoint select object--	
			end
		
			if PlayerDist < g_quest_poster[e]['range'] and tEnt[e] == e and GetEntityVisibility(e) == 1 then
				if g_quest_poster[e]['prompt_display'] == 1 then PromptLocal(e,g_quest_poster[e]['questprompt']) end
				if g_quest_poster[e]['prompt_display'] == 2 then Prompt(g_quest_poster[e]['questprompt']) end			
				if g_KeyPressE == 1 then
					-- set game to this quest
					g_UserGlobalQuestTitleShowing = g_quest_poster[e]['questtitle']
					g_UserGlobalQuestTitleShowingObject = g_quest_poster[e]['questobject']
					g_UserGlobalQuestTitleShowingObject2 = g_quest_poster[e]['questreceiver']
					ScreenToggle(g_quest_poster[e]['questscreen'])
					if play_once[e] == 0 then
						PlaySound(e,0)
						play_once[e] = 1
					end
				end
			end
		else
			Hide(e)
			for tquestindex = 1, hud0_quest_qty, 1 do
				if GetCollectionQuestAttribute(tquestindex,"title") == g_quest_poster[e]['questtitle'] then
					if hud0_quest_status[tquestindex] == "inactive" then
						g_quest_poster[e]['queststarted'] = 0
					end
					break
				end
			end
		end
		if g_quest_poster[e]['questtitle'] == g_UserGlobalQuestTitleActive then
			-- hide in game, we have accepted this one and doing it now
			if g_UserGlobalQuestTitleActiveE > 0 then
				local tquestcomplete = 0
				
				if g_quest_poster[e]['queststarted'] == 1 then
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

				if g_quest_poster[e]['questtype'] == "activate" then
					-- ACTIVATE
					if quest_objno[e] == 0 then						
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_poster[e]['questobject']) then
									quest_objno[e] = a									
									break
								end
							end
						end
					end
					if g_ActivateQuestComplete == 1 then tquestcomplete = 1 end
				end					

				if g_quest_poster[e]['questtype'] == "collect" then
					-- COLLECT
					if quest_objno[e] == 0 then						
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_poster[e]['questobject']) then
									quest_objno[e] = a
									break
								end
							end
						end
					end
					----Complete Collect Single Item Quest ----------------------
					if GetEntityCollected(quest_objno[e]) == 1 and g_quest_poster[e]['questquantity'] == 1 then					
						tquestcomplete = 1							
					end
					---- Complete Collect Multi Items Quest ----------------------
					if g_quest_poster[e]['questquantity'] > 1 then
						if g_Time > check_timer[e] then
							check_timer[e] = check_timer[e] + 500  -- check once per 1/2second			
							quest_quantity[e] = 0
							for ee = 1, g_EntityElementMax, 1 do
								if GetEntityCollected(ee) == 1 then
									if lower(GetEntityName(ee)) == lower(g_quest_poster[e]['questobject']) then
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
						if tcurrentcount >= g_quest_poster[e]['questquantity'] then
							tcurrentcount = g_quest_poster[e]['questquantity']
							tquestcomplete = 1
						end					
						if quest_quantity[e] > check_quantity[e] then PromptDuration(tcurrentcount.. " of " ..g_quest_poster[e]['questquantity'].. " " ..g_quest_poster[e]['questobject'].. "'s collected",2000) end
						check_quantity[e] = quest_quantity[e]
					end
				end
				
				if g_quest_poster[e]['questtype'] == "destroy" then
					-- DESTROY
					if quest_objno[e] == 0 then
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_poster[e]['questobject']) then
								quest_objno[e] = a
								break
								end
							end
						end
					end						
					if g_Entity[quest_objno[e]]['health'] <= 0 then
						tquestcomplete = 1
					end
				end
				
				if g_quest_poster[e]['questtype'] == "deliver" then
					-- DELIVER
					if quest_objno[e] == 0 then
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_poster[e]['questobject']) then
								quest_objno[e] = a								
								break
								end
							end
						end
					end
					if quest_recno[e] == 0 then
						for b = 1, g_EntityElementMax do
							if b ~= nil and g_Entity[b] ~= nil then
								if lower(GetEntityName(b)) == lower(g_quest_poster[e]['questreceiver']) then
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
					PlayNon3DSound(e,1)
					PromptDuration(g_quest_poster[e]['questendtext'],2000)
					-- award value
					local myMoney = 0
					if _G["g_UserGlobal['".."MyMoney".."']"] ~= nil then myMoney = _G["g_UserGlobal['".."MyMoney".."']"] end
					_G["g_UserGlobal['".."MyMoney".."']"] = myMoney + tonumber(g_quest_poster[e]['questvalue'])
					-- award XP
					local myXP = 0
					if _G["g_UserGlobal['".."MyXP".."']"] ~= nil then myXP = _G["g_UserGlobal['".."MyXP".."']"] end
					_G["g_UserGlobal['".."MyXP".."']"] = myXP + tonumber(g_quest_poster[e]['questpoints'])
					-- activate another object
					for ee = 1, g_EntityElementMax do
						if ee ~= nil and g_Entity[ee] ~= nil then
							if GetEntityName(ee) ~= nil then
								if string.lower(GetEntityName(ee)) == string.lower(g_quest_poster[e]['questactivate']) then
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

