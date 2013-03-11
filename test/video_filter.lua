function GetFrame(frame, frame_i)
	local ctx = g2d.create_context(frame)
	local kernel = {width = 7, height = 7}
	local kernel_size = kernel.width * kernel.height
	for i = 1, kernel_size do
		kernel[i] = 1 / kernel_size
	end
	g2d.image_convolution(frame, kernel)
end
