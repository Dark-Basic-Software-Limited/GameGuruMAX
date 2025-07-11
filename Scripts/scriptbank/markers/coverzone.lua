-- Cover Zone v5 by Lee and Necrym59
-- DESCRIPTION: Cover zone for NPCs to find and use.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [@CoverType=1(1=Standing, 2=Crouched, 3=Prone)]

g_coverzone 		= {}
local spawnatstart 	= {}
local covertype		= {}
local status 		= {}

function coverzone_properties(e, spawnatstart, covertype)
	g_coverzone[e].spawnatstart = spawnatstart or 1
	g_coverzone[e].covertype = covertype or 1
end

function coverzone_init(e)
	g_coverzone[e] = {}
	g_coverzone[e].spawnatstart = 1
	g_coverzone[e].covertype = 1
	g_coverzone[e].status = "init"
end

function coverzone_main(e)
	if g_coverzone[e].status == "init" then
		if g_coverzone[e].spawnatstart == 1 then
			SetActivated(e,1)
			g_coverzone[e].status = "normal"
			
			-- now for the clever bit, work out best defender positions around cover zone area
			g_coverzone[e].defendermap = {}
			local basex = g_Entity[ e ]['x']
			local basey = g_Entity[ e ]['y'] + 5
			local basez = g_Entity[ e ]['z']
			local defendermapindex = 0
			for zz = -300, 300, 20 do
				for xx = -300, 300, 20 do
					if RDIsWithinAndOverMesh(basex+xx,basey,basez+zz) == 1 then
						if IsPointWithinZone(e,basex+xx,basey,basez+zz) == 1 then
							local navmeshyposhere = RDGetYFromMeshPosition(basex+xx,basey,basez+zz)
							if navmeshyposhere < basey then
								local tr = 50
								local techniquetype = 1
								if techniquetype == 1 then
									-- this technique allocates ONE cover position that provides BEST coverage for a SINGLE direction
									local fromx = basex+xx
									local fromy = basey
									local fromz = basez+zz
									local foundbestcount = 0
									local foundlastbestrealangle = 0
									local foundbestrealangle = 0
									for aa = 0, 15, 1 do
										local tox = fromx
										local toy = fromy
										local toz = fromz
										local realangle = aa*22.5
										tox = tox + math.sin(math.rad(realangle))*tr
										toz = toz + math.cos(math.rad(realangle))*tr
										hit = IntersectStaticPerformant(fromx,fromy,fromz,tox,toy,toz,0,0,0,0)
										if hit > 0 then -- for terrain ~= 0
											foundbestcount = foundbestcount + 1
											foundlastbestrealangle = foundbestrealangle
											foundbestrealangle = realangle
										end
									end
									local tblocktype = 1
									if foundbestcount > 1 then
										-- test if can stand here
										local tox = fromx
										local toy = fromy
										local toz = fromz
										local realangle = foundlastbestrealangle
										tox = tox + math.sin(math.rad(realangle))*tr
										toz = toz + math.cos(math.rad(realangle))*tr
										local higher = 60
										hit = IntersectStaticPerformant(fromx,fromy+higher,fromz,tox,toy+higher,toz,0,0,0,0)
										if hit > 0 then
											-- with peekability to the left
											realangle = foundlastbestrealangle - 90
											local sidestepdist = 40
											local sidefromx = fromx + math.sin(math.rad(realangle))*sidestepdist
											local sidefromz = fromz + math.cos(math.rad(realangle))*sidestepdist
											realangle = foundlastbestrealangle
											local lookviewdist=200
											tox = sidefromx + math.sin(math.rad(realangle))*lookviewdist
											toz = sidefromz + math.cos(math.rad(realangle))*lookviewdist
											hit = IntersectStaticPerformant(sidefromx,fromy+higher,sidefromz,tox,toy+higher,toz,0,0,0,0)
											if hit == 0 then
												tblocktype = 2
											end
										end
										if hit > 0 then
											-- with peekability to the right
											realangle = foundlastbestrealangle + 90
											local sidestepdist = 40
											local sidefromx = fromx + math.sin(math.rad(realangle))*sidestepdist
											local sidefromz = fromz + math.cos(math.rad(realangle))*sidestepdist
											realangle = foundlastbestrealangle
											local lookviewdist=200
											tox = sidefromx + math.sin(math.rad(realangle))*lookviewdist
											toz = sidefromz + math.cos(math.rad(realangle))*lookviewdist
											hit = IntersectStaticPerformant(sidefromx,fromy+higher,sidefromz,tox,toy+higher,toz,0,0,0,0)
											if hit == 0 then
												if tblocktype == 2 then
													if math.random(1,2)==1 then tblocktype = 3 end
												else
													tblocktype = 3
												end
											end
										end
										DoTokenDrop(fromx,fromy,fromz,1,4000 + (tblocktype*2000))
										defendermapindex=defendermapindex+1
										g_coverzone[e].defendermap[defendermapindex] = {}
										g_coverzone[e].defendermap[defendermapindex].x = fromx
										g_coverzone[e].defendermap[defendermapindex].y = fromy
										g_coverzone[e].defendermap[defendermapindex].z = fromz
										g_coverzone[e].defendermap[defendermapindex].a = foundlastbestrealangle
										g_coverzone[e].defendermap[defendermapindex].blocktype = tblocktype
									end
								end
							end
						end
					end
				end
			end
			g_coverzone[e].defendermapmax = defendermapindex			
		end
		if g_coverzone[e].spawnatstart == 0 then
			SetActivated(e,0)
			g_coverzone[e].status = "deactivated"
		end
	end

	if g_Entity[e]['activated'] == 1 then g_coverzone[e].status = "normal" end
	if g_Entity[e]['activated'] == 0 then g_coverzone[e].status = "deactivated" end
	
	if g_coverzone[e].status == "normal" then
		-- cover zone welcomes visitors
	end	
		
	if g_EntityExtra[e]['restoremenow'] ~= nil then
		if g_EntityExtra[e]['restoremenow'] == 1 then
			g_EntityExtra[e]['restoremenow'] = 0
			g_coverzone[e].status = "init"
		end
	end	
end