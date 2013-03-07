function GetSamples(samples, samples_n, start_sample_i)
	local samples_per_channel = samples_n / AUDIO_CHANNELS
	for channel = 1, AUDIO_CHANNELS do
		for channel_sample_i = 0, samples_per_channel-1 do
			local sample_i = channel + channel_sample_i * AUDIO_CHANNELS
			local freq = (start_sample_i + channel_sample_i) / AUDIO_SAMPLE_RATE
			samples[sample_i] = math.sin(freq * math.pi * 600)
		end
	end
end
