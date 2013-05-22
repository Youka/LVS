-- Defines, strokes and clears lines path
local function draw_lines(ctx, off_x, off_y)
	ctx:path_move_to(off_x, off_y)
	ctx:path_line_to(off_x+100, off_y+100)
	ctx:path_line_to(off_x+200, off_y)
	ctx:stroke()
	ctx:path_clear()
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = frame:get_context()
	-- Set lines color & width
	ctx:set_source(g2du.white)
	ctx:set_line_width(10)
	-- Draw lines with round edges
	ctx:set_line_cap("ROUND")
	ctx:set_line_join("ROUND")
	draw_lines(ctx, 10, 10)
	-- Draw lines with sharp edges
	ctx:set_line_cap("SQUARE")
	ctx:set_line_join("MITER")
	draw_lines(ctx, 10, 40)	
	-- Draw lines with flat edges
	ctx:set_line_cap("FLAT")
	ctx:set_line_join("BEVEL")
	draw_lines(ctx, 10, 70)
	-- Draw lines with dashes
	ctx:set_dash(0, {10, 2, 5})
	draw_lines(ctx, 10, 100)		
end