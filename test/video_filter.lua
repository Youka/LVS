function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	g2d.image_convolution(frame, {
		width = 1, height = 1,
		1
	})
end
