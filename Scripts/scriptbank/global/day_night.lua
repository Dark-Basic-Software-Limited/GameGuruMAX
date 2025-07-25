-- Day_Night v20 by Necrym59 and Lee
-- DESCRIPTION: A global behavior to allow for a Day/Night time cycler
-- DESCRIPTION: [#START_ANGLE=-95(-180,180)]
-- DESCRIPTION: [TIME_DILATION=1(1,1000)]
-- DESCRIPTION: [MIN_AMBIENCE_R=10(1,255)]
-- DESCRIPTION: [MIN_AMBIENCE_G=25(1,255)]
-- DESCRIPTION: [MIN_AMBIENCE_B=75(1,255)]
-- DESCRIPTION: [MIN_EXPOSURE#=0.20(0.01,1.00)]
-- DESCRIPTION: [#SUN_ROLL=-95(-180,180)]
-- DESCRIPTION: [#SUN_PITCH=70(-180,180)]
-- DESCRIPTION: [#SUN_YAW=0(-180,180)]
-- DESCRIPTION: [MIN_INTENSITY#=3.45(0.01,50.00)]
-- DESCRIPTION: [MAX_AMBIENCE_R=255(1,255)]
-- DESCRIPTION: [MAX_AMBIENCE_G=255(1,255)]
-- DESCRIPTION: [MAX_AMBIENCE_B=255(1,255)]
-- DESCRIPTION: [MAX_EXPOSURE#=1.00(0.01,1.00)]
-- DESCRIPTION: [MAX_INTENSITY#=7.40(0.01,50.00)]
-- DESCRIPTION: [@TRIGGER_EVENT_A=27(1=1am,2=2am,3=3am,4=4am,5=5am,6=6am,7=7am,8=8am,9=9am,10=10am,11=11am,12=12am,13=1pm,14=2pm,15=3pm,16=4pm,17=5pm,18=6pm,19=7pm,20=8pm,21=9pm,22=10pm,23=11pm,24=12pm,25=7 Days,26=28 Days, 27=None)]
-- DESCRIPTION: [@TRIGGER_EVENT_B=27(1=1am,2=2am,3=3am,4=4am,5=5am,6=6am,7=7am,8=8am,9=9am,10=10am,11=11am,12=12am,13=1pm,14=2pm,15=3pm,16=4pm,17=5pm,18=6pm,19=7pm,20=8pm,21=9pm,22=10pm,23=11pm,24=12pm,25=7 Days,26=28 Days, 27=None)]
-- DESCRIPTION: [@START_DAY=1(1=Sunday, 2=Monday, 3=Tuesday, 4=Wednesday, 5=Thursday, 6=Friday, 7=Saturday)]
-- DESCRIPTION: [@@READOUT_USER_GLOBAL$=""(0=globallist)] User Global for displaying day and time (eg: MyUserGlobal)
-- DESCRIPTION: [LIGHT_CONTROL!=0] Control day/night lights
-- DESCRIPTION: [LIGHT_NAME$="Light"] Name of Light(s) to turn on/off
-- DESCRIPTION: [LIGHT_RANGE=300(1,5000)] Strength of light
-- DESCRIPTION: [DIAGNOSTICS!=0]


local lower = string.lower
g_sunrollposition = {}
g_updatedposition = {}

local day_night = {}
local start_angle = {}
local sun_roll = {}
local sun_pitch = {}
local sun_yaw = {}
local time_dilation = {}
local diagnostics = {}
local min_ambience_r = {}
local min_ambience_g = {}
local min_ambience_b = {}
local min_exposure = {}
local min_intensity = {}
local max_ambience_r = {}
local max_ambience_g = {}
local max_ambience_b = {}
local max_exposure = {}
local max_intensity = {}
local trigger_event_a = {}
local trigger_event_b = {}
local start_day = {}
local readout_user_global = {}

local suntimer = {}
local sunmoonroll = {}
local sunmoonpitch = {}
local sunmoonyaw = {}
local ambrvalue = {}
local ambgvalue = {}
local ambbvalue = {}
local expovalue = {}
local sintvalue = {}
local ambrvaluem = {}
local ambgvaluem = {}
local ambbvaluem = {}
local expovaluem = {}
local sintvaluem = {}

local light_control = {}
local light_name = {}
local light_range = {}
local dolightsoff = {}
local dolightson = {}

local status = {}
local lightlist = {}
local lightNum = {}
local state = {}
local tod = {}
local currentdaytime = {}
local changeday = {}
local daycount = {}
local weekcount = {}
local mode = {}
local event_trig = {}

function day_night_properties(e, start_angle, time_dilation, min_ambience_r, min_ambience_g, min_ambience_b, min_exposure, sun_roll, sun_pitch, sun_yaw, min_intensity, max_ambience_r, max_ambience_g, max_ambience_b, max_exposure, max_intensity, trigger_event_a, trigger_event_b, start_day, readout_user_global, light_control, light_name, light_range, diagnostics)
	-- start_angle in legacy version now replaced with RPY below but retained for compatability
	day_night[e].start_angle = start_angle
	day_night[e].time_dilation = time_dilation
	day_night[e].min_ambience_r = min_ambience_r
	day_night[e].min_ambience_g = min_ambience_g
	day_night[e].min_ambience_b = min_ambience_b
	day_night[e].min_exposure = min_exposure
	if sun_roll == nil then sun_roll = start_angle end
	day_night[e].sun_roll = sun_roll
	if sun_pitch == nil then sun_pitch = 75 end
	day_night[e].sun_pitch = sun_pitch
	if sun_yaw == nil then sun_yaw = 0 end
	day_night[e].sun_yaw = sun_yaw
	if min_intensity == nil then min_intensity = 3.4 end
	day_night[e].min_intensity = min_intensity
	if max_ambience_r == nil then max_ambience_r = 255 end
	day_night[e].max_ambience_r = max_ambience_r
	if max_ambience_g == nil then max_ambience_g = 255 end
	day_night[e].max_ambience_g = max_ambience_g
	if max_ambience_b == nil then max_ambience_b = 255 end
	day_night[e].max_ambience_b = max_ambience_b
	if max_exposure == nil then max_exposure = 1.00 end
	day_night[e].max_exposure = max_exposure
	if max_intensity == nil then max_intensity = 7.4 end
	day_night[e].max_intensity = max_intensity
	if trigger_event_a == nil then trigger_event_a = 25 end
	day_night[e].trigger_event_a = trigger_event_a
	day_night[e].trigger_event_b = trigger_event_b	
	day_night[e].start_day = start_day
	day_night[e].readout_user_global = readout_user_global
	day_night[e].light_control = light_control or 0
	day_night[e].light_name = light_name
	day_night[e].light_range = light_range
	day_night[e].diagnostics = diagnostics	
end

function day_night_init(e)
	day_night[e] = {}
	day_night[e].start_angle = -95
	day_night[e].sun_roll = -95
	day_night[e].sun_pitch = 75
	day_night[e].sun_yaw = 0
	day_night[e].time_dilation = 1
	day_night[e].min_ambience_r = 0
	day_night[e].min_ambience_g = 0
	day_night[e].min_ambience_b = 0
	day_night[e].min_exposure = 0.00
	day_night[e].min_intensity = 3.4
	day_night[e].max_ambience_r = 255
	day_night[e].max_ambience_g = 255
	day_night[e].max_ambience_b = 255
	day_night[e].max_exposure = 1.00
	day_night[e].max_intensity = 7.4
	day_night[e].trigger_event_a = 27
	day_night[e].trigger_event_b = 27
	day_night[e].start_day = 1
	day_night[e].readout_user_global = ""
	day_night[e].light_control = 1
	day_night[e].light_name = ""
	day_night[e].light_range = 300
	day_night[e].diagnostics = 1	

	status[e] = "init"
	state[e] = ""
	g_sunrollposition = 0
	g_updatedposition = 0
	sunmoonroll[e] = 0
	sunmoonpitch[e] = 0
	sunmoonyaw[e] = 0
	suntimer[e] = math.huge
	sintvalue[e] = 0
	event_trig[e] = 0
	currentdaytime[e] = 0
	mode[e] = ""
	tod[e] = ""
	changeday[e] = 0
	daycount[e] = 0
	weekcount[e] = 0
	dolightsoff[e] = 0
	dolightson[e] = 0
	lightNum[e] = 0
	Hide(e)
end

function day_night_main(e)

	if status[e] == "init" then
		ambrvalue[e] = day_night[e].min_ambience_r
		ambgvalue[e] = day_night[e].min_ambience_g
		ambbvalue[e] = day_night[e].min_ambience_b
		expovalue[e] = day_night[e].min_exposure
		sintvalue[e] = day_night[e].min_intensity
		if sunmoonroll[e] > 0 then
			ambrvalue[e] = day_night[e].max_ambience_r
			ambgvalue[e] = day_night[e].max_ambience_g
			ambbvalue[e] = day_night[e].max_ambience_b
			expovalue[e] = day_night[e].max_exposure
			sintvalue[e] = day_night[e].max_intensity
		end
		if day_night[e].time_dilation >= 1000 then day_night[e].time_dilation = 1000 end
		sunmoonroll[e] = day_night[e].start_angle
		if sunmoonroll[e] < -90 then mode[e] = "Night" end
		if sunmoonroll[e] >= -90 then mode[e] = "Day" end
		if sunmoonroll[e] >= 90 then mode[e] = "Night" end
		if sunmoonroll[e] < 0 and sunmoonroll[e] > -90 then state[e] = "dawn" end
		if sunmoonroll[e] > 0 and sunmoonroll[e] < 80 then state[e] = "ambient" end
		if sunmoonroll[e] >= 80 then state[e] = "dusk" end
		if sunmoonroll[e] >= 90 then state[e] = "dark" end
		sunmoonpitch[e] = day_night[e].sun_pitch
		sunmoonyaw[e] = day_night[e].sun_yaw
		suntimer[e] = g_Time + 1000
		SetSunDirection(sunmoonroll[e],sunmoonpitch[e],sunmoonyaw[e])
		--Check for Lights --
		if day_night[e].light_control == 1 and day_night[e].light_name > "" then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then				
					if lower(GetEntityName(n)) == lower(day_night[e].light_name) then
						table.insert(lightlist,n)
					end
				end
			end
		end
		status[e] = "endinit"
	end
	
	if g_Time > suntimer[e] then
		sunmoonroll[e] = (sunmoonroll[e] + 0.0042) --1 Sec = 0.0042 deg
		SetSunDirection(sunmoonroll[e],sunmoonpitch[e],sunmoonyaw[e])				
		g_sunrollposition = sunmoonroll[e]
		
		if g_updatedposition > 0 then			
			sunmoonroll[e] = (g_sunrollposition + g_updatedposition)
			g_updatedposition = 0
		end
		
		suntimer[e] = g_Time + 1000 / day_night[e].time_dilation
	end

	if mode[e] == "Night" then
		SetSunLightingColor(255,255,255)
		SetSunIntensity(sintvalue[e])
		SetExposure(expovalue[e])
		SetAmbienceRed(ambrvalue[e])
		SetAmbienceGreen(ambgvalue[e])
		SetAmbienceBlue(ambbvalue[e])
		SetAmbienceIntensity(120)
		--Swap in Moon Image if possible
		--if sunmoonroll[e] > 180 then sunmoonroll[e] = -180 end
		if sunmoonroll[e] > 165 then sunmoonroll[e] = -165 end
		--Turn to Day Mode
		if sunmoonroll[e] >= -90 then mode[e] = "Day" end
	end

	if mode[e] == "Day" then
		SetSunLightingColor(255,255,255)
		SetSunIntensity(sintvalue[e])
		SetExposure(expovalue[e])
		SetAmbienceRed(ambrvalue[e])
		SetAmbienceGreen(ambgvalue[e])
		SetAmbienceBlue(ambbvalue[e])
		SetAmbienceIntensity(120)
		if sunmoonroll[e] < 0 and sunmoonroll[e] > -90 then state[e] = "dawn" end
		if sunmoonroll[e] > -60 and sunmoonroll[e] < 80 then state[e] = "ambient" end
		if sunmoonroll[e] > 80 then state[e] = "dusk" end
		if sunmoonroll[e] > 90 then state[e] = "dark" end

		if state[e] == "dawn" then
			if ambrvalue[e] < day_night[e].max_ambience_r then ambrvalue[e] = ambrvalue[e] + 0.2 end
			if ambgvalue[e] < day_night[e].max_ambience_g then ambgvalue[e] = ambgvalue[e] + 0.2 end
			if ambbvalue[e] < day_night[e].max_ambience_b then ambbvalue[e] = ambbvalue[e] + 0.2 end
			if expovalue[e] < day_night[e].max_exposure then expovalue[e] = expovalue[e] + 0.0004 end
			if sintvalue[e] < day_night[e].max_intensity then sintvalue[e] = sintvalue[e] + 0.001 end
		end
		if state[e] == "ambient" then
			ambrvalue[e] = day_night[e].max_ambience_r
			ambgvalue[e] = day_night[e].max_ambience_g
			ambbvalue[e] = day_night[e].max_ambience_b
			expovalue[e] = day_night[e].max_exposure
			sintvalue[e] = day_night[e].max_intensity
		end
		if state[e] == "dusk" then
			if ambrvalue[e] > day_night[e].min_ambience_r then ambrvalue[e] = ambrvalue[e] - 0.2 end
			if ambgvalue[e] > day_night[e].min_ambience_g then ambgvalue[e] = ambgvalue[e] - 0.2 end
			if ambbvalue[e] > day_night[e].min_ambience_b then ambbvalue[e] = ambbvalue[e] - 0.1 end
			if expovalue[e] > day_night[e].min_exposure then expovalue[e] = expovalue[e] - 0.0002 end
			if sintvalue[e] > day_night[e].min_intensity then sintvalue[e] = sintvalue[e] - 0.001 end
		end
		--Turn to Night Mode
		if sunmoonroll[e] >= 90 then mode[e] = "Night" end
	end

	if sunmoonroll[e] <= -165.5 and changeday[e] == 1 then
		changeday[e] = 0
	end
	if sunmoonroll[e] > -165.5 then
		tod[e] = "12am"
	end
	if sunmoonroll[e] > -150.0 then
		tod[e] = "1am"
		if day_night[e].trigger_event_a == 1 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 1 then event_trig[e] = 1 end		
	end
	if sunmoonroll[e] > -135.0 then
		tod[e] = "2am"
		if day_night[e].trigger_event_a == 2 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 2 then event_trig[e] = 1 end			
	end
	if sunmoonroll[e] > -120.0 then
		tod[e] = "3am"
		if day_night[e].trigger_event_a == 3 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 3 then event_trig[e] = 1 end			
	end
	if sunmoonroll[e] > -105.0 then
		tod[e] = "4am"
		if day_night[e].trigger_event_a == 4 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 4 then event_trig[e] = 1 end			
	end
	if sunmoonroll[e] > -90.0 then
		tod[e] = "5am"
		if day_night[e].trigger_event_a == 5 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 5 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > -75.0 then
		tod[e] = "6am"
		if day_night[e].trigger_event_a == 6 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 6 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > -60.0 then
		tod[e] = "7am"
		if day_night[e].trigger_event_a == 7 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 7 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > -50.0 then
		tod[e] = "8am"
		if day_night[e].trigger_event_a == 8 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 8 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > -45.0 then
		tod[e] = "9am"
		if day_night[e].trigger_event_a == 9 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 9 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > -30.0 then
		tod[e] = "10am"
		if day_night[e].trigger_event_a == 10 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 10 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > -15.0 then
		tod[e] = "11am"
		if day_night[e].trigger_event_a == 11 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 11 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] >= 0 then
		tod[e] = "12pm"
		if day_night[e].trigger_event_a == 12 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 12 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 15.0 then
		tod[e] = "1pm"
		if day_night[e].trigger_event_a == 13 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 13 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 30.0 then
		tod[e] = "2pm"
		if day_night[e].trigger_event_a == 14 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 14 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 45.0 then
		tod[e] = "3pm"
		if day_night[e].trigger_event_a == 15 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 15 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 50.0 then
		tod[e] = "4pm"
		if day_night[e].trigger_event_a == 16 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 16 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 60.0 then
		tod[e] = "5pm"
		if day_night[e].trigger_event_a == 17 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 17 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 75.0 then
		tod[e] = "6pm"
		if day_night[e].trigger_event_a == 18 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 18 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 90.0 then
		tod[e] = "7pm"
		if day_night[e].trigger_event_a == 19 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 19 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 105.0 then
		tod[e] = "8pm"
		if day_night[e].trigger_event_a == 20 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 20 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 120.0 then
		tod[e] = "9pm"
		if day_night[e].trigger_event_a == 21 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 21 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 135.0 then
		tod[e] = "10pm"
		if day_night[e].trigger_event_a == 22 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 22 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 150.0 then
		tod[e] = "11pm"
		if day_night[e].trigger_event_a == 23 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 23 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] > 165.5 then
		tod[e] = "12am"
		if day_night[e].trigger_event_a == 24 then event_trig[e] = 1 end
		if day_night[e].trigger_event_b == 24 then event_trig[e] = 1 end
	end
	if sunmoonroll[e] >= 165.5 and changeday[e] == 0 then
		day_night[e].start_day = day_night[e].start_day + 1
		if day_night[e].start_day > 7 then day_night[e].start_day = 1 end
		if day_night[e].start_day > 7 then weekcount[e] = 1 end
		daycount[e] = daycount[e] + 1
		changeday[e] = 1
	end
	if day_night[e].trigger_event_a == 25 or day_night[e].trigger_event_b == 25 and weekcount[e] == 1 then event_trig[e] = 1 end
	if day_night[e].trigger_event_a == 26 or day_night[e].trigger_event_b == 26 and daycount[e] == 28 then event_trig[e] = 1 end
	
	if day_night[e].start_day == 1 then currentdaytime[e] = ("Sunday  " ..tod[e]) end
	if day_night[e].start_day == 2 then currentdaytime[e] = ("Monday  " ..tod[e]) end
	if day_night[e].start_day == 3 then currentdaytime[e] = ("Tuesday  " ..tod[e]) end
	if day_night[e].start_day == 4 then currentdaytime[e] = ("Wednesday  " ..tod[e]) end
	if day_night[e].start_day == 5 then currentdaytime[e] = ("Thursday  " ..tod[e]) end
	if day_night[e].start_day == 6 then currentdaytime[e] = ("Friday  " ..tod[e]) end
	if day_night[e].start_day == 7 then currentdaytime[e] = ("Saturday  " ..tod[e]) end

	if _G["g_UserGlobal['"..day_night[e].readout_user_global.."']"] ~= nil or day_night[e].readout_user_global ~= "" then
		_G["g_UserGlobal['"..day_night[e].readout_user_global.."']"] = currentdaytime[e]
	end

	if event_trig[e] == 1 then
		ActivateIfUsed(e)
		PerformLogicConnections(e)
		if daycount[e] == 28 then daycount[e] = 0 end
		if weekcount[e] == 1 then weekcount[e] = 0 end
		event_trig[e] = 0
	end

	--LIGHTS-----------------------------------------------------------------------------
	if day_night[e].light_control == 1 then
		if g_sunrollposition > -90 and g_sunrollposition < 84 then  --Day
			if dolightsoff[e] == 0 then
				for a,b in pairs (lightlist) do
					SetLightRange(b,0)
					SetActivated(b,0)
				end 
			end
			dolightsoff[e] = 1
			dolightson[e] = 0
		end		
		if g_sunrollposition > 85 then  --Night
			SetActivated(e,1)
			if dolightson[e] == 0 then
				for a,b in pairs (lightlist) do
					SetLightRange(b,day_night[e].light_range)
					SetActivated(b,1)
				end
			end	
			dolightson[e] = 1
			dolightsoff[e] = 0
		end
	end
	-------------------------------------------------------------------------------------

	if day_night[e].diagnostics == 1 then
		Text(1,22,3,"Day/Time: " ..currentdaytime[e])
		Text(1,24,3,"Sun/Moon Angle: " ..math.floor(sunmoonroll[e]))
		Text(1,26,3,"Time Mode: " ..mode[e])
		Text(1,28,3,"Dialation: " ..day_night[e].time_dilation)
		Text(1,30,3,"State: " ..state[e])
		Text(1,32,3,"Ambience R: " ..math.floor(ambrvalue[e]))
		Text(1,34,3,"Ambience G: " ..math.floor(ambgvalue[e]))
		Text(1,36,3,"Ambience B: " ..math.floor(ambbvalue[e]))
		Text(1,38,3,"Exposure: " ..math.floor(expovalue[e]*100)/100)
		Text(1,40,3,"Intensity: " ..math.floor(sintvalue[e]*100)/100)
	end
end

function day_night_exit(e)
end