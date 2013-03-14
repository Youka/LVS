-- Create transformation matrix
local matrix = g2d.create_matrix()
matrix:translate(250, 100)
-- Create raster source
local source = g2d.create_source_linear_gradient(0, 0, 50, 0)
source:add_color(0, 1, 0, 0)
source:add_color(0.5, 0, 1, 0)
source:add_color(1, 0, 0, 1)
source:set_extender("MIRROR")

-- Process frame
function GetFrame(frame, frame_i)
	-- Create image context
	local ctx = g2d.create_context(frame)
	ctx:set_matrix(matrix)
	ctx:set_source(source)
	-- Define path
	ctx:path_add_text("こんにちわ", "Times New Roman", 60, true, true, true, true)
	-- Stroke path
	ctx:path_stroke()
end
