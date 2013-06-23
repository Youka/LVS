-- Kernels for image convolution
local box_blur_kernel = g2du.create_box_blur_kernel(3)
local gaussian_blur_kernel = g2du.create_gaussian_blur_kernel(3.5)
local motion_blur_kernel = g2du.create_motion_blur_kernel(3, 45)
local sharpen_kernel = g2du.create_sharpen_kernel(1)
local edge_detect_kernel = g2du.create_edge_detect_kernel(1)
local emboss_kernel = {
	width = 3, height = 3,
	1, 1, 0,
	1, 0, -1,
	0, -1, -1
}

-- Process frames
function GetFrame(frame, frame_i)
	-- Convolute frame by time/frame number
	if frame_i < 100 then
		-- No change/original
	elseif frame_i < 200 then
		frame:convolute(box_blur_kernel)
	elseif frame_i < 300 then
		frame:convolute(gaussian_blur_kernel)
	elseif frame_i < 400 then
		frame:convolute(motion_blur_kernel)
	elseif frame_i < 500 then
		frame:convolute(sharpen_kernel)
	elseif frame_i < 600 then
		frame:convolute(edge_detect_kernel)
	elseif frame_i < 700 then
		frame:convolute(emboss_kernel)
	end
end