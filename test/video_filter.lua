function GetFrame(frame, frame_i)
	g2d.image_convolution(frame, {
		width = 3, height = 3,
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	})
end
