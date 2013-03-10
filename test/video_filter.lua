function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	g2d.image_convolution(frame, {
		width = 11, height = 1,
		1/11, 1/11, 1/11, 1/11, 1/11, 1/11, 1/11, 1/11, 1/11, 1/11, 1/11
	})
end
