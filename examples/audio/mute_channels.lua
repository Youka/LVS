-- Process samples
function GetSamples(samples, samples_n, start_sample_i)
	-- Iterate through samples (skipping first channel)
	for channel = 2, AUDIO_CHANNELS do
		for channel_sample_i = 0, samples_n / AUDIO_CHANNELS - 1 do
			-- Calculate sample index
			local sample_i = channel + channel_sample_i * AUDIO_CHANNELS
			-- Mute audio
			samples[sample_i] = 0
		end
	end
end