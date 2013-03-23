-- Create file target for audio
local file = io.open("samples.txt", "w")
-- Write audio meta information to file
file:write(string.format("Channels: %d\nSample rate: %d\nSamples: %d\n\n", AUDIO_CHANNELS, AUDIO_SAMPLE_RATE, AUDIO_SAMPLES))

-- Process samples
function GetSamples(samples, samples_n, start_sample_i)
	-- Trim samples
	for sample_i = 1, samples_n do
		samples[sample_i] = samples[sample_i] - samples[sample_i] % 0.0001
	end
	-- Write samples to file
	file:write(table.concat(samples, "\n", 1, samples_n))
	file:write("\n")
end