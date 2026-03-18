#include "AudioModificator.h"

#include <algorithm>

#include "Utils.h"

AudioSaveChannel AudioModificator::m_savedChannels;

void AudioModificator::Copy(const AudioMemory& audio, const AudioSelection& selection)
{
	if (selection.endSample <= selection.startSample || audio.audioChannels.empty())
		return;

	m_savedChannels.channels.resize(audio.audioChannels.size());

	for (int i = 0; i < audio.audioChannels.size(); i++)
	{
		m_savedChannels.channels[i].assign(audio.audioChannels[i].begin() + selection.startSample, audio.audioChannels[i].begin() + selection.endSample);
	}
}

void AudioModificator::Cut(AudioMemory& audio, const AudioSelection& selection)
{
	if (audio.audioChannels.empty())
		return;

	if (selection.startSample >= selection.endSample)
		return;

	for (auto& channel : audio.audioChannels)
	{
		if (selection.endSample > channel.size())
			return;
	}

	m_savedChannels.channels.resize(audio.audioChannels.size());

	for (size_t i = 0; i < audio.audioChannels.size(); ++i)
	{
		m_savedChannels.channels[i].assign(
			audio.audioChannels[i].begin() + selection.startSample,
			audio.audioChannels[i].begin() + selection.endSample
		);

		audio.audioChannels[i].erase(
			audio.audioChannels[i].begin() + selection.startSample,
			audio.audioChannels[i].begin() + selection.endSample
		);
	}
}

void AudioModificator::Paste(AudioMemory& audio, unsigned int sampleStart)
{
	if (audio.audioChannels.empty() || m_savedChannels.channels.empty())
		return;

	for (size_t i = 0; i < audio.audioChannels.size(); ++i)
	{
		auto& channel = audio.audioChannels[i];
		const auto& saved = m_savedChannels.channels[i];

		if (sampleStart > channel.size())
		{
			sampleStart = channel.size();
		}

		channel.insert( channel.begin() + sampleStart, saved.begin(), saved.end() );
	}
}


void AudioModificator::Paste(AudioMemory& audio)
{
	audio.audioChannels.resize(m_savedChannels.channels.size());

	for (int i = 0; i < audio.audioChannels.size(); i++)
	{
		audio.audioChannels[i].assign(m_savedChannels.channels[i].begin(), m_savedChannels.channels[i].begin() + m_savedChannels.channels[i].size());
	}
}

void AudioModificator::Reverse(AudioMemory& audio, const AudioSelection& selection)
{
	for (auto& channel : audio.audioChannels)
	{
		std::reverse(channel.begin() + selection.startSample, channel.begin() + selection.endSample);
	}
}

#undef min

void AudioModificator::Mix(AudioMemory& dst, const AudioMemory& src)
{
	if (dst.audioChannels.size() != src.audioChannels.size())
		return;

	for (int i = 0; i < dst.audioChannels.size(); i++)
	{
		int minimumSize = std::min(dst.audioChannels[i].size(), src.audioChannels[i].size());

		for (int j = 0; j < minimumSize; j++)
		{

			float sample = src.audioChannels[i][j];
			sample += dst.audioChannels[i][j];
			sample = AudioUtils::Clamp(sample, -1, 1);
			dst.audioChannels[i][j] = sample;
		}
	}
}


void AudioModificator::Envelope(AudioMemory& audio, std::vector<std::vector<float>>& position)
{
	const int channels = static_cast<int>(audio.audioChannels.size());
	const int frames = static_cast<int>(audio.audioChannels[0].size());
	const float sampleRate = static_cast<float>(audio.pWaveFile->GetWaveForm().format.frequence);

	std::vector<float> sampleData;
	sampleData.reserve(frames * channels);
	 
	for (int i = 0; i < frames; i++)
	{
		for (int ch = 0; ch < channels; ch++)
		{
			sampleData.push_back(audio.audioChannels[ch][i]);
		}
	}
	 
	for (size_t i = 0; i + 1 < position.size(); i++)
	{
		int x1 = static_cast<int>(position[i][0] * sampleRate);
		int x2 = static_cast<int>(position[i + 1][0] * sampleRate);

		float y1 = position[i][1];
		float y2 = position[i + 1][1];
		 
		x1 = AudioUtils::Clamp(x1, 0, frames);
		x2 = AudioUtils::Clamp(x2, 0, frames);
		if (x2 <= x1)
		{
			continue;
		}

		for (int frame = x1; frame < x2; frame++)
		{
			float t = float(frame - x1) / float(x2 - x1);
			float gain = static_cast<float>( AudioUtils::LinearInterpolation(y1, y2, t) );
			 
			for (int ch = 0; ch < channels; ch++)
			{
				int index = frame * channels + ch;
				sampleData[index] *= gain;
			}
		}
	}
	 
	for (int i = 0; i < frames; i++)
	{
		for (int ch = 0; ch < channels; ch++)
		{
			audio.audioChannels[ch][i] = sampleData[i * channels + ch];
		}
	}
}

void AudioModificator::Speed(AudioMemory& audio, float speed)
{
	if (speed <= 0.0f)
		return;

	const int channels = static_cast<int>(audio.audioChannels.size());
	const int srcFrames = static_cast<int>(audio.audioChannels[0].size());
	const int dstFrames = static_cast<int>(srcFrames / speed);

	std::vector<std::vector<float>> newChannels;
	newChannels.resize(channels);
	for (int ch = 0; ch < channels; ch++)
	{
		newChannels[ch].resize(dstFrames);
	}

	for (int i = 0; i < dstFrames; i++)
	{
		float srcPos = i * speed;
		int index1 = static_cast<int>(srcPos);
		int index2 = std::min(index1 + 1, srcFrames - 1);
		float t = srcPos - index1;

		for (int ch = 0; ch < channels; ch++)
		{
			float s1 = audio.audioChannels[ch][index1];
			float s2 = audio.audioChannels[ch][index2];

			newChannels[ch][i] = static_cast<float>( AudioUtils::LinearInterpolation(s1, s2, t) );
		}
	}

	audio.audioChannels = std::move(newChannels);
}

#define min