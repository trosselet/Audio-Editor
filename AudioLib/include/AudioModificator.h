#pragma once

#include "AudioMemory.h"
#include <vector>

class AudioSaveChannel
{
public:
    std::vector<std::vector<float>> channels;
};

class AudioModificator
{
public:
    static void Copy(const AudioMemory& audio, const AudioSelection& selection);
    static void Cut(AudioMemory& audio, const AudioSelection& selection);
    static void Paste(AudioMemory& audio, unsigned int sampleStart);
    static void Paste(AudioMemory& audio);
    static void Reverse(AudioMemory& audio, const AudioSelection& selection);
    static void Mix(AudioMemory& dst, const AudioMemory& src);

    static void Envelope(AudioMemory& audio, std::vector<std::vector<float>>& position);
    static void Speed(AudioMemory& audio, float speed);

private:
	static AudioSaveChannel m_savedChannels;
};


