-- Imagezone v4 by Lee and Necrym59 and BoltActionGaming
-- DESCRIPTION: While the player is within the zone, the image set in [IMAGEFILE$=""] is displayed on screen.
-- DESCRIPTION: Set [Duration=4000] in milliseconds for how long the image is displayed.
-- DESCRIPTION: If [!ClickToExit=0] is set, the image will stop displaying after a mouse click is detected.
-- DESCRIPTION: Set [!Fullscreen=0] to display the image across the whole screen, or in 3D in your level.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered. 
-- DESCRIPTION: [!DisplayOnce=0] to display the image only once then destroy the zone upon leaving.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> plays a sound when entering the zone.

g_imageinzone = {}
g_imageinzone_soundplaying = 0
local status = {}

function imageinzone_properties(e, imagefile, duration, clicktoexit, fullscreen, zoneheight, displayonce, spawnatstart)
	g_imageinzone[e]['imagefile'] = imagefile
	g_imageinzone[e]['duration'] = duration
	g_imageinzone[e]['clicktoexit'] = clicktoexit or 0
	g_imageinzone[e]['fullscreen'] = fullscreen or 0
	g_imageinzone[e]['zoneheight'] = zoneheight
	g_imageinzone[e]['displayonce'] = displayonce or 0
	g_imageinzone[e]['spawnatstart'] = spawnatstart
	g_imageinzone[e]['starttime'] = g_Time	
end 

function imageinzone_init(e)
	g_imageinzone[e] = {}
	g_imageinzone[e]['imagefile'] = "" 
	g_imageinzone[e]['duration'] = 4000
	g_imageinzone[e]['clicktoexit'] = 0
	g_imageinzone[e]['active'] = 1
	g_imageinzone[e]['fullscreen'] = 0
	g_imageinzone[e]['zoneheight'] = 100
	g_imageinzone[e]['displayonce'] = 0
	g_imageinzone[e]['imageid'] = 0	
	g_imageinzone[e]['sprite'] = 0	
	g_imageinzone[e]['active'] = 1
	g_imageinzone[e]['spawnatstart'] = 1
	g_imageinzone[e]['starttime'] = g_Time
	g_imageinzone[e]['triggered'] = 0 -- Added tracker

	status[e] = "init"
end

function imageinzone_main(e)
	if status[e] == "init" then
		if g_imageinzone[e]['spawnatstart'] == 1 then SetActivated(e,1) end
		if g_imageinzone[e]['spawnatstart'] == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then	
		if g_imageinzone[e]['zoneheight'] == nil then g_imageinzone[e]['zoneheight'] = 100 end
		if g_imageinzone[e]['zoneheight'] ~= nil then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+65 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_imageinzone[e]['zoneheight'] then
				if g_imageinzone[e]['active'] == 1 then
					if g_imageinzone[e]['imageid'] == 0 then 
						local nImageFile = g_imageinzone[e]['imagefile']
						if nImageFile ~= "" then 
							g_imageinzone[e]['imageid'] = LoadImage(nImageFile)
						end 
						PerformLogicConnections(e)
						-- Record time when player entered zone so it can be used in the duration to display calculation.
						g_imageinzone[e]['starttime'] = g_Time
						g_imageinzone[e]['triggered'] = 1 -- Flag that the zone has successfully run once
					else 
						-- Display image for 'duration' milliseconds.
						if g_Time - g_imageinzone[e]['starttime'] < g_imageinzone[e]['duration'] then 
							if g_imageinzone[e]['fullscreen'] == 0 then
								PromptImage ( g_imageinzone[e]['imageid'] )
							else
								if g_imageinzone[e]['sprite'] == 0 then
									g_imageinzone[e]['sprite'] = CreateSprite ( g_imageinzone[e]['imageid'] )
								end
								if g_imageinzone[e]['sprite'] > 0 then
									SetSpriteDepth ( g_imageinzone[e]['sprite'], 100 )
									aspectratio = GetImageHeight(g_imageinzone[e]['imageid']) / GetImageWidth(g_imageinzone[e]['imageid'])
									SetSpriteSize ( g_imageinzone[e]['sprite'], 100, 100*aspectratio )
									SetSpriteOffset ( g_imageinzone[e]['sprite'], 50, 50*aspectratio )
									SetSpritePosition ( g_imageinzone[e]['sprite'], 50, 50  )
								end
							end
						else
							if g_imageinzone[e]['sprite'] > 0 then 
								DeleteSprite ( g_imageinzone[e]['sprite'] ) g_imageinzone[e]['sprite'] = 0 
							end
						end
						-- Play Sound0
						if g_imageinzone_soundplaying ~= e then
							PlaySoundIfSilent(e,0)
							g_imageinzone_soundplaying = e
						end
						if g_imageinzone[e]['clicktoexit'] == 1 and g_MouseClick ~= 0 then
							g_imageinzone[e]['active'] = 0
						end
					end 
				else 
					if g_imageinzone[e]['sprite'] > 0 then 
						DeleteSprite ( g_imageinzone[e]['sprite'] ) g_imageinzone[e]['sprite'] = 0 
					end
				end
			else
				if g_imageinzone[e]['sprite'] > 0 then 
					DeleteSprite ( g_imageinzone[e]['sprite'] ) g_imageinzone[e]['sprite'] = 0 
				end
				if g_imageinzone_soundplaying == e then
					g_imageinzone_soundplaying = 0
				end
				-- Reset 'starttime' so the image will display again when entering the zone a second time.
				g_imageinzone[e]['starttime'] = g_Time
				-- Resetting 'active' will ensure that the imagezone can be used on the next entry, even after clicking to close it.
				if g_imageinzone[e]['displayonce'] == 0 then g_imageinzone[e]['active'] = 1 end
				if g_imageinzone[e]['displayonce'] == 1 and g_imageinzone[e]['triggered'] == 1 then					g_imageinzone[e]['active'] = 0
					SetActivated(e,0)
					Destroy(e)
				end
			end
		end
	end	
end