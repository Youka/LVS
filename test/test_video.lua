-- Transformation matrix
local matrix = g2d.create_matrix()
matrix:translate(300,300)
g2du.rotate_xy(matrix, 90, 0)

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Define path
	ctx:set_matrix(matrix)
	ctx:path_add_rectangle(-50,-50,100,100)
	-- Fill path
	ctx:set_source(g2du.white)
	ctx:path_fill()
end