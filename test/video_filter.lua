local kernel = {width = 17, height = 17}
do
	local kernel_size = kernel.width * kernel.height
	local kernel_value = 1 / kernel_size
	for i = 1, kernel_size do
		kernel[i] = kernel_value
	end
end

function GetFrame(frame, frame_i)
		g2d.image_convolution(frame, kernel)
end
