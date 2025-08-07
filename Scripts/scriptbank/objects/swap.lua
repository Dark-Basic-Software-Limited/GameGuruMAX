-- Swap v6 by Necrym59
-- DESCRIPTION: A global behavior that will hide all the same named objects in-game and swap them out for another set of same named objects.
-- DESCRIPTION: Attach to an object Physics=ON. Activate by a linked switch or zone.
-- DESCRIPTION: NPC's can only be swapped once.
-- DESCRIPTION: [OBJECT_NAME1$=""] to swap out
-- DESCRIPTION: [OBJECT_NAME2$=""] to swap in
-- DESCRIPTION: [!NAVMESH_BLOCK=0]
-- DESCRIPTION: <Sound0> when swapping

local lower = string.lower
local swap 				= {}
local object_name1 		= {}
local object_name2 		= {}
local navmesh_block		= {}
local object_no1 		= {}
local object_no2 		= {}

local status			= {}
local doonce			= {}
local posx				= {}
local posy				= {}
local posz				= {}
local angx				= {}
local angy				= {}
local angz				= {}
local isnpc				= {}

function swap_properties(e, object_name1, object_name2, navmesh_block)
	swap[e].object_name1 = lower(object_name1)
	swap[e].object_name2 = lower(object_name2)
	swap[e].navmesh_block = navmesh_block or 0
end 

function swap_init(e)
	swap[e] = {}
	swap[e].object_name1 = ""
	swap[e].object_name2 = ""
	swap[e].navmesh_block = 0
	swap[e].object_n01 = 0
	swap[e].object_n02 = 0	
	
	status[e] = "init"
	doonce[e] = 0
	posx[e] = 0
	posy[e] = 0
	posz[e] = 0
	angx[e] = 0
	angy[e] = 0
	angz[e] = 0
	isnpc[e] = 0
end

function swap_main(e)
	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == swap[e].object_name1 then
					swap[e].object_n01 = n
				end	 
			end
		end
		for m = 1, g_EntityElementMax do
			if m ~= nil and g_Entity[m] ~= nil then
				if lower(GetEntityName(m)) == swap[e].object_name2 then
					swap[e].object_n02 = m
				end	 
			end
		end
		Hide(swap[e].object_n02)
		status[e] = "swapout"
	end
		
	if g_Entity[e]['activated'] == 1 and status[e] == "swapout" then
		if status[e] == "swapout" then
			if doonce[e] == 0 then
				PlaySound(e,0)
				doonce[e] = 1
			end
			---Swap out Object 1 ----
			posx[e],posy[e],posz[e],angx[e],angy[e],angz[e] = GetEntityPosAng(swap[e].object_n01)						
			Hide(swap[e].object_n01)
			CollisionOff(swap[e].object_n01)
			if GetEntityAllegiance(swap[e].object_n01) ~= -1 then
				isnpc[e] = 1
				SetEntityActive(swap[e].object_n01,0)
			end
			---Swap in Object 2 ----
			Spawn(swap[e].object_n02)
			ResetPosition(swap[e].object_n02,posx[e],posy[e],posz[e])
			ResetRotation(swap[e].object_n02,angx[e],angy[e],angz[e])						
			Show(swap[e].object_n02)
			CollisionOn(swap[e].object_n02)
			if GetEntityAllegiance(swap[e].object_n02) ~= -1 then
				isnpc[e] = 1
				SetEntityActive(swap[e].object_n02,1)
			end							
			if swap[e].navmesh_block == 1 then
				local x,y,z = GetEntityPosAng(swap[e].object_n02)
				y = RDGetYFromMeshPosition(x,y,z)
				local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[swap[e].object_n02]['obj'])
				local sx, sy, sz = GetObjectScales(g_Entity[swap[e].object_n02]['obj'])
				local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
				local sizex = w
				local sizez = l
				local angle = GetEntityAngleY(swap[e].object_n02)
				local blockmode = 1					
				RDBlockNavMeshWithShape(x,y,z,w,1,l,angle)
			end
			status[e] = "swapback"
		end
		doonce[e] = 0
		SetActivated(e,0)
	end
	
	if g_Entity[e]['activated'] == 1 and status[e] == "swapback" and isnpc[e] == 0 then
		if status[e] == "swapback" then
			if doonce[e] == 0 then
				PlaySound(e,0)
				doonce[e] = 1
			end	
			---Swap out Object 2 ----
			if swap[e].navmesh_block == 1 then
				local x,y,z = GetEntityPosAng(swap[e].object_n02)
				y = RDGetYFromMeshPosition(x,y,z)
				local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[swap[e].object_n02]['obj'])
				local sx, sy, sz = GetObjectScales(g_Entity[swap[e].object_n02]['obj'])
				local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
				local sizex = w
				local sizez = l
				local angle = GetEntityAngleY(swap[e].object_n02)
				local blockmode = 0
				RDBlockNavMeshWithShape(x,y,z,w,blockmode,l,angle)
			end
			posx[e],posy[e],posz[e],angx[e],angy[e],angz[e] = GetEntityPosAng(swap[e].object_n02)
			Hide(swap[e].object_n02)
			CollisionOff(swap[e].object_n02)
			SetEntityActive(swap[e].object_n02,1)
			---Swap in Object 1 ----
			if GetEntityAllegiance(swap[e].object_n01) ~= -1 then
				isnpc[e] = 1
				SetEntityActive(swap[e].object_n01,1)
			end			
			ResetPosition(swap[e].object_n01,posx[e],posy[e],posz[e])
			ResetRotation(swap[e].object_n01,angx[e],angy[e],angz[e])
			Show(swap[e].object_n01)
			CollisionOn(swap[e].object_n01)
			SetEntityActive(swap[e].object_n01,1)
			if swap[e].navmesh_block == 1 then
				local x,y,z = GetEntityPosAng(swap[e].object_n01)
				y = RDGetYFromMeshPosition(x,y,z)
				local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[swap[e].object_n01]['obj'])
				local sx, sy, sz = GetObjectScales(g_Entity[swap[e].object_n01]['obj'])
				local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
				local sizex = w
				local sizez = l
				local angle = GetEntityAngleY(swap[e].object_n01)
				local blockmode = 1					
				RDBlockNavMeshWithShape(x,y,z,w,blockmode,l,angle)
			end						
			status[e] = "swapout"
		end
		doonce[e] = 0
		SetActivated(e,0)
	end	
end