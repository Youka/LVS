LoadASS("E:\\PROJEKTE\\LVS\\test\\Zetsuen_no_Tempest_-_ED_[ktime.youka].ass")

function GetFrame(frame, frame_i)
	local ms = frame_i / VIDEO_FPS * 1000
	local ctx = g2d.create_context(frame)
	for _, line in ipairs(lines) do
		if ms >= line.start_time and ms < line.end_time then
			ctx:set_matrix(g2d.create_matrix(1, 0, 0, 1, line.x, line.y))
			ctx:path_add_text(line.text, line.styleref.fontname, line.styleref.fontsize, line.styleref.bold, line.styleref.italic, line.styleref.underline, line.styleref.strikeout)
			ctx:set_source(g2du.white)
			ctx:path_fill()
		end
	end
end