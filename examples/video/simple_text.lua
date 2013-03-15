-- Create transformation matrix
local translation = g2d.create_matrix()
translation:translate(250, 100)
-- Create raster sources
local white = g2d.create_source_color(1, 1, 1)
local black = g2d.create_source_color(0, 0, 0)

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Define path
	ctx:set_matrix(translation)
	ctx:path_add_text("こんにちわ", "Arial", 60, true, true, true, true)
	-- Stroke path
	ctx:set_source(white)
	ctx:set_line_join("ROUND")
	ctx:set_line_width(6)
	ctx:path_stroke()
	-- Fill path
	ctx:set_source(black)
	ctx:path_fill()
end
