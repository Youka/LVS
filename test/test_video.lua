-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Set transformation
	local matrix = g2d.create_matrix()
	matrix:translate(50,50)
	g2du.rotate_xy(matrix, 80,80)
	matrix:translate(-50,-50)
	ctx:set_matrix(matrix)
	-- Set source
	local source = g2d.create_source_mesh_gradient()
	source:add_mesh(
		0,0, 1,0,0,1,
		100,0, 0,1,0,1,
		100,100, 0,0,1,1,
		0,100, 1,1,0,1
	)
	ctx:set_source(source)
	-- Define path
	ctx:path_add_rectangle(0,0,100,100)
	-- Fill path
	ctx:path_fill()
end