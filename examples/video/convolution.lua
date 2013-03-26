-- Kernels for image convolution
local box_blur_kernel = g2du.create_box_blur_kernel(3)
local gaussian_blur_kernel = g2du.create_gaussian_blur_kernel(3)
local motion_blur_kernel = g2du.create_motion_blur_kernel(3, 45)
local sharpen_kernel = g2du.create_sharpen_kernel(1)
local edge_detect_kernel = g2du.create_edge_detect_kernel(1)

-- Process frames
function GetFrame(frame, frame_i)
	-- Convolute frame by time/frame number
	if frame_i < 100 then
		-- No change/original
	elseif frame_i < 200 then
		g2d.image_convolution(frame, box_blur_kernel)
	elseif frame_i < 300 then
		g2d.image_convolution(frame, gaussian_blur_kernel)
	elseif frame_i < 400 then
		g2d.image_convolution(frame, motion_blur_kernel)
	elseif frame_i < 500 then
		g2d.image_convolution(frame, sharpen_kernel)
	elseif frame_i < 600 then
		g2d.image_convolution(frame, edge_detect_kernel)
	end
end