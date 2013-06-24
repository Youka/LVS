-- Parse ASS file and fill global tables
ass.load("karaoke\\chihaya_ed.ass")

-- Roumaji & kanji
local function roumaji_kanji(ctx, ms, line)

	-- TODO

end

-- Subtitle
local function subtitle(ctx, ms, line)

	-- TODO

end

-- Process frames
function GetFrame(frame, frame_i)
	-- Create drawing context
	local ctx = frame:get_context()
	-- Get frame time
	local ms = frame_i / VIDEO_FPS * 1000
	-- Look for frame-related ASS lines
	for _, line in ipairs(lines) do
		if ms >= line.start_time - line.infade/2 and ms < line.end_time + line.outfade/2 then
			-- Draw ASS line
			if line.styleref.alignment > 3 then
				roumaji_kanji(ctx, ms, line)
			else
				subtitle(ctx, ms, line)
			end
		end
	end
end