-- Hidden v3 by Necrym59
-- DESCRIPTION: A global behavior that will hide all the same named objects in-game.
-- DESCRIPTION: All objects to be hidden must be set dynamic eg: Physics ON, IsImobile ON
-- DESCRIPTION: [OBJECT_NAME$=""]
-- DESCRIPTION: [@COLLISION_MODE=1(1=On, 2=Off)] on or off.
-- DESCRIPTION: [@NAVMESH_BLOCK=1(1=On, 2=Off)] on or off.

local lower = string.lower
local hidden 			= {}
local object_name 		= {}
local collision_mode 	= {}

local status			= {}
local objEnt			= {}

function hidden_properties(e, object_name, collision_mode, navmesh_block)
	hidden[e] = g_Entity[e]
	hidden[e].object_name = lower(object_name)
	hidden[e].collision_mode = collision_mode
	hidden[e].navmesh_block = navmesh_block
end 

function hidden_init_name(e)
	hidden[e] = {}
	hidden[e].object_name = ""
	hidden[e].collision_mode = 1
	hidden[e].navmesh_block = 1
	status[e] = "init"
end

function hidden_main(e)
	
	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == hidden[e].object_name then					
					Hide(n)
					if hidden[e].collision_mode == 1 then CollisionOn(n) end
					if hidden[e].collision_mode == 2 then CollisionOff(n) end					
					if hidden[e].navmesh_block == 1 then
						local x,y,z = GetEntityPosAng(n)
						y = RDGetYFromMeshPosition(x,y,z)
						local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[n]['obj'])
						local sx, sy, sz = GetObjectScales(g_Entity[n]['obj'])
						local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
						local sizex = w
						local sizez = l
						local angle = GetEntityAngleY(n)
						local blockmode = 1					
						RDBlockNavMeshWithShape(x,y,z,w,1,l,angle)
					end					
				end
			end
		end
		status[e] = "endinit"	
	end
end