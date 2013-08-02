-- ASS file loading
ass.load("karaoke\\res\\oreimo_s2_6 - ed.ass")
lines[6].outfade = 570
lines[7].infade = lines[6].outfade
lines[17].outfade = lines[6].outfade
lines[18].infade = lines[6].outfade

-- Font loading (needed for ASS text)
g2d.register_font("karaoke\\res\\VAGROUND.TTF")

-- Textures loading
local paws
do
	local paw_white_image, paw_black_image = g2d.create_image_from_png("karaoke\\res\\Pfote_weiß.png"), g2d.create_image_from_png("karaoke\\res\\Pfote_schwarz.png")
	paws = {
		white = {src = g2d.create_pattern(paw_white_image), w = paw_white_image:get_width(), h = paw_white_image:get_height()},
		black = {src = g2d.create_pattern(paw_black_image), w = paw_black_image:get_width(), h = paw_black_image:get_height()},
		lines = {}
	}
	for li, line in ipairs(lines) do
		local stamps = {n = math.floor(line.width * line.height / 800)}
		for i=1, stamps.n do
			stamps[i] = {rotation = math.random(0,360), x = math.random(0,line.width), y = math.random(0,line.height)}
		end
		paws.lines[line.i] = stamps
	end
end

-- Draw text (including in- & outfade)
local function draw_text(ctx, ms, line)
	-- Shortcut
	local border = line.styleref.outline
	-- Vertical line position
	local y = line.style == "Subtitle Insert" and line.y + line.extlead + line.height or line.y
	-- Color & texture data
	local fill_color, border_color, paw
	if line.i % 2 == 0 then
		fill_color, border_color, paw = g2du.black, g2du.white, paws.white
	else
		fill_color, border_color, paw = g2du.white, g2du.black, paws.black
	end
	-- Text image
	local text = g2d.create_image("RGBA", border * 2 + line.width, border * 2 + line.height)
	local tctx = text:get_context()
	tctx:path_add_text(border, border, line.text, ass.unpack_font(line.styleref))
	tctx:set_line_width(border * 2)
	tctx:set_source(border_color)
	tctx:stroke()
	tctx:set_source(fill_color)
	tctx:fill()
	local paw_pos = paws.lines[line.i]
	for i=1, paw_pos.n do
		paw.src:set_matrix(g2d.create_matrix()
			:translate(border + paw_pos[i].x, border + paw_pos[i].y)
			:rotate(paw_pos[i].rotation)
			:scale(0.3,0.3)
			:translate(-paw.w / 2, -paw.h / 2)
		)
		tctx:set_source(paw.src)
		tctx:fill()
	end
	-- Draw text image with transparency
	text = g2d.create_pattern(text)
	text:set_matrix(g2d.create_matrix():translate(-border+line.x, -border+y))
	ctx:set_source(text)
	if ms < line.start_time or ms > line.end_time then
		local mask = g2d.create_lgradient(0, y - border, 0, y + line.height + border)
		local fade_range = 0.33
		if ms < line.start_time then
			local fade_progress = (1 + fade_range*2) * math.inrange(line.start_time - line.infade, line.start_time, ms)
			mask:add_color(1+fade_range - fade_progress,0,0,0,0):add_color(1+fade_range*2 - fade_progress,0,0,0,1)
		elseif ms > line.end_time then
			local fade_progress = (1 + fade_range*2) * math.inrange(line.end_time, line.end_time + line.outfade, ms)
			mask:add_color(1 - fade_progress,0,0,0,1):add_color(1+fade_range - fade_progress,0,0,0,0)
		end
		ctx:masked_paint(mask)
	else
		ctx:paint()
	end
end

-- Draw sylable effect on text
local function draw_syl_effect(ctx, ms, line)
	-- Effect data
	local out_offset = 200
	local gradient_width = 0.75
	-- Effect drawing
	ctx:set_composition("DIFFERENCE")
	for si, syl in ipairs(line.syls) do
		if ms >= syl.start_time and ms < syl.end_time + out_offset and syl.text ~= "" then
			local cx, cy = syl.x + syl.width/2, syl.y + syl.height/2
			local gradient_progress = (1 + gradient_width) * math.inrange(syl.start_time, syl.end_time + out_offset, ms)
			ctx:set_source(
				g2d.create_rgradient(cx,cy,0, cx,cy,math.distance(syl.width/2,syl.height/2))
				:add_color(-gradient_width + gradient_progress,1,1,1,0)
				:add_color(-gradient_width*2/3 + gradient_progress,1,1,1,1)
				:add_color(-gradient_width/3 + gradient_progress,1,1,1,1)
				:add_color(gradient_progress,1,1,1,0)
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
		if ms >= line.start_time - line.infade and ms < line.end_time + line.outfade then
			-- Draw ASS line
			draw_text(ctx, ms, line)
			if #line.syls > 0 then
				draw_syl_effect(ctx, ms, line)
			end
		end
	end
end