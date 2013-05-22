-- Triangle points
local points = {
	{x = 100, y = 400},
	{x = 500, y = 400},
	{x = 300, y = 100}
}
-- Create triangle gradient (note: aliased)
local gradient = g2d.create_mgradient():add_mesh(
	points[1].x, points[1].y, 1, 0, 0, 1,
	points[2].x, points[2].y, 0, 1, 0, 1,
	points[3].x, points[3].y, 0, 0, 1, 1
)

-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = frame:get_context()
	-- Define triangle path
	ctx:path_move_to(points[1].x, points[1].y)
	ctx:path_line_to(points[2].x, points[2].y)
	ctx:path_line_to(points[3].x, points[3].y)
	ctx:path_close()
	-- Fill path
	ctx:set_source(gradient)
	ctx:fill()
end
