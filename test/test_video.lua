-- Process frames
function GetFrame(frame, frame_i)
	-- Create drawing context
	local ctx = g2d.create_context(frame)
	-- Define path
	ctx:set_matrix(g2d.create_matrix(1,0,0,1,0,0))
	ctx:path_add_text("LVS", "Comic Sans MS", 50, true, false, false, false)
	-- Set source
	local source = g2d.create_source_radial_gradient(35,25,5,
													35,25,30)
	source:add_color(0,1,1,1)
	source:add_color(1,0.1,0.1,0.1)
	ctx:set_source(source)
	-- Fill path
	ctx:path_fill()
end