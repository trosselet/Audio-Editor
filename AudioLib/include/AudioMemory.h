#pragma once

#include <vector>
#include "WaveFile.h"

struct AudioSelection
{
	unsigned int    startSample;
	unsigned int    endSample;

	AudioSelection(unsigned int _startSample, unsigned int _endSample)
	{
		startSample = _startSample, endSample = _endSample;
	}
	AudioSelection()
	{
		startSample = 0, endSample = 0;
	}
};

class AudioMemory
{
public:
	AudioMemory();
	~AudioMemory();

	void FromWaveFile(WaveFile& waveFile);
	void ToWaveFile() const;
	
	std::vector<std::vector<float>> audioChannels;
	AudioSelection selection;

	WaveFile* pWaveFile = nullptr;
};

