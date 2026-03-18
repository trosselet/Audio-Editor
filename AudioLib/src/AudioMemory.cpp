#include "AudioMemory.h"

#include <Utils.h>

AudioMemory::AudioMemory()
{
}

AudioMemory::~AudioMemory()
{
}

void AudioMemory::FromWaveFile(WaveFile& waveFile)
{
    pWaveFile = &waveFile;
    const AUDIOFORMAT& fmt = waveFile.GetWaveForm().format;
    const auto& buffer = waveFile.GetAudioData();

    int numChannels = fmt.nbrChannel;
    int bytesPerSample = fmt.bitsPerSample / 8;
    int numFrames = buffer.size() / (numChannels * bytesPerSample);

    audioChannels.resize(numChannels);
    for (auto& ch : audioChannels)
    {
        ch.resize(numFrames);
    }

    if (fmt.audioFormat == PCM_INT && fmt.bitsPerSample == 16)
    {
        const int16_t* samples = reinterpret_cast<const int16_t*>(buffer.data());

        for (int i = 0; i < numFrames; ++i)
        {
            for (int ch = 0; ch < numChannels; ++ch)
            {
                int16_t s = samples[i * numChannels + ch];
                audioChannels[ch][i] = s / 32768.0f;
            }
        }
    }
    else if (fmt.audioFormat == PCM_FLOAT && fmt.bitsPerSample == 32)
    {
        const float* samples = reinterpret_cast<const float*>(buffer.data());

        for (int i = 0; i < numFrames; ++i)
        {
            for (int ch = 0; ch < numChannels; ++ch)
            {
                audioChannels[ch][i] =
                    samples[i * numChannels + ch];
            }
        }
    }

}

void AudioMemory::ToWaveFile() const
{

	AUDIOFORMAT format = pWaveFile->GetWaveForm().format;

    if (format.audioFormat == PCM_INT && format.bitsPerSample == 16)
    {
        std::vector<char> out;
        out.resize(audioChannels[0].size() * format.nbrChannel * sizeof(int16_t));

        int16_t* samples = reinterpret_cast<int16_t*>(out.data());

        for (int i = 0; i < audioChannels[0].size(); ++i)
        {
            for (int ch = 0; ch < format.nbrChannel; ++ch)
            {
                float s = AudioUtils::Clamp(audioChannels[ch][i], -1.0f, 1.0f);
                samples[i * format.nbrChannel + ch] = static_cast<int16_t>(s * 32767.0f);
            }
        }

        pWaveFile->SetAudioData(out);
    }
    else if (format.audioFormat == PCM_FLOAT && format.bitsPerSample == 32)
    {
        std::vector<char> out;
        out.resize(audioChannels[0].size() * format.nbrChannel * sizeof(float));

        float* samples = reinterpret_cast<float*>(out.data());

        for (int i = 0; i < audioChannels[0].size(); ++i)
        {
            for (int ch = 0; ch < format.nbrChannel; ++ch)
            {
                samples[i * format.nbrChannel + ch] = audioChannels[ch][i];
            }
        }

        pWaveFile->SetAudioData(out);
    }
}
