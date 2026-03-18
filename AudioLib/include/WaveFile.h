#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <xaudio2.h>

enum eAudioFormat : short
{
    PCM_INT = 1,
    PCM_FLOAT = 3,
};

#pragma pack(push, 1)

struct WAVEHEADER
{
    char fileTypeBlockID[4];
    int  fileSize;
    char fileFormatID[4];
};

struct AUDIOFORMAT
{
    char  formatBlockID[4];
    int   blockSize;
    short audioFormat;
    short nbrChannel;
    int   frequence;
    int   bytePerSec;
    short bytePerBlock;
    short bitsPerSample;
};

struct AUDIODATA
{
    char dataBlockId[4];
    int  dataSize;
};

struct WAVEFORM
{
    WAVEHEADER  header;
    AUDIOFORMAT format;
};

#pragma pack(pop)

class WaveFile
{
public:
    WaveFile();
    ~WaveFile();

    bool Open(const std::string& name);
    void Close();

    bool Save(const std::string& name) const;

    WAVEFORMATEX GetWaveFormat() const;
    const std::vector<char>& GetAudioData() const;
	void SetAudioData(const std::vector<char>& data) { m_audioBuffer = data; }
	void SetAudioDataSize(const int& size) { m_audioBuffer.resize(size); }

	WAVEFORM GetWaveForm() const { return m_waveForm; }

private:
    WAVEFORM            m_waveForm;
    AUDIODATA           m_audioData;
    std::fstream        m_file;
    std::vector<char>   m_audioBuffer;
};
