-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- ImagePanel v13 by Necrym59 and Lee
-- DESCRIPTION: Will show an image panel with overlays activated from a trigger zone, switch or range.
-- DESCRIPTION: Attach to an object. Always active ON then link to a trigger zone or switch or set a range.
-- DESCRIPTION: [USE_RANGE=80(0,100)],[PROMPT_TEXT$="E to use"]
-- DESCRIPTION: Display [POSITION_X=2(0,100)],[POSITION_Y=10(0,100)][SCALE=50(1,100)],[@ASPECT_RATIO=1(1=On, 2=Off)]
-- DESCRIPTION: Set [OVERLAYS=4(1,8)][#OVERLAY_TIME=2.0(0.1,10.0)] [#DISPLAY_TIME=12.0(0.1,100)]
-- DESCRIPTION: [IMAGE_BASE$="imagebank\\misc\\testimages\\"],[IMAGE_SET$="set1"],
-- DESCRIPTION: [@REVIEWABLE=1(1=No, 2=Yes)] Yes on switch or range only
-- DESCRIPTION: <Sound0> for overlays
-- DESCRIPTION: <Sound1> for voice overs

local imagepanel 	= {}
local use_range 	= {}
local prompt_text 	= {}
local position_x 	= {}
local position_y 	= {}
local scale			= {}
local aspect_ratio	= {}
local overlays		= {}
local overlay_time	= {}
local display_time	= {}
local image_base	= {}
local image_set		= {}
local reviewable	= {}

local image_time 	= {}
local status 		= {}
local overlayed1	= {}
local overlayed2	= {}
local overlayed3	= {}
local overlayed4	= {}
local overlayed5	= {}
local overlayed6	= {}
local overlayed7	= {}
local overlayed8	= {}
local overlayed9	= {}
local displayed 	= {}
local pressed 		= {}
local timeset 		= {}
local imagebase	    = {}
local imagef1       = {}
local imagef2       = {}
local imagef3       = {}
local imagef4       = {}
local imagef5       = {}
local imagef6       = {}
local imagef7       = {}
local imagef8       = {}
	
function imagepanel_properties(e,use_range, prompt_text, position_x, position_y, scale, aspect_ratio, overlays, overlay_time, display_time, image_base, image_set, reviewable)
	imagepanel[e] = g_Entity[e]
	imagepanel[e].use_range = use_range
	imagepanel[e].prompt_text = prompt_text
	imagepanel[e].position_x = position_x
	imagepanel[e].position_y = position_y
	imagepanel[e].scale = scale
	imagepanel[e].aspect_ratio = aspect_ratio
	imagepanel[e].overlays = overlays
	imagepanel[e].overlay_time = overlay_time
	imagepanel[e].display_time = display_time
	imagepanel[e].image_base = image_base
	imagepanel[e].image_set = image_set	
	imagepanel[e].reviewable = reviewable
end
 
function imagepanel_init(e)
	imagepanel[e] = g_Entity[e]
	imagepanel[e].use_range = 80
	imagepanel[e].prompt_text = "E to use"
	imagepanel[e].position_x = 50
	imagepanel[e].position_y = 50
	imagepanel[e].scale = 50
	imagepanel[e].aspect_ratio = 1
	imagepanel[e].overlays = 3
	imagepanel[e].overlay_time = 2.0
	imagepanel[e].display_time = 5.0
	imagepanel[e].image_base =	"imagebank\\misc\\testimages\\"
	imagepanel[e].image_set = "set1"
	imagepanel[e].reviewable = 1
	image_time[e] = 0	
	status[e] = "init"
	pressed[e] = 0
	timeset[e] = 0
	imagebase[e] = 0	
	imagef1[e] = 0
	imagef2[e] = 0
	imagef3[e] = 0
	imagef4[e] = 0
	imagef5[e] = 0
	imagef6[e] = 0
	imagef7[e] = 0
	imagef8[e] = 0
end
 
function imagepanel_main(e)	
	imagepanel[e] = g_Entity[e]
	if status[e] == "init" then
		--Base Panel--
		imagebase[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\panel.png"))
		if imagepanel[e].aspect_ratio == 1 then aspectratio = GetImageHeight(imagepanel[e]) / GetImageWidth(imagepanel[e]) end
		if imagepanel[e].aspect_ratio == 2 then aspectratio = 1 end
		SetSpriteSize(imagebase[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
		SetSpriteDepth(imagebase[e],100)
		SetSpritePosition(imagebase[e],1000,1000)
		--Overlays--
		if imagepanel[e].overlays > 0 then
			imagef1[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\overlay1.png"))
			SetSpriteSize(imagef1[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
			SetSpriteDepth(imagef1[e],99)
			SetSpritePosition(imagef1[e],1000,1000)		
		end
		if imagepanel[e].overlays > 1 then
			imagef2[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\overlay2.png"))
			SetSpriteSize(imagef2[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
			SetSpriteDepth(imagef2[e],98)
			SetSpritePosition(imagef2[e],1000,1000)
		end			
		if imagepanel[e].overlays > 2 then
			imagef3[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\overlay3.png"))
			SetSpriteSize(imagef3[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
			SetSpriteDepth(imagef3[e],97)
			SetSpritePosition(imagef3[e],1000,1000)
		end
		if imagepanel[e].overlays > 3 then
			imagef4[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\overlay4.png"))
			SetSpriteSize(imagef4[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
			SetSpriteDepth(imagef4[e],96)
			SetSpritePosition(imagef4[e],1000,1000)
		end
		if imagepanel[e].overlays > 4 then
			imagef5[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\overlay5.png"))
			SetSpriteSize(imagef5[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
			SetSpriteDepth(imagef5[e],95)
			SetSpritePosition(imagef5[e],1000,1000)
		end
		if imagepanel[e].overlays > 5 then
			imagef6[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\overlay6.png"))
			SetSpriteSize(imagef6[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
			SetSpriteDepth(imagef6[e],94)
			SetSpritePosition(imagef6[e],1000,1000)
		end
		if imagepanel[e].overlays > 5 then
			imagef7[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\overlay7.png"))
			SetSpriteSize(imagef7[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
			SetSpriteDepth(imagef7[e],93)
			SetSpritePosition(imagef7[e],1000,1000)
		end
		if imagepanel[e].overlays > 5 then
			imagef8[e] = CreateSprite(LoadImage(imagepanel[e].image_base.. "" ..imagepanel[e].image_set.. "\\overlay8.png"))
			SetSpriteSize(imagef8[e],imagepanel[e].scale/aspectratio,imagepanel[e].scale)
			SetSpriteDepth(imagef8[e],92)
			SetSpritePosition(imagef8[e],1000,1000)
		end
		overlayed1[e] = 0		
		overlayed2[e] = 0
		overlayed3[e] = 0
		overlayed4[e] = 0
		overlayed5[e] = 0
		overlayed6[e] = 0
		overlayed7[e] = 0
		overlayed8[e] = 0
		overlayed9[e] = 0
		displayed[e] = 0
		status[e] = "display"
		StartTimer(e)
	end
	
	local PlayerDist = GetPlayerDistance(e)	
	if PlayerDist < imagepanel[e].use_range then
		local LookingAt = GetPlrLookingAtEx(e,1)		
		if LookingAt == 1 and pressed[e] == 0 then 
			Prompt(imagepanel[e].prompt_text)
			if g_KeyPressE == 1 and pressed[e] == 0 then pressed[e] = 1 end
			StartTimer(e)
		end
	end
	if g_Entity[e]['activated'] == 1 and pressed[e] == 0 then
		imagepanel[e].use_range = 0
		imagepanel[e].reviewable = 1
	end
	if g_Entity[e]['activated'] == 1 or pressed[e] == 1 then		
		
		if status[e] == "display" then	
			if GetTimer(e) >= 10 then 
				PasteSpritePosition(imagebase[e],imagepanel[e].position_x,imagepanel[e].position_y)				
			end
			if GetTimer(e) >= 10 and overlayed1[e] == 0 then
				PlaySound(e,0)
				PlaySound(e,1)
				overlayed1[e] = 1
			end			
			
			if GetTimer(e) >= imagepanel[e].overlay_time*1000 and imagepanel[e].overlays > 0 then 
				PasteSpritePosition(imagef1[e],imagepanel[e].position_x,imagepanel[e].position_y)	
			end
			if GetTimer(e) >= imagepanel[e].overlay_time*1000 and imagepanel[e].overlays > 0 and overlayed2[e] == 0 then
				PlaySound(e,0)
				overlayed2[e] = 1
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*2)*1000 and imagepanel[e].overlays > 1 then 
				PasteSpritePosition(imagef2[e],imagepanel[e].position_x,imagepanel[e].position_y)
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*2)*1000 and imagepanel[e].overlays > 1 and overlayed3[e] == 0 then
				PlaySound(e,0)
				overlayed3[e] = 1
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*3)*1000 and imagepanel[e].overlays > 2 then 
				PasteSpritePosition(imagef3[e],imagepanel[e].position_x,imagepanel[e].position_y)
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*3)*1000 and imagepanel[e].overlays > 2 and overlayed4[e] == 0 then
				PlaySound(e,0)
				overlayed4[e] = 1				
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*4)*1000 and imagepanel[e].overlays > 3 then 
				PasteSpritePosition(imagef4[e],imagepanel[e].position_x,imagepanel[e].position_y)
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*4)*1000 and imagepanel[e].overlays > 3 and overlayed5[e] == 0 then
				PlaySound(e,0)
				overlayed5[e] = 1				
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*5)*1000 and imagepanel[e].overlays > 4 then 
				PasteSpritePosition(imagef5[e],imagepanel[e].position_x,imagepanel[e].position_y)
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*5)*1000 and imagepanel[e].overlays > 4 and overlayed6[e] == 0 then
				PlaySound(e,0)
				overlayed6[e] = 1
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*6)*1000 and imagepanel[e].overlays > 5 then 
				PasteSpritePosition(imagef6[e],imagepanel[e].position_x,imagepanel[e].position_y)
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*6)*1000 and imagepanel[e].overlays > 5 and overlayed7[e] == 0 then
				PlaySound(e,0)
				overlayed7[e] = 1
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*7)*1000 and imagepanel[e].overlays > 6 then 
				PasteSpritePosition(imagef7[e],imagepanel[e].position_x,imagepanel[e].position_y)
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*7)*1000 and imagepanel[e].overlays > 6 and overlayed8[e] == 0 then
				PlaySound(e,0)
				overlayed8[e] = 1
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*8)*1000 and imagepanel[e].overlays > 7 then 
				PasteSpritePosition(imagef8[e],imagepanel[e].position_x,imagepanel[e].position_y)
			end
			if GetTimer(e) >= (imagepanel[e].overlay_time*8)*1000 and imagepanel[e].overlays > 7 and overlayed9[e] == 0 then
				PlaySound(e,0)
				overlayed9[e] = 1
			end
			
			---Timing----
			if image_time[e] == 0 then
				image_time[e] = GetTimer(e) + (imagepanel[e].display_time * 1000)			
			end
			if GetTimer(e) >= image_time[e] then
				status[e] = "end"
			end
		end
		
		if status[e] == "end" then			
			DeleteSprite(imagebase[e])
			if imagepanel[e].overlays > 0 then DeleteSprite(imagef1[e]) end
			if imagepanel[e].overlays > 1 then DeleteSprite(imagef2[e]) end
			if imagepanel[e].overlays > 2 then DeleteSprite(imagef3[e]) end
			if imagepanel[e].overlays > 3 then DeleteSprite(imagef4[e]) end
			if imagepanel[e].overlays > 4 then DeleteSprite(imagef5[e]) end
			if imagepanel[e].overlays > 5 then DeleteSprite(imagef6[e]) end
			if imagepanel[e].overlays > 6 then DeleteSprite(imagef7[e]) end
			if imagepanel[e].overlays > 7 then DeleteSprite(imagef8[e]) end
			g_Entity[e]['activated'] = 0			
			if imagepanel[e].reviewable == 1 then status[e] = "end" end
			if imagepanel[e].reviewable == 2 then
				status[e] = "init"
				pressed[e] = 0
			end	
		end
	end
	
	if g_Entity[e]['activated'] == 0 then		
	end
end --main 
 
function imagepanel_exit(e)
end


