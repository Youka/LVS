function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	local image = g2d.create_image_from_png("check.png")
	g2d.create_png_from_image(frame, "frame.png")
	local matrix = g2d.create_matrix()
end
