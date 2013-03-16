-- Process samples
function GetSamples(samples, samples_n, start_sample_i)
	-- Iterate through samples
	for channel = 1, AUDIO_CHANNELS do
		for channel_sample_i = 0, samples_n / AUDIO_CHANNELS - 1 do
			-- Calculate sample index
			local sample_i = channel + channel_sample_i * AUDIO_CHANNELS
			-- Half audio volume
			samples[sample_i] = samples[sample_i] / 2
		end
	end
end
