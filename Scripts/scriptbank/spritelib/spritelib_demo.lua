-- Spritelib_Demo v1 by Bolt-Action Gaming
-- DESCRIPTION: While the player is within the zone, spritelib demo runs.
-- DESCRIPTION: [USER_RANGE=150] controls how close the player needs to be,
-- DESCRIPTION: [CURSOR_ASSET$="imagebank\\HUD\\cursor.png"] is used for the cursor,
-- DESCRIPTION: [IMAGE_BASE$="imagebank\\HUD\\spritelib\\"] specifies the grouped images,
-- DESCRIPTION: [IMAGE_SET$="set1"] specifies the specific set to use.

local spritelib = require "scriptbank\\spritelib"

g_spritelib = {}

local zone_status       = {}
local demo_sprite       = {}
local demo_cursor       = {} 
local dialogue_box      = {} 
local gauge_mask        = {} 
local compass_strip     = {}
local altimeter_strip   = {}

local current_step      = {}
local step_start_time   = {}
local initialized       = {}

local hud_btn1 = {}
local hud_btn2 = {}
local hud_btn3 = {}

local current_demo_page = 1
local max_demo_pages    = 5
local last_page_state   = 0 
local last_j_state      = 0
local last_k_state      = 0
local q_key_pressed     = false

function spritelib_demo_properties(e, user_range, cursor_asset, image_base, image_set)
    g_spritelib[e]['user_range'] = user_range or 150
    g_spritelib[e]['cursor_asset'] = cursor_asset or "imagebank\\HUD\\cursor.png"
    g_spritelib[e]['image_base'] = image_base or "imagebank\\HUD\\spritelib\\"
    g_spritelib[e]['image_set'] = image_set or "set1"
end

function spritelib_demo_init(e)
	g_spritelib[e] = {}
	g_spritelib[e]['user_range'] = 150
	g_spritelib[e]['cursor_asset'] = ""
	g_spritelib[e]['image_base'] = "" 
	g_spritelib[e]['image_set'] = "" 
    zone_status[e]     = "init"
    demo_sprite[e]     = nil
    demo_cursor[e]     = nil
    dialogue_box[e]    = nil
    gauge_mask[e]      = nil
    compass_strip[e]   = nil
    altimeter_strip[e] = nil
	hud_btn1[e] = nil
	hud_btn2[e] = nil
	hud_btn3[e] = nil
    current_step[e]    = 1
    step_start_time[e] = 0
    initialized[e]     = false
    q_key_pressed      = false
end

function spritelib_demo_main(e)

    if g_Entity[e]['plrinzone'] == 1 then
		
        if not initialized[e] then
            demo_cursor[e]  = spritelib.new(g_spritelib[e]['cursor_asset'], 3, -1)
			local relpath = g_spritelib[e]['image_base'] .. g_spritelib[e]['image_set'] .. "\\"
            demo_sprite[e]  = spritelib.new(relpath.."hp_bar.png", 12, 20) 
            dialogue_box[e] = spritelib.new_nineslice(relpath.."nineslice\\text_box", 2.5)
            gauge_mask[e]   = spritelib.new(relpath.."hudslice\\text_box.png", 30, 20)
            compass_strip[e]   = spritelib.new(relpath.."compass_strip.png", 8, 3)
            altimeter_strip[e] = spritelib.new(relpath.."altimeter_strip.png", 4, 10)
            hud_btn1[e] = spritelib.new(relpath.."hudslice\\singlebtn.png", 5, 2.5)
			hud_btn2[e] = spritelib.new(relpath.."hudslice\\doublebtn.png", 5, 2.5)
			hud_btn3[e] = spritelib.new(relpath.."hudslice\\togglebtn.png", 5, 2.5)
            step_start_time[e] = os.clock()
            initialized[e]     = true
        end

        if g_InKey == "q" or g_InKey == "Q" then
            if not q_key_pressed then
                q_key_pressed = true
                spritelib.set_mouse_state(not spritelib.get_mouse_state())
                g_InKey = ""
            end
        else
            q_key_pressed = false
        end

        if spritelib.get_mouse_state() then
            SetSpritePosition(g_Entity[e], 500, 500)
            demo_sprite[e]:park_offscreen()

            local k_pressed = (g_InKey == "k" or g_InKey == "K")
            local j_pressed = (g_InKey == "j" or g_InKey == "J")
            
            if k_pressed and last_k_state == 0 then
                current_demo_page = current_demo_page + 1
                if current_demo_page > max_demo_pages then current_demo_page = 1 end
                last_k_state = 1
            elseif not k_pressed then last_k_state = 0 end
            
            if j_pressed and last_j_state == 0 then
                current_demo_page = current_demo_page - 1
                if current_demo_page < 1 then current_demo_page = max_demo_pages end
                last_j_state = 1
            elseif not j_pressed then last_j_state = 0 end

            -- Content State Transition Gutter
            if current_demo_page ~= last_page_state then
                dialogue_box[e]:clear_buffer()
				dialogue_box[e]:set_pos(50.0, 50.0)
                dialogue_box[e]:set_size(40.0, 20.0)
                last_page_state = current_demo_page
                
                if current_demo_page == 5 then
                    dialogue_box[e]:park_offscreen()
                    -- CRITICAL: Instantly position parent window so it doesn't fly in from 500,500
                    gauge_mask[e]:set_pos(50.0, 50.0)
                else
                    gauge_mask[e]:park_offscreen()
                    compass_strip[e]:park_offscreen()
                    altimeter_strip[e]:park_offscreen()
					hud_btn1[e]:park_offscreen()
                    hud_btn2[e]:park_offscreen()
                    hud_btn3[e]:park_offscreen()
                end

                if current_demo_page == 1 then
                    dialogue_box[e]:set_title("DEMO 1: STANDARD MULTI-LINE LOGGING", 1)
                    dialogue_box[e]:set_alignment("left")
                    dialogue_box[e]:add_log("System operational baseline verified.", 1, 255, 255, 255)
                    dialogue_box[e]:add_log("Warning: Minor power cell depletion detected.", 1, 255, 165, 0)
                    dialogue_box[e]:add_log("Critically low structural integrity on Sector 7!", 1, 255, 0, 0)
                elseif current_demo_page == 2 then
                    dialogue_box[e]:set_title("DEMO 2: LOG ELEMENT MAX SIZE CLAMPING", 1)
                    dialogue_box[e]:set_alignment("center")
                    for i = 1, 25 do dialogue_box[e]:add_log("Telemetry Data Array Entry Iteration Track #" .. i, 1, 200, 200, 255) end
                elseif current_demo_page == 3 then
                    dialogue_box[e]:set_title("DEMO 3: CENTERED & RIGHT ALIGNMENT DESIGNATIONS", 3)
                    dialogue_box[e]:set_alignment("right")
                    dialogue_box[e]:add_log("Right-justified target monitoring row tracking...", 1, 240, 230, 140)
                    dialogue_box[e]:set_alignment("center")
                    dialogue_box[e]:add_log("--- CORE STRUCTURAL SECTION RE-CENTERED ---", 1, 255, 255, 255)
                elseif current_demo_page == 4 then
                    dialogue_box[e]:set_title("DEMO 4: CASCADING RPG TYPEWRITER LOG", 2)
                    dialogue_box[e]:set_alignment("left")
                    dialogue_box[e]:add_log("Static text string initialized solidly.", 1, 255, 255, 255)
                    dialogue_box[e]:add_log_typed("First line items crawling outward across the container matrix space...", 1, 30, 0, 255, 255)
                    dialogue_box[e]:add_log_typed("Second cascading typewriter line awaiting its sequence turn...", 1, 45, 255, 105, 180)
                end
            end

            local page_prompt = "PAGE [" .. current_demo_page .. "/" .. max_demo_pages .. "] | K: Next | J: Prev | Space: Skip Crawl | Q: Exit Menu"
            Prompt(page_prompt)

			if current_demo_page == 5 then
				-- Process drag/resize calculations on parent frame
				gauge_mask[e]:process_window_interaction(4,4)
				
				local current_w = gauge_mask[e].w
				local current_h = gauge_mask[e].h
				local half_pw = gauge_mask[e].w / 2.0
				local half_ph = gauge_mask[e].h / 2.0
				
				-- ========================================================================
				-- CONFIGURATION DECK: FINE-TUNE YOUR RAW LOCAL OFFSETS HERE (No scale math needed!)
				-- =========================================================================
				local COMPASS_X_BASE   = -half_pw + 6.0   -- Local X position relative to parent center
				local COMPASS_Y_BASE   = -2.0             -- Local Y position
				local ALTIMETER_X_BASE = half_pw - 5.0    -- Local X position
				local ALTIMETER_Y_BASE = -half_ph + 4.0   -- Local Y position
				
				local BUTTON_Y_OFFSET  = half_ph - 3.0    -- Local Y offset from center
				local BUTTON_X_SPACING = 7.0              -- Spacing distance between buttons
				-- =========================================================================
				
				-- 1. Animate moving instrument offsets locally
				local heading_offset = (os.clock() * 3.0) % 8.0
				local alt_offset     = (os.clock() * 1.5) % 6.0
				
				-- 2. Bind Parent-Child connections using raw local values
				compass_strip[e]:set_parent(gauge_mask[e], COMPASS_X_BASE + heading_offset, COMPASS_Y_BASE)
				altimeter_strip[e]:set_parent(gauge_mask[e], ALTIMETER_X_BASE, ALTIMETER_Y_BASE + alt_offset)
				
				-- 3. Bind Buttons Row using clean static offsets
				hud_btn1[e]:set_parent(gauge_mask[e], -BUTTON_X_SPACING, BUTTON_Y_OFFSET)
				hud_btn2[e]:set_parent(gauge_mask[e], 0,                 BUTTON_Y_OFFSET)
				hud_btn3[e]:set_parent(gauge_mask[e], BUTTON_X_SPACING,  BUTTON_Y_OFFSET)
				
				-- Process Execution Actions for Buttons
				local click1, mode1 = hud_btn1[e]:process_button()
				local click2, mode2 = hud_btn2[e]:process_button()
				local click3, mode3 = hud_btn3[e]:process_button()
				
				if click1 and (mode1 == "single") then PlaySound(e, 0) end
				if click2 and (mode2 == "double") then PlaySound(e, 1) ActivateIfUsed(e) end
				if click3 and (mode3 == "single") then
					hud_btn3[e].toggle_state = not hud_btn3[e].toggle_state
					gauge_mask[e]:set_rainbow(hud_btn3[e].toggle_state)
				end
				
				-- Priorities
				compass_strip[e].priority   = 1
				altimeter_strip[e].priority = 1
				gauge_mask[e].priority      = 10
				hud_btn1[e].priority        = 11
				hud_btn2[e].priority        = 11
				hud_btn3[e].priority        = 11
				
				-- Commit updates
				gauge_mask[e]:update()
				compass_strip[e]:update()
				altimeter_strip[e]:update()
				hud_btn1[e]:update()
				hud_btn2[e]:update()
				hud_btn3[e]:update()
				
				-- Overlay textual tooltips cleanly right over the screen layout positions
				local bx1, by1 = hud_btn1[e]:get_render_positions()
				local bx2, by2 = hud_btn2[e]:get_render_positions()
				local bx3, by3 = hud_btn3[e]:get_render_positions()

            else
                dialogue_box[e].priority = 10
                dialogue_box[e]:draw()
            end
            
            demo_cursor[e].priority = -1 -- -1 ensures it skips array sorting and stays on top of everything
            demo_cursor[e]:snap_to_mouse()
            demo_cursor[e]:update()
        else
            last_page_state = 0 
            Prompt("Press Q to open the Interactive Multi-Page UI Menu Dashboard.")
            
            dialogue_box[e]:park_offscreen()
		-- Standard page fallback cleanup
			gauge_mask[e]:park_offscreen()
			compass_strip[e]:park_offscreen()
			altimeter_strip[e]:park_offscreen()
			hud_btn1[e]:park_offscreen()
			hud_btn2[e]:park_offscreen()
			hud_btn3[e]:park_offscreen()

            local elapsed_seconds = os.clock() - step_start_time[e]
            local time_remaining = math.max(0, 5.0 - elapsed_seconds)
            local step = current_step[e]

            if step == 1 then
                Prompt("Step 1: Exponential Lerp Translation. [Time: " .. string.format("%.1f", time_remaining) .. "s]")
                demo_sprite[e]:slide_to(50.0, 50.0, 4.5) 
                if elapsed_seconds >= 5.0 then step_start_time[e] = os.clock() current_step[e] = 2 end
            elseif step == 2 then
                Prompt("Step 2: Linear Vector Slideline. [Time: " .. string.format("%.1f", time_remaining) .. "s]")
				if elapsed_seconds <= 5 and elapsed_seconds > 2.5 then 
					demo_sprite[e]:slide_to(50.0, 50.0, 2.5)
				elseif elapsed_seconds <= 2.49 and elapsed_seconds > .01 then 
					demo_sprite[e]:slide_to(25.0, 45.0, 2.5)
				end
				
                if elapsed_seconds >= 5.0 then step_start_time[e] = os.clock() current_step[e] = 3 end
            elseif step == 3 then
                Prompt("Step 3: Linear Color Cycling Routine. [Time: " .. string.format("%.1f", time_remaining) .. "s]")
                demo_sprite[e]:set_rainbow(true)
                if elapsed_seconds >= 5.0 then demo_sprite[e]:set_rainbow(false) step_start_time[e] = os.clock() current_step[e] = 4 end
			elseif step == 4 then
                Prompt("Step 4: Dynamic Alpha Fading Core. [Time: " .. string.format("%.1f", time_remaining) .. "s]")
                if elapsed_seconds <= 0.1 then
                    demo_sprite[e].a = 255
                    demo_sprite[e]:fade_to(0, 4.0)
                end
                if elapsed_seconds >= 5.0 then 
                    demo_sprite[e]:fade_to(255, 0) 
                    step_start_time[e] = os.clock() 
                    current_step[e] = 5 
                end
            elseif step == 5 then
                Prompt("Step 5: Impact Jiggle Simulation. [Time: " .. string.format("%.1f", time_remaining) .. "s]")
                if elapsed_seconds < 3.5 then if math.floor(elapsed_seconds * 10) % 15 == 0 then demo_sprite[e]:trigger_jiggle(4.0, 35.0) end end
                if elapsed_seconds >= 5.0 then step_start_time[e] = os.clock() current_step[e] = 6 end
            elseif step == 6 then
                Prompt("Step 6: Scale Modifiers & Rotation. [Time: " .. string.format("%.1f", time_remaining) .. "s]")
                local scale_wave = 1.0 + (math.cos(os.clock() * 4.0) * 0.5)
                demo_sprite[e]:set_scale(scale_wave, scale_wave)
                local spin_angle = (os.clock() * 90.0) % 360
                demo_sprite[e]:set_rotation(spin_angle)
                if elapsed_seconds >= 5.0 then
                    demo_sprite[e]:set_scale(1.0, 1.0) demo_sprite[e]:set_rotation(0)
                    step_start_time[e] = os.clock() current_step[e] = 1
                end
            end

            demo_sprite[e].priority = 5
            demo_sprite[e]:update()
            demo_cursor[e]:park_offscreen()
            demo_cursor[e]:update()
        end
    else
        if initialized[e] then
            spritelib.set_mouse_state(false)
            if demo_sprite[e]     then demo_sprite[e]:park_offscreen() end
            if demo_cursor[e]     then demo_cursor[e]:park_offscreen() end
            if dialogue_box[e]    then dialogue_box[e]:park_offscreen() end
            if gauge_mask[e]      then gauge_mask[e]:park_offscreen() end
            if compass_strip[e]   then compass_strip[e]:park_offscreen() end
            if altimeter_strip[e] then altimeter_strip[e]:park_offscreen() end
            initialized[e] = false
            q_key_pressed  = false
        end
    end
end