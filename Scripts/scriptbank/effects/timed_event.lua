-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Timed Event v8 - by Necrym59
-- DESCRIPTION: Will activate timed delayed linked events.
-- DESCRIPTION: Attach this behavior to an object. Link a zone or switch to activate this object.
-- DESCRIPTION: Link from this object to other entities for delayed single or multiple activation.
-- DESCRIPTION: [EVENT_TEXT$="Event Underway"] Text to display when event activated
-- DESCRIPTION: [EVENT_COUNT=1(1,1000)] Number of events
-- DESCRIPTION: [EVENT_DELAY=5(1,100)] in seconds
-- DESCRIPTION: [@VISIBILITY=2(1=Hide, 2=Show)]
-- DESCRIPTION: [AUTO_RESET!=0] Auto Reinitialise
-- DESCRIPTION: <Sound0> plays for each event

local timedevent 		= {}
local event_text 		= {}
local event_count		= {}
local event_delay		= {}
local visibility		= {}
local auto_reset		= {}
local wait 				= {}
local delaytime 		= {}
local eventcount		= {}
local status 			= {}
local doonece 			= {}

function timed_event_properties(e, event_text, event_count, event_delay, visibility, auto_reset)
	timedevent[e].event_text = event_text
	timedevent[e].event_count = event_count
	timedevent[e].event_delay = event_delay
	timedevent[e].visibility = visibility
	timedevent[e].auto_reset = auto_reset or 0
end

function timed_event_init(e)
	timedevent[e] = {}
	timedevent[e].event_text = ""
	timedevent[e].event_count = 1
	timedevent[e].event_delay = 5
	timedevent[e].visibility = 2
	timedevent[e].auto_reset = 0
	
	wait[e] = math.huge
	delaytime[e] = 0
	eventcount[e] = 0
	doonece[e] = 0
	status[e] = "init"
end

function timed_event_main(e)

	if status[e] == "init" then
		eventcount[e] = timedevent[e].event_count
		delaytime[e] = timedevent[e].event_delay * 1000
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if timedevent[e].visibility == 1 then
			Hide(e)
			CollisionOff(e)
		end
		if doonece[e] == 0 then
			wait[e] = g_Time + delaytime[e]
			doonece[e] = 1
			status[e] = 'process'
		end
	end
	if status[e] == 'process' then
		if g_Time > wait[e] then
			if eventcount[e] > 0 then
				PromptDuration(timedevent[e].event_text,1000)
				PlaySound(e,0)
				SetActivatedWithMP(e,201)
				PerformLogicConnections(e)
				eventcount[e] = eventcount[e]-1
				if eventcount[e] == 0 then
					status[e] = "finished"
				end
				wait[e] = g_Time + delaytime[e]
			end
		end
	end
	if status[e] == "finished" then
		if timedevent[e].auto_reset == 0 then
			SetActivated(e,0)
		end
		if timedevent[e].auto_reset == 1 then
			status[e] = "init"
			doonece[e] = 0
			wait[e] = g_Time + delaytime[e]
		end				
	end
end