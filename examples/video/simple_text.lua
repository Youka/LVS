-- Create transformation matrix
local translation = g2d.create_matrix()
translation:translate(350, 100)

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Define path
	ctx:set_matrix(translation)
	ctx:path_add_text("こんにちわ\nGuten Tag!\nGood day!", "Arial", 60, true, true, true, true)
	-- Stroke path
	ctx:set_source(g2du.white)
	ctx:set_line_join("ROUND")
	ctx:set_line_width(6)
	ctx:path_stroke()
	-- Fill path
	ctx:set_source(g2du.black)
	ctx:path_fill()
end
