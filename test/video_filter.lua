function GetFrame(frame, frame_i)
	-- Create matrix
	local matrix = g2d.create_matrix()
	matrix:translate(250, 100)
	-- Create source
	local color = g2d.create_source_linear_gradient(0, 0, 0, 60)
	color:add_color(0, 1, 0, 0)
	color:add_color(0.5, 1, 1, 0)
	color:add_color(1, 1, 0, 0)
	-- Create image context
	local ctx = g2d.create_context(frame)
	ctx:set_matrix(matrix)
	ctx:set_source(color)
	-- Define and fill path
	ctx:path_add_text("Hello world!", "Times New Roman", 60)
	ctx:path_fill()
end
