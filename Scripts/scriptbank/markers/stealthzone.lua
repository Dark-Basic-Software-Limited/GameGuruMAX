-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Stealthzone v7 by Necrym59
-- DESCRIPTION: The player will be in Stealth mode while in this Zone
-- DESCRIPTION: Attach to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="In Stealth Zone"]
-- DESCRIPTION: [DISCOVERY_RANGE=80(50,2000)]
-- DESCRIPTION: [ICON_IMAGEFILE$="imagebank\\misc\\testimages\\stealthzone_icon.png"]
-- DESCRIPTION: [ICON_POSITION_X=90(0,100)]
-- DESCRIPTION: [ICON_POSITION_Y=80(0,100)]
-- DESCRIPTION: Zone Height [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [DISABLE_WEAPON!=0]

local stealthzone 		= {}
local prompt_text 		= {}
local discovery_range 	= {}
local icon_imagefile 	= {}
local icon_position_x 	= {}
local icon_position_y 	= {}
local zoneheight		= {}
local disable_weapon 	= {}

local shieldactive 		= {}
local stealthicon 		= {}
local entrange			= {}
local doonce			= {}
local tableName			= {}
local status 			= {}
local sp_imgwidth		= {}
local sp_imgheight		= {}

function stealthzone_properties(e, prompt_text, discovery_range, icon_imagefile, icon_position_x, icon_position_y, zoneheight)
	stealthzone[e].prompt_text = prompt_text
	stealthzone[e].discovery_range = discovery_range
	stealthzone[e].icon_imagefile = icon_imagefile
	stealthzone[e].icon_position_x = icon_position_x
	stealthzone[e].icon_position_y = icon_position_y
	stealthzone[e].zoneheight = zoneheight or 100
end

function stealthzone_init(e)
	stealthzone[e] = {}
	stealthzone[e].prompt_text = "In Stealth Zone"
	stealthzone[e].discovery_range = 30
	stealthzone[e].icon_imagefile = "imagebank\\misc\\testimages\\stealthzone_icon.png"
	stealthzone[e].icon_position_x = 90
	stealthzone[e].icon_position_y = 80
	stealthzone[e].zoneheight = 100
	shieldactive[e] = 0
	
	status[e] = "init"
	doonce[e] = 0
	entrange[e] = 0
	tableName[e] = "stealthzone" ..tostring(e)
	_G[tableName[e]] = {}
	stealthicon = CreateSprite(LoadImage(stealthzone[e].icon_imagefile))
	sp_imgwidth[e] = GetImageWidth(LoadImage(stealthzone[e].icon_imagefile))
	sp_imgheight[e] = GetImageHeight(LoadImage(stealthzone[e].icon_imagefile))	
	SetSpriteSize(stealthicon,-1,-1)
	SetSpriteDepth(stealthicon,1)
	SetSpriteColor(stealthicon,255,255,255,3)
	SetSpriteOffset(stealthicon,sp_imgwidth[e]/2.0, sp_imgheight[e]/2.0)
	SetSpritePosition(stealthicon,200,200)
end

function stealthzone_main(e)

	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if GetEntityAllegiance(n) == 0 then
					table.insert(_G[tableName[e]],n)
				end
			end
		end
		status[e] = "endinit"
	end

	if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+stealthzone[e].zoneheight then
		shieldactive[e] = 1
		if doonce[e] == 0 then
			PromptDuration(stealthzone[e].prompt_text,1000)
			doonce[e] = 1
		end
		if shieldactive[e] == 1 then
			SetSpriteColor(stealthicon,255,255,255,50)
			PasteSpritePosition(stealthicon,stealthzone[e].icon_position_x,stealthzone[e].icon_position_y)
		end
	end

	if g_Entity[e]['plrinzone'] == 0 then
		shieldactive[e] = 0
		if shieldactive[e] == 0 then SetSpritePosition(stealthicon,200,200) end
	end

	if shieldactive[e] == 1 then
		for _,v in pairs (_G[tableName[e]]) do
			if g_Entity[v] ~= nil then
				entrange[e] = math.ceil(GetFlatDistanceToPlayer(v))
				if g_Entity[v]["health"] > 0 and entrange[e] > stealthzone[e].discovery_range then
					SetEntityAllegiance(v,2)
				end
				if g_Entity[v]["health"] > 0 and entrange[e] < stealthzone[e].discovery_range then
					SetEntityAllegiance(v,0)
				end
			end
		end
	end

	if shieldactive[e] == 0 then
		for _,v in pairs (_G[tableName[e]]) do
			if g_Entity[v] ~= nil then
				SetEntityAllegiance(v,0)
			end
		end
	end
end

function GetFlatDistanceToPlayer(v)
	if g_Entity[v] ~= nil then
		local distDX = g_PlayerPosX - g_Entity[v]['x']
		local distDZ = g_PlayerPosZ - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end



