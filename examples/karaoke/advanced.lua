-- ASS file loading
ass.load("karaoke\\oreimo_s2_6 - ed.ass")
lines[6].outfade = 570
lines[7].infade = lines[6].outfade
lines[17].outfade = lines[6].outfade
lines[18].infade = lines[6].outfade
-- Image resources
local paws
do
	local paw_white_image, paw_black_image = g2d.create_image_from_png("karaoke\\Pfote_weiß.png"), g2d.create_image_from_png("karaoke\\Pfote_schwarz.png")
	paws = {
		white = {src = g2d.create_pattern(paw_white_image), w = paw_white_image:get_width(), h = paw_white_image:get_height()},
		black = {src = g2d.create_pattern(paw_black_image), w = paw_black_image:get_width(), h = paw_black_image:get_height()},
		lines = {}
	}
	for _, line in ipairs(lines) do
		local stamps = {n = math.floor(line.width * line.height / 1000)}
		for i=1, stamps.n do
			stamps[i] = {rotation = math.random(0,360), x = math.random(0,line.width), y = math.random(0,line.height)}
		end
		paws.lines[line.i] = stamps
	end
end

-- Draw text (including in- & outfade)
local function draw_text(ctx, ms, line)

end

-- Draw sylable effect on text
local function syl_effect(ctx, ms, line)
	-- Effect data
	local out_offset = 200
	local fade_fx_width = 0.5
	-- Effect drawing
	ctx:set_composition("DIFFERENCE")
	for si, syl in ipairs(line.syls) do
		if ms >= syl.start_time and ms < syl.end_time + out_offset and syl.text ~= "" then
			local fade_progress = (1 + fade_fx_width) * math.inrange(syl.start_time, syl.end_time + out_offset, ms)
			local cx, cy = syl.x + syl.width/2, syl.y + syl.height/2
			ctx:set_source(
				g2d.create_rgradient(cx,cy,0, cx,cy,math.distance(syl.width/2,syl.height/2))
				:add_color(-fade_fx_width + fade_progress,1,1,1,0)
				:add_color(-fade_fx_width*2/3 + fade_progress,1,1,1,1)
				:add_color(-fade_fx_width/3 + fade_progress,1,1,1,1)
				:add_color(fade_progress,1,1,1,0)
			)
			ctx:path_add_text(syl.x, syl.y, syl.text, ass.unpack_font(line.styleref))
			ctx:fill()
			ctx:path_clear()
		end
	end
	ctx:set_composition("OVER")
end

-- Process frames
function GetFrame(frame, frame_i)
	-- Frame data
	local ctx = frame:get_context()
	local ms = frame_i / VIDEO_FPS * 1000
	-- Look for frame-related ASS lines
	for _, line in ipairs(lines) do
		if ms >= line.start_time - line.infade/2 and ms < line.end_time + line.outfade/2 then
			-- Draw ASS line
			draw_text(ctx, ms, line)
			if #line.syls > 0 then
				syl_effect(ctx, ms, line)
			end
		end
	end
end