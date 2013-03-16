local white = g2d.create_source_color(1, 1, 1)

function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	ctx:set_source(white)
	ctx:set_composition("DIFFERENCE")
	ctx:paint()
end
