-- spritelib.lua
local spritelib = {}
spritelib.__index = spritelib

local function lerp(start, stop, amt) return start + (stop - start) * amt end
local function clamp(val, min, max) return math.max(min, math.min(max, val)) end

local mouse_is_active = false
local last_click_state = 0 -- 0 = up, 1 = down

-- 1. INITIALIZE CONTAINER
function spritelib.new(image_path, base_w, base_h)
    local self = setmetatable({}, spritelib)
    
    self.image  = LoadImage(image_path)
    self.id     = CreateSprite(self.image)
    
    -- Transform States (0-100 Percentages)
    self.x      = 50
    self.y      = 50
    self.w = base_w or 5
    self.h = base_h or -1 
    self.angle  = 0
    self.depth  = 50
    self.priority = 0
    
    self.scale_x = 1.0
    self.scale_y = 1.0
    
    -- Sizing reference benchmarks for dynamic bounding boxes
    self.initial_w = self.w
    self.initial_h = self.h
    
    -- Window Interaction State Engines
    self.is_dragging = false
    self.is_resizing = false
    self.drag_offset_x = 0.0
    self.drag_offset_y = 0.0
    
    -- Color/Alpha States
    self.r, self.g, self.b, self.a = 255, 255, 255, 255
    
    -- Tweening Target Vectors
    self.target_x, self.target_y = self.x, self.y
    self.lerp_speed = 4.0 
    
    -- FX States
    self.fade_target = nil
    self.fade_speed  = 1.0
    self.rainbow     = false
    self.hue_timer   = 0
    
    -- Jiggle / Juice Variables
    self.jiggle_intensity = 0
    self.jiggle_speed     = 0
    self.jiggle_timer     = 0
    
    -- Progress / Scissor Clipping States
    self.clip_active = false
    self.window_x    = 50
    self.window_y    = 50
    self.window_w    = 5
    self.window_h    = 5
    
    -- Parent / Child Properties
    self.parent = nil
    self.rel_x  = 0
    self.rel_y  = 0
    
    self.debug_enabled = false
    self.last_clock = os.clock()
    
	local base_h = ((self.h == -1) and (self.w * 1.7778) or self.h)
	SetSpriteSize(self.id, self.w, base_h)
	SetSpriteOffset(self.id, self.w / 2.0, base_h / 2.0) -- <-- ADD THIS: Forces Center Pivot Alignment
	SetSpriteDepth(self.id, self.depth)
	self:park_offscreen()
    self.is_hovered   = false
    self.is_pressed   = false
    self.last_click_t = 0
    self.toggle_state = false
   
    return self
end

function spritelib:set_parent(parent_obj, offset_x, offset_y)
    self.parent = parent_obj
    self.rel_x = offset_x or 0.0
    self.rel_y = offset_y or 0.0
end

-- =========================================================================
-- INTERACTIVE BUTTON LOGIC ENGINE
-- =========================================================================
function spritelib:process_button()
    if not spritelib.get_mouse_state() then return false, nil end
    
    local current_time = os.clock()
    local mouse_over = self:is_mouse_over()
    self.is_hovered = mouse_over
    
    -- Visual Feedback: Hover states dim slightly, pressed states dim further
    if mouse_over then
        if g_MouseClick == 1 then
            self.is_pressed = true
            self:set_color(180, 180, 180) -- Darker click tint
        else
            -- Check for standard click-release action trigger
            if self.is_pressed then
                self.is_pressed = false
                self:set_color(220, 220, 220) -- Hover tint
                
                -- Determine Click Multiplicity (Double Click Threshold: 0.3 seconds)
                if (current_time - self.last_click_t) < 0.3 then
                    self.last_click_t = 0 -- Reset
                    return true, "double"
                else
                    self.last_click_t = current_time
                    return true, "single"
                end
            end
            self:set_color(220, 220, 220) -- Hover tint
        end
    else
        self.is_pressed = false
        self:set_color(255, 255, 255) -- Full natural brightness
    end
    
    return false, nil
end

-- =========================================================================
-- SYSTEM POINTER LAYER
-- =========================================================================
function spritelib.set_mouse_state(active)
    if active then
        if not mouse_is_active then
            FreezePlayer()
            ActivateMouse()
            mouse_is_active = true
        end
    else
        if mouse_is_active then
            UnFreezePlayer()
            DeactivateMouse()
            mouse_is_active = false
            last_click_state = 0
        end
    end
end

function spritelib.get_mouse_state() return mouse_is_active end

function spritelib.get_mouse_pos()
    if not mouse_is_active then return 50.0, 50.0 end
    return g_MouseX, g_MouseY
end

function spritelib.check_mouse_click()
    if not mouse_is_active then return false, nil end
    local current_click = g_MouseClick
    local triggered = false
    local mode = nil
    
    if current_click == 1 and last_click_state == 0 then
        triggered = true mode = "down" last_click_state = 1
    elseif current_click == 1 and last_click_state == 1 then
        triggered = true mode = "held"
    elseif current_click == 0 and last_click_state == 1 then
        triggered = true mode = "up" last_click_state = 0
    end
    return triggered, mode
end

function spritelib:is_mouse_over()
    if not mouse_is_active then return false end
    local mx, my = spritelib.get_mouse_pos()
    local rx, ry = self:get_render_positions()
    
    local parent_sx = self.parent and self.parent.scale_x or 1.0
    local parent_sy = self.parent and self.parent.scale_y or 1.0
    
    local final_w = self.w * self.scale_x * parent_sx
    
    -- FIXED: If self.h is provided explicitly, it still needs to be adjusted 
    -- by the widescreen aspect ratio to match the true physical layout height!
	local base_h  = ((self.h == -1) and (self.w * 1.7778) or (self.h * 1.7778))
    local final_h = base_h * self.scale_y * parent_sy
    
    -- Perfectly matched bounding box calculations around the center pivot
    local left   = rx - (final_w / 2.0)
    local right  = rx + (final_w / 2.0)
    local top    = ry - (final_h / 2.0)
    local bottom = ry + (final_h / 2.0)
    
    return (mx >= left and mx <= right and my >= top and my <= bottom)
end
-- =========================================================================
-- DRAG & RESIZE ENGINE EXTENSION
-- =========================================================================
function spritelib:process_window_interaction(header_px, resize_px)
    if not mouse_is_active then return end
    
    local mx, my = spritelib.get_mouse_pos()
    local sw = GetDeviceWidth()
    local sh = GetDeviceHeight()
    
    -- Dynamic conversion factors from absolute pixels to hardware percentage layouts
    local header_h_pct = (header_px / sh) * 100.0
    local resize_w_pct = (resize_px / sw) * 100.0
    local resize_h_pct = (resize_px / sh) * 100.0
    
	local final_w = self.w * self.scale_x
    local base_h = ((self.h == -1) and (self.w * 1.7778) or (self.h * 1.7778))
    local final_h = base_h * self.scale_y
    
    -- Explicitly derive top-left origin bounds from the center placement point
    local final_x = self.x - (final_w / 2)
    local final_y = self.y - (final_h / 2)
    
    local left   = self.x - (final_w / 2.0)
    local right  = self.x + (final_w / 2.0)
    local top    = self.y - (final_h / 2.0)
    local bottom = self.y + (final_h / 2.0)
    
    if g_MouseClick == 1 then
        if not self.is_dragging and not self.is_resizing then

--			if (mx >= (self.x - final_w / 2) and mx <= (self.x + final_w / 2)) and (my >= (self.y - final_h / 2) and my <= (self.y - final_h / 2) + (header_px * 1.7778)) then
--				self.is_dragging = true
--				self.drag_offset_x = mx - self.x
--				self.drag_offset_y = my - self.y
--			elseif (mx >= (self.x + final_w / 2) - resize_px and mx <= (self.x + final_w / 2)) and (my >= (self.y + final_h / 2) - (resize_px * 1.7778) and my <= (self.y + final_h / 2)) then
--				self.is_resizing = true
--			end
			if (mx >= (self.x - final_w / 2) and mx <= (self.x + final_w / 2)) and (my >= (self.y - final_h / 2) and my <= (self.y - final_h / 2) + (header_px * 1.7778)) then
				self.is_dragging = true
				self.drag_offset_x = mx - self.x
				self.drag_offset_y = my - self.y
			elseif (mx >= (self.x + final_w / 2) - resize_px and mx <= (self.x + final_w / 2)) and (my >= (self.y + final_h / 2) - (resize_px * 1.7778) and my <= (self.y + final_h / 2)) then
				self.is_resizing = true
				self.is_dragging = false
			end
		end
    else
        self.is_dragging = false
        self.is_resizing = false
    end
    
    -- Process Active Interactive Actions
    if self.is_dragging then
        self.target_x = mx - self.drag_offset_x
        self.target_y = my - self.drag_offset_y
        -- Enforce boundary clamping safely on dragged containers
        self.target_x = clamp(self.target_x, final_w / 2.0, 100.0 - (final_w / 2.0))
        self.target_y = clamp(self.target_y, final_h / 2.0, 100.0 - (final_h / 2.0))
    elseif self.is_resizing then
        local calculated_w = (mx - left)
        local calculated_h = (my - top)
        
        -- Map values smoothly into unified scaling multipliers clamped between +/- 25%
        if self.initial_w and self.initial_w > 0 then
            local target_scale_x = clamp(calculated_w / self.initial_w, 0.75, 1.25)
            local base_initial_h = ((self.initial_h == -1) and (self.initial_w * 1.7778) or self.initial_h)
            local target_scale_y = clamp(calculated_h / base_initial_h, 0.75, 1.25)
            self:set_scale(target_scale_x, target_scale_y)
        end
    end
end

-- =========================================================================
-- CORE OBJECT METHODS
-- =========================================================================
function spritelib:set_pos(x, y)
    self.x, self.y = x, y
    self.target_x, self.target_y = x, y
    return self
end

function spritelib:set_scale(sx, sy)
    self.scale_x = sx or 1.0
    self.scale_y = sy or sx or 1.0
    return self
end

function spritelib:set_exact_size(w, h)
    self.w = w self.h = h
    self.initial_w = w self.initial_h = h
    return self
end

function spritelib:set_color(r, g, b)
    self.r = clamp(r or 255, 0, 255)
    self.g = clamp(g or 255, 0, 255)
    self.b = clamp(b or 255, 0, 255)
    return self
end

function spritelib:set_debug(state)
    self.debug_enabled = not not state
    return self
end

function spritelib:set_rotation(angle) self.angle = angle or 0 return self end
function spritelib:slide_to(tx, ty, speed)
    self.target_x = tx self.target_y = ty
    self.lerp_speed = speed or 4.0 return self
end

function spritelib:snap_to_mouse()
    if mouse_is_active then
        self.x, self.target_x = g_MouseX, g_MouseX
        self.y, self.target_y = g_MouseY, g_MouseY
    end
    return self
end

function spritelib:fade_to(target_alpha, duration_seconds)
    local clamped_target = clamp(target_alpha, 0, 255)
    if self.fade_target == clamped_target then return self end
    self.fade_target = clamped_target
    local diff = math.abs(self.a - self.fade_target)
    self.fade_speed = duration_seconds > 0 and (diff / duration_seconds) or diff
    return self
end

function spritelib:set_rainbow(state)
    self.rainbow = state
    if not state then self.r, self.g, self.b = 255, 255, 255 end
    return self
end

function spritelib:trigger_jiggle(intensity, speed)
    self.jiggle_intensity = intensity or 2.0
    self.jiggle_speed = speed or 25.0
    self.jiggle_timer = 0
    return self
end

function spritelib:park_offscreen()
    self.x, self.y = 500, 500
    self.target_x, self.target_y = 500, 500
    SetSpritePosition(self.id, 500, 500)
    return self
end

function spritelib:set_scissor_window(enabled, wx, wy, ww, wh) -- currently nonfunctional
    self.clip_active = enabled
    if enabled then
        self.window_x = wx
        self.window_y = wy
        self.window_w = ww
        self.window_h = wh
    end
    return self
end

function spritelib:get_render_positions()
    if self.parent then
        -- Scale the relative offsets securely using the parent's uniform scale properties
        local parent_sx = self.parent.scale_x or 1.0
        local parent_sy = self.parent.scale_y or 1.0
        return self.parent.x + (self.rel_x * parent_sx), 
               self.parent.y + (self.rel_y * parent_sy)
    end
    return self.x, self.y
end
-- =========================================================================
-- CORE LIFECYCLE RUNTIME ENGINE LOOP
-- =========================================================================
function spritelib:update()
    if not self.id or self.id == 0 then return end
    local current_clock = os.clock()
    local true_dt = current_clock - self.last_clock
    self.last_clock = current_clock
    true_dt = math.min(true_dt, 0.1)
    
    -- 1. PARENT ATTACHMENT ANCHOR ENGINE (Must run BEFORE position processing)
    if self.parent then
        local lx = self.local_x or 0
        local ly = self.local_y or 0
        
        -- Set the targets linearly relative to parent center point 
        -- (No exponential compounding over frames!)
        self.target_x = self.parent.x + (lx * self.parent.scale_x)
        self.target_y = self.parent.y + (ly * self.parent.scale_y)
        
        -- Inherit uniform scaling factors downward from parent
        self.scale_x = self.parent.scale_x
        self.scale_y = self.parent.scale_y
    end
    
	-- 2. Clean, Linear Position Interpolation Math
    if self.x ~= self.target_x or self.y ~= self.target_y then
        -- FIX: If a player is manually grabbing and dragging/resizing the panel,
        -- bypass the frame interpolation to prevent the heavy "slingshotting/elastic lag".
        if self.is_dragging or self.is_resizing then
            self.x = self.target_x
            self.y = self.target_y
        else
            -- Keep the smooth exponential decay lerp for automated script slides and demos
            local t = 1 - math.exp(-self.lerp_speed * true_dt)
            self.x = lerp(self.x, self.target_x, t)
            self.y = lerp(self.y, self.target_y, t)
        end
    end
    
    -- Fade Computations
    if self.fade_target then
        if self.a < self.fade_target then
            self.a = self.a + (self.fade_speed * true_dt)
            if self.a >= self.fade_target then self.a = self.fade_target self.fade_target = nil end
        elseif self.a > self.fade_target then
            self.a = self.a - (self.fade_speed * true_dt)
            if self.a <= self.fade_target then self.a = self.fade_target self.fade_target = nil end
        end
    end
    
    -- Rainbow Cycle Engine
    if self.rainbow then
        self.hue_timer = self.hue_timer + true_dt * 3.0
        self.r = math.floor(math.sin(self.hue_timer) * 127 + 128)
        self.g = math.floor(math.sin(self.hue_timer + 2.094) * 127 + 128)
        self.b = math.floor(math.sin(self.hue_timer + 4.188) * 127 + 128)
    end
    
    -- Jiggle Render Modifiers
    local render_offset_x, render_offset_y = 0, 0
    if self.jiggle_intensity > 0 then
        self.jiggle_timer = self.jiggle_timer + true_dt * self.jiggle_speed
        render_offset_x = math.sin(self.jiggle_timer) * self.jiggle_intensity
        render_offset_y = math.cos(self.jiggle_timer * 1.5) * self.jiggle_intensity
        self.jiggle_intensity = self.jiggle_intensity - (true_dt * 3.5)
    end
    
    local final_alpha = clamp(math.floor(self.a + 0.5), 0, 255)
    SetSpriteColor(self.id, self.r, self.g, self.b, final_alpha)
    
    -- Cascading Matrix Computations (Combine scales natively)
    local parent_sx = self.parent and self.parent.scale_x or 1.0
    local parent_sy = self.parent and self.parent.scale_y or 1.0
    local final_scale_x = self.scale_x
    local final_scale_y = self.scale_y

    --local final_w = self.w * final_scale_x
    --local base_h = ((self.h == -1) and (self.w * 1.7778) or self.h)
    --local final_h = base_h * final_scale_y
	local final_w = self.w * final_scale_x
	local base_h = ((self.h == -1) and (self.w * 1.7778) or (self.h * 1.7778))
	local final_h = base_h * final_scale_y
    
    SetSpriteSize(self.id, final_w, final_h)
    SetSpriteOffset(self.id, final_w / 2.0, final_h / 2.0)

    local render_x, render_y = self:get_render_positions()
    local final_x = render_x + render_offset_x
    local final_y = render_y + render_offset_y

    SetSpritePosition(self.id, final_x, final_y)
    SetSpriteAngle(self.id, self.angle or 0)
    SetSpritePriority(self.id, self.priority or 0)
end

function spritelib:check_window_inputs(header_h_pct, resize_handle_w_pct)
    if not self.id or self.id == 0 then return end
    
    local final_w = self.w * self.scale_x
    local base_h = ((self.h == -1) and (self.w * 1.7778) or self.h)
    local final_h = base_h * self.scale_y
    
    local left   = self.x
    local right  = self.x + final_w
    local top    = self.y
    local bottom = self.y + final_h
    
    local header_bottom = top + (header_h_pct or 4.0)
    local mc_x, mc_y = spritelib.get_mouse_pos()
    local mouse_click = (g_MouseClick == 1)
    
    local rx_left = right - (resize_handle_w_pct or 2.0)
    local ry_top  = bottom - (resize_handle_w_pct or 2.0)
    
    -- State Locker: Check corner interaction
    if mouse_click and (mc_x >= rx_left and mc_x <= right and mc_y >= ry_top and mc_y <= bottom) then
        if not self.is_dragging then
            self.is_resizing = true
        end
    end

    -- 1. Continuous Delta Resizing Engine
    if self.is_resizing and mouse_click then
        if self.initial_mouse_x == 0 then
            -- First click anchor frame initialization
            self.initial_mouse_x = mc_x
            self.initial_mouse_y = mc_y
        else
            -- Calculate frame-to-frame movement delta shifts
            local delta_x = mc_x - self.initial_mouse_x
            local delta_y = mc_y - self.initial_mouse_y
            
            -- Translate screen movement increments smoothly into fractional scale shifts
            local scale_change_x = delta_x / (self.w or 1.0)
            local scale_change_y = delta_y / (base_h or 1.0)
            
            -- Accumulate adjustments progressively 
            self.scale_x = clamp(self.scale_x + scale_change_x, 0.2, 5.0)
            self.scale_y = clamp(self.scale_y + scale_change_y, 0.2, 5.0)
            
            -- CRITICAL: Instantly update coordinate anchors to clear cache historical drift
            self.initial_mouse_x = mc_x
            self.initial_mouse_y = mc_y
        end
        return
    else
        self.is_resizing = false
        self.initial_mouse_x = 0
        self.initial_mouse_y = 0
    end
    
    -- 2. Drag Tracker Engine
    if mouse_click then
        if not self.is_dragging then
            if mc_x >= left and mc_x <= right and mc_y >= top and mc_y <= header_bottom then
                self.is_dragging = true
                self.drag_offset_x = mc_x - self.x
                self.drag_offset_y = mc_y - self.y
            end
        else
            self.target_x = mc_x - self.drag_offset_x
            self.target_y = mc_y - self.drag_offset_y
        end
    else
        self.is_dragging = false
    end
end

function spritelib:destroy()
    if self.id and self.id > 0 then DeleteSprite(self.id) end
    self.id = nil
end

-- =========================================================================
-- ENCAPSULATED 9-IMAGE TEXT WINDOW & ROLLING MESSAGE CONTAINER
-- =========================================================================
local nineslice = {}
nineslice.__index = nineslice

local function get_slice_path(base_path, suffix)
    local pure_path = base_path:gsub("%.png$", ""):gsub("%.PNG$", "")
    return pure_path .. suffix .. ".png"
end

function spritelib.new_nineslice(base_image_path, edge_thickness_pct)
    local self = setmetatable({}, nineslice)
    
    self.thick = edge_thickness_pct or 2.5
    
    local suffixes = {
        "_tl", "_t", "_tr",
        "_l",  "_c", "_r",
        "_bl", "_b", "_br"
    }
    
    self.images  = {}
    self.sprites = {}
    
    for i, suffix in ipairs(suffixes) do
        local full_path = get_slice_path(base_image_path, suffix)
        self.images[i]  = LoadImage(full_path)
        self.sprites[i] = CreateSprite(self.images[i])
        SetSpritePriority(self.sprites[i], 1)
        SetSpritePosition(self.sprites[i], 500, 500)
    end
    
-- Positioning and Size Layout (0-100 Percentages)
    self.x = 50
    self.y = 50
    self.w = 40
    self.h = 20
    self.a = 255
    
    -- Interaction Limits
    self.min_w = 30.0    
    self.min_h = 15.0    
    self.resizing = false
    
	-- DRAG & DROP MOVEMENT TRACKING
    self.dragging = false
    self.drag_offset_x = 0.0
    self.drag_offset_y = 0.0
	
    -- ENCAPSULATED TEXT & SCROLL STORAGE ENGINE
    self.buffer = {}            
    self.scroll_index = 0       
    self.padding_x = 3.0        
    self.padding_y = 2.0        
    self.max_buffer_size = 150  
    
    -- NEW LOW-HANGING FRUIT ATTRIBUTES
    self.title_text = ""       -- Holds window title string
    self.title_size = 1        -- Font size assignment code for title
    self.alignment  = "left"   -- Choices: "left", "center", "right"
    
    return self
end

-- New Title Setter Method
function nineslice:set_title(text, font_size)
    self.title_text = tostring(text or "")
    self.title_size = font_size or 1
    return self
end

-- New Alignment Mode Setter Method
function nineslice:set_alignment(align_mode)
    local mode = tostring(align_mode):lower()
    if mode == "center" or mode == "right" then
        self.alignment = mode
    else
        self.alignment = "left"
    end
    return self
end

function nineslice:set_pos(x, y) self.x, self.y = x, y return self end
function nineslice:set_size(w, h) self.w, self.h = w, self.h return self end
function nineslice:set_alpha(alpha) self.a = math.max(0, math.min(255, alpha or 255)) return self end

-- Clear the active message text table instantly
function nineslice:clear_buffer()
    self.buffer = {}
    self.scroll_index = 0
    return self
end

-- Force change the safety cap ceiling dynamically
function nineslice:set_max_buffer(limit)
    self.max_buffer_size = tonumber(limit) or 150
    return self
end

-- Push new data lines natively with an explicit font size tag (Defaults to 1 if skipped)
function nineslice:add_log(text_string, font_size, r, g, b)
    local preferred_size = font_size or 1
    
    -- Bulletproof Typing: Explicitly check for nil or invalid types before casting
    local target_r = 255
    local target_g = 255
    local target_b = 255
    
    if r and g and b then
        target_r = math.floor(clamp(tonumber(r) or 255, 0, 255))
        target_g = math.floor(clamp(tonumber(g) or 255, 0, 255))
        target_b = math.floor(clamp(tonumber(b) or 255, 0, 255))
    end
    
    -- Construct the expanded encapsulated line entry object
    table.insert(self.buffer, {
        text = tostring(text_string),
        size = preferred_size,
        r = target_r,
        g = target_g,
        b = target_b
    })
    
    -- Rolling Memory Protection Ceiling
    if #self.buffer > self.max_buffer_size then
        table.remove(self.buffer, 1)
        if self.scroll_index > 0 then
            self.scroll_index = self.scroll_index - 1
        end
    end
    
    return self
end
-- Dynamically maps line height spacing footprint based on the font size being evaluated
function nineslice:get_line_height(font_size)
    -- Map your pixel-step constraints dynamically per font tier choice
    if font_size == 1 then return 2.8  -- Clean 18pt vertical spacing block
    elseif font_size == 2 then return 3.8
    elseif font_size == 3 then return 4.5
    else return 3.0 end
end

-- Capacity estimate helper (Uses font size 1 as baseline spacing target)
function nineslice:get_capacity()
    local usable_h = math.max(0, self.h - (self.thick * 2.0) - (self.padding_y * 2.0))
    return math.floor(usable_h / self:get_line_height(1))
end

function nineslice:process_interaction()
    local mx, my = g_MouseX, g_MouseY
    local half_w = self.w / 2.0
    local half_h = self.h / 2.0
    local left   = self.x - half_w
    local right  = self.x + half_w
    local top    = self.y - half_h
    local bottom = self.y + half_h
    
    local click_zone = 3.5 
    
    -- Evaluate the Resizing Drag Handles (Bottom-Right Corner)
    local inside_corner_x = (mx >= (right - click_zone) and mx <= (right + click_zone))
    local inside_corner_y = (my >= (bottom - click_zone) and my <= (bottom + click_zone))
    
    -- Evaluate Header Grab Handle Zone (Top edge down through border/padding thickness)
    -- This isolates a clear hit-test strip across the top of the container box
    local header_thickness = self.thick + 2.5
    local inside_header = (mx >= left and mx <= right and my >= top and my <= (top + header_thickness))

    if g_MouseClick == 1 then
        if not self.resizing and not self.dragging then
            if inside_corner_x and inside_corner_y then
                self.resizing = true
            elseif inside_header then
                self.dragging = true
                self.drag_offset_x = mx - self.x
                self.drag_offset_y = my - self.y
            end
        end
    else
        self.resizing = false
        self.dragging = false
    end
    
    -- Execute Active Resizing Transformations
    if self.resizing then
        self.w = math.max(self.min_w, math.abs(mx - self.x) * 2.0)
        self.h = math.max(self.min_h, math.abs(my - self.y) * 2.0)
    end
    
    -- Execute Active Window Drag Positions with Hard Screen Clamping
    if self.dragging then
        local target_x = mx - self.drag_offset_x
        local target_y = my - self.drag_offset_y
        
        -- Clamp boundaries to keep the window geometry from flying entirely off screen edges
        self.x = math.max(half_w, math.min(100.0 - half_w, target_x))
        self.y = math.max(half_h, math.min(100.0 - half_h, target_y))
    end
    
    -- Process Scroll Wheel Viewport Shifts
    if mx >= left and mx <= right and my >= top and my <= bottom then
        if g_MouseWheel ~= 0 then
            if g_MouseWheel > 0 then
                self.scroll_index = math.max(0, self.scroll_index - 1)
            elseif g_MouseWheel < 0 then
                self.scroll_index = self.scroll_index + 1
            end
        end
    end
end

-- New Feature Hook: Push a dynamic line that crawls across the display grid letter by letter
-- Syntax: object:add_log_typed("Text String", size, characters_per_sec, r, g, b)
function nineslice:add_log_typed(text_string, font_size, speed_chars_sec, r, g, b)
    local preferred_size = font_size or 1
    local speed = speed_chars_sec or 30.0 -- Default to 30 characters per second if skipped
    
    local target_r = 255
    local target_g = 255
    local target_b = 255
    if r and g and b then
        target_r = math.floor(clamp(tonumber(r) or 255, 0, 255))
        target_g = math.floor(clamp(tonumber(g) or 255, 0, 255))
        target_b = math.floor(clamp(tonumber(b) or 255, 0, 255))
    end
    
    -- Construct the entry object with typing state properties initialized
    table.insert(self.buffer, {
        text = tostring(text_string),
        size = preferred_size,
        r = target_r,
        g = target_g,
        b = target_b,
        -- Typewriter engine tags:
        is_typed = true,
        current_visible_len = 0.0, -- Tracks character length progress as a float accumulator
        cps = speed
    })
    
    if #self.buffer > self.max_buffer_size then
        table.remove(self.buffer, 1)
        if self.scroll_index > 0 then
            self.scroll_index = self.scroll_index - 1
        end
    end
    
    return self
end

function nineslice:check_typewriter_bypass()
    -- Look for standard interact/skip key maps
    local skip_pressed = (g_InKey == "e" or g_InKey == "E" or g_InKey == " ")
    
    if skip_pressed then
        -- Scan from front to back to find the FIRST line currently typing
        for i = 1, #self.buffer do
            local entry = self.buffer[i]
            if entry.is_typed and entry.current_visible_len < string.len(entry.text) then
                -- Force the character counter instantly to maximum length boundary
                entry.current_visible_len = string.len(entry.text)
                g_InKey = "" -- Flush key register immediately to prevent bouncing
                break
            end
        end
    end
end

function nineslice:draw()
    self:process_interaction()

    -- Calculate high-precision frametime delta using os.clock() to bypass g_TimeElapsed VSync issues
    local current_clock = os.clock()
    if not self.last_clock then self.last_clock = current_clock end
    local precision_dt = current_clock - self.last_clock
    self.last_clock = current_clock
    
    -- Safety clamp to prevent text jumping during sudden loading hitches or engine hiccups
    if precision_dt > 0.1 then precision_dt = 0.1 end

    local t = self.thick
    local half_w = self.w / 2.0
    local half_h = self.h / 2.0
    
    local left   = self.x - half_w
    local right  = self.x + half_w
    local top    = self.y - half_h
    local bottom = self.y + half_h
    
    local fill_w = math.max(0.1, self.w - (t * 2.0))
    local fill_h = math.max(0.1, self.h - (t * 2.0))
    
    -- Build/Stamps the 9-Slice Geometry Background Layout
    local pass_matrix = {
        {1, left,         top,          t,      t     }, 
        {2, left + t,     top,          fill_w, t     }, 
        {3, right - t,    top,          t,      t     }, 
        {4, left,         top + t,      t,      fill_h}, 
        {5, left + t,     top + t,      fill_w, fill_h}, 
        {6, right - t,    top + t,      t,      fill_h}, 
        {7, left,         bottom - t,   t,      t     }, 
        {8, left + t,     bottom - t,   fill_w, t     }, 
        {9, right - t,    bottom - t,   t,      t     }  
    }
    
    for _, item in ipairs(pass_matrix) do
        local spr_id = self.sprites[item[1]]
        SetSpritePriority(spr_id, 1)
        SetSpriteColor(spr_id, 255, 255, 255, self.a)
        SetSpriteSize(spr_id, item[4], item[5])
        SetSpritePosition(spr_id, item[2], item[3])
        PasteSprite(spr_id)
        SetSpritePosition(spr_id, 500, 500)
    end
	--for _, item in ipairs(pass_matrix) do
    --    local spr_id = self.sprites[item[1]]
    --    -- Keep priority hierarchy consistent with your requested architecture
    --    SetSpritePriority(spr_id, self.priority or 5)
    --    SetSpriteColor(spr_id, 255, 255, 255, self.a)
    --    SetSpriteSize(spr_id, item[4], item[5])
     --   SetSpritePosition(spr_id, item[2], item[3])
    --    -- REMOVED: PasteSprite(spr_id) and offscreen parking
    --end
    
    -- RENDER THE CONTAINER TITLE HEADER
    local title_margin_shift = 0.0
    if self.title_text ~= "" then
        -- Map dynamic font step vertical size roughly to safeguard layout space
        local title_h = self:get_line_height(self.title_size)
        title_margin_shift = title_h + 1.0
        
        -- Default Header Calculation: Center it right onto the upper window pane threshold
        -- Font horizontal multiplier approximation per character slot (~0.45% screen width per character)
        local approximate_char_w = 0.45
        if self.title_size == 2 then approximate_char_w = 0.65
        elseif self.title_size == 3 then approximate_char_w = 0.85 end
        
        local title_len_pct = string.len(self.title_text) * approximate_char_w
        local title_x = self.x - (title_len_pct / 2.0)
        local title_y = top + t + 0.5
        
        -- Paint Header Text onto the base background layer securely
        Text(title_x, title_y, self.title_size, self.title_text)
    end
    
    -- DYNAMIC MULTI-SIZE RENDERING PASS WITH ALIGNMENT TARGETING
    if #self.buffer > 0 then
        -- Shift viewport down cleanly if a Title Bar is claimed to avoid truncation overlaps
        local text_start_y = top + t + self.padding_y + title_margin_shift
        local current_line_y = text_start_y
        local lines_printed = 0
        
        -- Determine exactly how many lines fit within the vertical bounds safely
        local tracking_y = text_start_y
        local max_visible_count = 0
        local scan_index = 1 + self.scroll_index
        
        while scan_index <= #self.buffer do
            local entry = self.buffer[scan_index]
            local row_h = self:get_line_height(entry.size)
            if (tracking_y + row_h) <= (bottom - t - self.padding_y) then
                max_visible_count = max_visible_count + 1
                tracking_y = tracking_y + row_h
                scan_index = scan_index + 1
            else
                break
            end
        end
        
        -- Viewport Clamping Rule
        local max_scroll_allowed = math.max(0, #self.buffer - max_visible_count)
        if self.scroll_index > max_scroll_allowed then
            self.scroll_index = max_scroll_allowed
        end
        
		-- Execute Text Stamp Array
		-- Process key interrupts for typewriter bypass actions safely
        self:check_typewriter_bypass()

        -- Sequential Typewriter Gate: Blocks subsequent lines from progressing
        local type_line_blocked = false

        -- Execute Text Stamp Array
        for i = 1 + self.scroll_index, #self.buffer do
            local entry = self.buffer[i]
            local line_h = self:get_line_height(entry.size)
            
            if (current_line_y + line_h) <= (bottom - t - self.padding_y) then
                if self.w < 25.0 then
                    TextColor(left + self.padding_x, current_line_y, 1, "[NARROW]", 255, 255, 255)
                    break
                else
                    local target_print_x = left + self.padding_x 
                    local string_to_render = entry.text
                    
                    if entry.is_typed then
                        -- Sequential Engine: Only process timeline math if no previous line is still typing
                        if not type_line_blocked then
                            if entry.current_visible_len < string.len(entry.text) then
                                entry.current_visible_len = entry.current_visible_len + (entry.cps * precision_dt)
                                if entry.current_visible_len > string.len(entry.text) then
                                    entry.current_visible_len = string.len(entry.text)
                                end
                            end
                            
                            -- If this line is STILL crawling, block all lines following it
                            if entry.current_visible_len < string.len(entry.text) then
                                type_line_blocked = true
                            end
                        end
                        
                        -- Slice up to the current integer floor truncation calculation
                        local reveal_count = math.floor(entry.current_visible_len)
                        string_to_render = string.sub(entry.text, 1, reveal_count)
                    end
                    
                    -- Alignment computations matching string width dynamics
                    
                    -- Alignment computations matching string width dynamics
                    if self.alignment == "center" or self.alignment == "right" then
                        local scale_char_factor = 0.42
                        if entry.size == 2 then scale_char_factor = 0.62
                        elseif entry.size == 3 then scale_char_factor = 0.82 end
                        
                        local string_width_pct = string.len(string_to_render) * scale_char_factor
                        
                        if self.alignment == "center" then
                            target_print_x = self.x - (string_width_pct / 2.0)
                        elseif self.alignment == "right" then
                            target_print_x = right - self.padding_x - string_width_pct
                        end
                    end
                    
                    if target_print_x < (left + self.padding_x) then
                        target_print_x = left + self.padding_x
                    end
                    
                    -- Paint current frame slice string atomically
                    if string.len(string_to_render) > 0 then
                        TextColor(target_print_x, current_line_y, entry.size, string_to_render, entry.r, entry.g, entry.b)
                    end
                end
                current_line_y = current_line_y + line_h
                lines_printed = lines_printed + 1
            else
                break
            end
        end
		-- =====================================================================
        -- SELF-CONTAINED AUTOMATED SCROLLBAR ENGINE
        -- =====================================================================
        -- Only draw if the data total overflows what can actively fit on screen
        if #self.buffer > max_visible_count then
            -- -----------------------------------------------------------------
            -- VISUAL TUNING CONTROLS FOR ELEVATION & DISTANCE
            -- -----------------------------------------------------------------
            -- Increase this to pull the top of the bar down, decrease/make negative to pull it up!
            local top_padding_nudge = -1.5 
            
            -- Adjust this to control how far down the scroll bar track can go
            local bottom_padding_nudge = -1.0
            
            -- Increase this to pull the golden '*' handle up relative to its calculation
            local visual_thumb_offset = 0.8
            -- -----------------------------------------------------------------

            local track_top = text_start_y + top_padding_nudge
            local track_bottom = bottom - t - self.padding_y + bottom_padding_nudge
            local track_height = track_bottom - track_top
            
            -- Position the scroll line tightly inside the right interior edge
            local scroll_bar_x = right - t - 1.2
            
            -- Calculate precise vertical positioning ratios based on overflow depth
            local total_overflow = math.max(1, #self.buffer - max_visible_count)
            local scroll_percentage = clamp(self.scroll_index / total_overflow, 0.0, 1.0)
            
            -- Map percentage smoothly onto track vertical layout space
            local thumb_y = track_top + (scroll_percentage * (track_height - 1.5))
            
            -- 1. Draw static background track rail gutter
            local rail_y = track_top
            while rail_y <= track_bottom do
                TextColor(scroll_bar_x, rail_y, 3, "-", 80, 80, 80) -- Gray dashes
                rail_y = rail_y + 1.8
            end
            
            -- 2. Draw Active Interactive Moving Thumb Slider
            TextColor(scroll_bar_x - 0.1, thumb_y - visual_thumb_offset, 3, "*", 255, 215, 0) -- Vibrant Gold
        end
        -- =====================================================================
    end
end

function nineslice:park_offscreen()
    self.resizing = false
    for i = 1, 9 do
        if self.sprites[i] and self.sprites[i] > 0 then
            SetSpritePosition(self.sprites[i], 500, 500)
        end
    end
    return self
end

function nineslice:destroy()
    for i = 1, 9 do
        if self.sprites[i] then DeleteSprite(self.sprites[i]) end
    end
    self.sprites = {}
    self.images  = {}
end

return spritelib