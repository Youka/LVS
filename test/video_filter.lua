function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	ctx:path_add_text("Hello world!", "Times New Roman", 60)
end
