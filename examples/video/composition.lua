-- Defines, fills and clears rectangle path
local function draw_rectangle(ctx, x, y, w, h)
	ctx:path_add_rectangle(x, y, w, h)
	ctx:path_fill()
	ctx:path_clear()
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Blue rectangle
	ctx:set_composition("SOURCE")
	ctx:set_source(g2du.blue)
	draw_rectangle(ctx, 0, 0, 150, 150)
	-- Overblending half-transparent white rectangle
	ctx:set_composition("OVER")
	ctx:set_source(g2d.create_source_color(1, 1, 1, 0.5))
	draw_rectangle(ctx, 160, 0, 150, 150)
	-- Red color addition rectangle
	ctx:set_composition("ADD")
	ctx:set_source(g2d.create_source_color(1, 0, 0))
	draw_rectangle(ctx, 320, 0, 150, 150)
	-- Color inversion rectangle
	ctx:set_composition("DIFFERENCE")
	ctx:set_source(g2du.white)
	draw_rectangle(ctx, 480, 0, 150, 150)
end