--[[
meta
	.comment
	.title
	.script_type
	.wrap_style
	.width
	.height
	.scaled_border_and_shadow
	.ratio
	.zoom
	.position
	.last_style
	.audio
	.video
styles
	[i]
		.name
		.fontname
		.fontsize
		.color1
			.r
			.g
			.b
			.a
		.color2
			.r
			.g
			.b
			.a
		.color3
			.r
			.g
			.b
			.a
		.color4
			.r
			.g
			.b
			.a
		.bold
		.italic
		.underline
		.strikeout
		.scale_x
		.scale_y
		.spacing
		.angle
		.borderstyle
		.outline
		.shadow
		.alignment
		.margin_l
		.margin_r
		.margin_v
		.encoding
lines
	[i]
		.comment
		.layer
		.start_time
		.mid_time
		.end_time
		.duration
		.infade
		.outfade
		.style
		.styleref
		.actor
		.margin_l
		.margin_r
		.margin_v
		.effect
		.k_text
		.text
		.i
		.width
		.height
		.ascent
		.descent
		.intlead
		.extlead
		.x
		.y
		.syls
			[i]
				.i
				.word_i
				.start_time
				.mid_time
				.end_time
				.duration
				.text
				.inline_fx
				.prespace
				.postspace
				.width
				.height
				.ascent
				.descent
				.intlead
				.extlead
				.x
				.y
		.chars
			[i]
				.i
				.syl_i
				.word_i
				.start_time
				.mid_time
				.end_time
				.duration
				.text
				.width
				.height
				.ascent
				.descent
				.intlead
				.extlead
				.x
				.y
		.words
			[i]
				.i
				.start_time
				.mid_time
				.end_time
				.duration
				.text
				.prespace
				.postspace
				.width
				.height
				.ascent
				.descent
				.intlead
				.extlead
				.x
				.y
]]--

-- ASS parser
function LoadASS(content)
	-- Unicode string trimming
	local function trim(text)
		-- Text to character table
		local chars, chars_n = table.create(text:ulen(),0), 0
		for ci, char in text:uchars() do
			chars_n = chars_n + 1
			chars[chars_n] = char
		end
		-- Count prespace
		local prespace = 0
		for ci = 1, chars_n do
			if chars[ci] ~= " " and chars[ci] ~= "\t" then
				prespace = ci-1
				break
			elseif ci == chars_n then
				prespace = chars_n
			end
		end
		-- Count postspace
		local postspace = 0
		if prespace ~= chars_n then
			for ci = chars_n, 1, -1 do
				if chars[ci] ~= " " and chars[ci] ~= "\t" then
					postspace = chars_n - ci
					break
				elseif ci == 1 then
					postspace = chars_n
				end
			end
		end
		-- Return trimmed text and spaces
		return text:sub(1+prespace,-1-postspace), prespace, postspace
	end
	-- Searching patterns
	local meta_comment = ";%s*(.*)"
	local meta_title = "Title:%s*(.*)"
	local meta_scripttype = "ScriptType:%s*(.*)"
	local meta_wrapstyle = "WrapStyle:%s*(%d)"
	local meta_width = "PlayResX:%s*(%d+)"
	local meta_height = "PlayResY:%s*(%d+)"
	local meta_scalebands = "ScaledBorderAndShadow:%s*(.*)"
	local meta_ratio = "Video Aspect Ratio:%s*(%d+%.?%d*)"
	local meta_zoom = "Video Zoom:%s*(%d+)"
	local meta_position = "Video Position:%s*(%d+)"
	local meta_laststyle = "Last Style Storage:%s*(.*)"
	local meta_audio = "Audio File:%s*(.*)"
	local meta_video = "Video File:%s*(.*)"
	local style_line = "Style:%s*(.-),(.-),(%d+%.?%d*),(.-),(.-),(.-),(.-),(%-?%d+),(%-?%d+),(%-?%d+),(%-?%d+),(%d+%.?%d*),(%d+%.?%d*),(%-?%d+%.?%d*),(%-?%d+%.?%d*),(%-?%d+),(%d+%.?%d*),(%-?%d+%.?%d*),(%d),(%d+),(%d+),(%d+),(%d+)"
	local dialog_line = "Dialogue:%s*(%d+),(.-),(.-),(.-),(.-),(%d+),(%d+),(%d+),(.-),(.*)"
	local comment_line = "Comment:%s*(%d+),(.-),(.-),(.-),(.-),(%d+),(%d+),(%d+),(.-),(.*)"
	-- Parse ASS line
	local function ParseLine(line)
			-- Meta
		if line:find(meta_comment) then
			if meta.comment then
				meta.comment = string.format("%s\n%s", meta.comment, line:match(meta_comment))
			else
				meta.comment = line:match(meta_comment)
			end
		elseif line:find(meta_title) then
			meta.title = line:match(meta_title)
		elseif line:find(meta_scripttype) then
			meta.script_type = line:match(meta_scripttype)
		elseif line:find(meta_wrapstyle) then
			meta.wrap_style = tonumber(line:match(meta_wrapstyle))
		elseif line:find(meta_width) then
			meta.width = tonumber(line:match(meta_width))
		elseif line:find(meta_height) then
			meta.height = tonumber(line:match(meta_height))
		elseif line:find(meta_scalebands) then
			meta.scaled_border_and_shadow = line:match(meta_scalebands) == "yes"
		elseif line:find(meta_ratio) then
			meta.ratio = tonumber(line:match(meta_ratio))
		elseif line:find(meta_zoom) then
			meta.zoom = tonumber(line:match(meta_zoom))
		elseif line:find(meta_position) then
			meta.position = tonumber(line:match(meta_position))
		elseif line:find(meta_laststyle) then
			meta.last_style = line:match(meta_laststyle)
		elseif line:find(meta_audio) then
			meta.audio = line:match(meta_audio)
		elseif line:find(meta_video) then
			meta.video = line:match(meta_video)
		-- Styles
		elseif line:find(style_line) then
			-- Extract style
			local style = table.create(0,23)
			style.name, style.fontname, style.fontsize,
			style.color1, style.color2, style.color3, style.color4,
			style.bold, style.italic, style.underline, style.strikeout,
			style.scale_x, style.scale_y, style.spacing, style.angle,
			style.borderstyle, style.outline, style.shadow,
			style.alignment, style.margin_l, style.margin_r, style.margin_v,
			style.encoding =
			line:match(style_line)
			-- Reformat style
			style.fontsize = tonumber(style.fontsize)
			for c = 1, 4 do
				local a, b, g, r = _G["style.color" .. c]:match("&H(%x%x)(%x%x)(%x%x)(%x%x)")
				if r then
					_G["style.color" .. c] = {r = tonumber(r,16) / 255, g = tonumber(g,16) / 255, b = tonumber(b,16) / 255, a = (255 - tonumber(a,16)) / 255}
				else
					_G["style.color" .. c] = nil
				end
			end
			style.bold = style.bold == "-1"
			style.italic = style.italic == "-1"
			style.underline = style.underline == "-1"
			style.strikeout = style.strikeout == "-1"
			style.scale_x = type(style.scale_x) == "number" and style.scale_x / 100 or nil
			style.scale_y = type(style.scale_y) == "number" and style.scale_x / 100 or nil
			style.spacing = tonumber(style.spacing)
			style.angle = tonumber(style.angle)
			style.borderstyle = style.borderstyle == "3"
			style.outline = tonumber(style.outline)
			style.shadow = tonumber(style.shadow)
			style.alignment = tonumber(style.alignment)
			style.margin_l = tonumber(style.margin_l)
			style.margin_r = tonumber(style.margin_r)
			style.margin_v = tonumber(style.margin_v)
			style.encoding = tonumber(style.encoding)
			styles[#styles+1] = style
		-- Lines
		elseif line:find(comment_line) or line:find(dialog_line) then
			-- Extract dialog
			local dialog = table.create(0,11)
			if line:find(comment_line) then
				dialog.comment = true
				dialog.layer, dialog.start_time, dialog.end_time, dialog.style, dialog.actor, dialog.margin_r, dialog.margin_l, dialog.margin_v, dialog.effect, dialog.k_text =
				line:match(comment_line)
			else
				dialog.comment = false
				dialog.layer, dialog.start_time, dialog.end_time, dialog.style, dialog.actor, dialog.margin_r, dialog.margin_l, dialog.margin_v, dialog.effect, dialog.k_text =
				line:match(dialog_line)
			end
			-- Reformat dialog
			dialog.layer = tonumber(dialog.layer)
			local h, m, s, ms = dialog.start_time:match("(%d+):(%d+):(%d+).(%d+)")
			if ms then
				dialog.start_time = ms * 10 + s * 1000 + m * 60000 + h * 3600000
			else
				dialog.start_time = nil
			end
			h, m, s, ms = dialog.end_time:match("(%d+):(%d+):(%d+).(%d+)")
			if ms then
				dialog.end_time = ms * 10 + s * 1000 + m * 1000 * 60 + h * 3600000
			else
				dialog.end_time = nil
			end
			dialog.margin_r = tonumber(dialog.margin_r)
			dialog.margin_l = tonumber(dialog.margin_l)
			dialog.margin_v = tonumber(dialog.margin_v)
			lines[#lines+1] = dialog
		end
	end
	-- Add more informations to line
	local function UpgradeLines()
		-- Through lines
		for li, line in ipairs(lines) do
			-- Add times
			if line.start_time and line.end_time then
				line.duration = line.end_time - line.start_time
				line.mid_time = line.start_time + line.duration / 2
			end
			-- Add style reference
			for si, style in ipairs(styles) do
				if line.style == style.name then
					line.styleref = style
				end
			end
			-- Add tagless text
			line.text = line.k_text:gsub("{.-}", "")
			-- Add index
			line.i = li
		end
	end
	-- Add line subtexts
	local function AddSubTexts()

		-- TODO

	end
	-- Add sizes to all line texts
	local function AddSizes()

		-- TODO

	end
	-- Add positions to all line texts
	local function AddPositions()

		-- TODO

	end
	-- Add line in- & outfade time
	local function AddInOutTimes(first_last_dur)

		-- TODO

	end
	-- Check function argument
	if type(content) ~= "string" then
		error("string expected", 2)
	end
	-- Try to open content as file
	if content:len() < 256 then
		local file = io.open(content, "r")
		if file then
			content = file:read("*a")
			file:close()
		end
	end
	-- Define global ASS tables
	meta, styles, lines = {}, {}, {}
	-- Parse lines
	for line in content:gmatch("[^\n]+") do
		ParseLine(line)
	end
	-- Add additional informations
	UpgradeLines()
	AddSubTexts()
	AddSizes()
	AddPositions()
	AddInOutTimes(1000.1)
end