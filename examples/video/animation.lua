-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Set line thickness
	ctx:set_line_width(6)
	-- Draw 5 progress lines
	for i = -2, 2 do
		-- Calculate progress status
		local pct = (math.floor(frame_i/2) + i) % 12 / 12
		-- Define line path
		ctx:path_move_to(math.ellipse(300, 300, 40, 40, pct * -360))
		ctx:path_line_to(math.ellipse(300, 300, 80, 80, pct * -360))
		-- Set line color
		ctx:set_source(g2d.create_source_color(1, 1, 1, 1 - math.abs(i) * 0.33))
		-- Draw progress line
		ctx:path_stroke()
		ctx:path_clear()
	end
end