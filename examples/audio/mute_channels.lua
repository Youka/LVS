-- Process samples
function GetSamples(samples, samples_n, start_sample_i)
	-- Iterate through samples (skipping first channel)
	for channel = 2, AUDIO_CHANNELS do
		for channel_sample_i = 0, samples_n / AUDIO_CHANNELS - 1 do
			-- Mute audio
			samples[channel + channel_sample_i * AUDIO_CHANNELS] = 0
		end
	end
end