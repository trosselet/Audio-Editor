#include "WaveFile.h"
#include <cstring>

WaveFile::WaveFile()
    : m_waveForm{}, m_audioData{}, m_file(), m_audioBuffer()
{
}

WaveFile::~WaveFile()
{
    Close();
}

bool WaveFile::Open(const std::string& name)
{
    m_file.open(name, std::ios::binary | std::ios::in);
    if (!m_file.is_open())
    {
        std::cout << "Cannot open file: " << name << std::endl;
        return false;
    }
     
    m_file.read(reinterpret_cast<char*>(&m_waveForm), sizeof(WAVEFORM));
     
    while (m_file.read(reinterpret_cast<char*>(&m_audioData), sizeof(AUDIODATA)))
    {
        if (std::memcmp(m_audioData.dataBlockId, "data", 4) == 0)
        {
            m_audioBuffer.resize(m_audioData.dataSize);
            m_file.read(m_audioBuffer.data(), m_audioData.dataSize);
            return true;
        }
        else
        { 
            m_file.seekg(m_audioData.dataSize, std::ios::cur);
        }
    }

    return false;
}

void WaveFile::Close()
{
    if (m_file.is_open())
        m_file.close();
}

WAVEFORMATEX WaveFile::GetWaveFormat() const
{
    WAVEFORMATEX wfx = {};

    if (m_waveForm.format.audioFormat == PCM_FLOAT)
    {
        wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;

    }
    else
    {
        wfx.wFormatTag = WAVE_FORMAT_PCM;
    }

    wfx.nChannels = m_waveForm.format.nbrChannel;
    wfx.nSamplesPerSec = m_waveForm.format.frequence;
    wfx.wBitsPerSample = m_waveForm.format.bitsPerSample;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;
    return wfx;
}

const std::vector<char>& WaveFile::GetAudioData() const
{
    return m_audioBuffer;
}

bool WaveFile::Save(const std::string& name) const
{
    if (m_audioBuffer.empty())
        return false;

    std::ofstream out(name, std::ios::binary);
    if (!out.is_open())
        return false;

    WAVEHEADER header = {};
    std::memcpy(header.fileTypeBlockID, "RIFF", 4);
    std::memcpy(header.fileFormatID, "WAVE", 4);

    header.fileSize =
        sizeof(WAVEHEADER) - 8 +
        sizeof(AUDIOFORMAT) +
        sizeof(AUDIODATA) +
        static_cast<int>(m_audioBuffer.size());

    AUDIOFORMAT format = m_waveForm.format;
    std::memcpy(format.formatBlockID, "fmt ", 4);
    format.blockSize = 16;

    AUDIODATA data = {};
    std::memcpy(data.dataBlockId, "data", 4);
    data.dataSize = static_cast<int>(m_audioBuffer.size());

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    out.write(reinterpret_cast<const char*>(&format), sizeof(format));
    out.write(reinterpret_cast<const char*>(&data), sizeof(data));
    out.write(m_audioBuffer.data(), m_audioBuffer.size());

    out.close();
    return true;
}
