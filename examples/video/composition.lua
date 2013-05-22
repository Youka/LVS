-- Defines, fills and clears rectangle path
local function draw_rectangle(ctx, x, y, w, h)
	ctx:path_add_rectangle(x, y, w, h)
	ctx:fill()
	ctx:path_clear()
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = frame:get_context()
	-- Blue rectangle
	ctx:set_composition("SOURCE")
	ctx:set_source(g2du.blue)
	draw_rectangle(ctx, 10, 10, 150, 150)
	-- Overblending half-transparent white rectangle
	ctx:set_composition("OVER")
	ctx:set_source(g2d.create_color(1, 1, 1, 0.5))
	draw_rectangle(ctx, 170, 10, 150, 150)
	-- Red color addition rectangle
	ctx:set_composition("ADD")
	ctx:set_source(g2d.create_color(1, 0, 0))
	draw_rectangle(ctx, 330, 10, 150, 150)
	-- Color inversion rectangle
	ctx:set_composition("DIFFERENCE")
	ctx:set_source(g2du.white)
	draw_rectangle(ctx, 490, 10, 150, 150)
end