local kernel = g2du.create_gaussian_blur_kernel(3)

function GetFrame(frame, frame_i)
	g2d.image_convolution(frame, kernel)
end