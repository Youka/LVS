-- Matrices for color transformations
local grey = g2du.create_grey_matrix(1)
local red_domination = g2du.create_colorrate_matrix(2,0.5,0.5)
local red_blue_swap = {
	0, 0, 1, 0,
	0, 1, 0, 0,
	1, 0, 0, 0,
	0, 0, 0, 1
}

-- Process frames
function GetFrame(frame, frame_i)
	-- Change frame colors by time/frame number
	if frame_i < 100 then
		-- No change/original
	elseif frame_i < 200 then
		frame:color_transform(grey)
	elseif frame_i < 300 then
		frame:color_transform(red_domination)
	elseif frame_i < 400 then
		frame:color_transform(red_blue_swap)
	end
end