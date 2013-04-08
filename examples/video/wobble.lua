-- Process frames
function GetFrame(frame, frame_i)
	-- Create image/frame context
	local ctx = g2d.create_context(frame)
	-- Define paths
	ctx:path_add_rectangle(100,300,200,100)
	ctx:path_move_to(580,200)
	ctx:path_add_arc(500,200,80,0,360)
	-- Transform path to wobble
	g2du.path_transform(ctx, function(x, y)
		return x + math.sin(y/4 + frame_i/20) * 3, y + math.cos(x/4 + frame_i/20) * 3
	end)
	-- Fill path
	ctx:path_fill()
end