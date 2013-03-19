local kernel = g2du.create_edge_detect_kernel()

function GetFrame(frame, frame_i)
	g2d.image_convolution(frame, kernel)
end