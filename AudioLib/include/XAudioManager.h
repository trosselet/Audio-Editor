#pragma once

#include <xaudio2.h>
#include <x3daudio.h>
#include <vector>
#include <windows.h>

class XAudioPlayer
{
public:
    XAudioPlayer();
    ~XAudioPlayer();

    bool Initialize();
    bool Play(const WAVEFORMATEX& format, const std::vector<char>& audioData);
    bool Play3DSound(const WAVEFORMATEX& format, const std::vector<char>& audioData);
    void Stop();

    void SetEmitterPos (X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR top, X3DAUDIO_VECTOR pos, X3DAUDIO_VECTOR velocity);
    void SetListenerPos(X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR top, X3DAUDIO_VECTOR pos, X3DAUDIO_VECTOR velocity);

private:
    void _SetEmitterPos (X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR top, X3DAUDIO_VECTOR pos, X3DAUDIO_VECTOR velocity);
    void _SetListenerPos(X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR top, X3DAUDIO_VECTOR pos, X3DAUDIO_VECTOR velocity);

private:
    IXAudio2*                   m_xaudio;
    IXAudio2MasteringVoice*     m_masterVoice;
    IXAudio2SourceVoice*        m_sourceVoice;
    WAVEFORMATEX                m_format;
    X3DAUDIO_HANDLE             m_X3DInstance;
    X3DAUDIO_LISTENER           m_listener;
    X3DAUDIO_EMITTER            m_emitter;
    X3DAUDIO_DSP_SETTINGS       m_dspSettings;
	bool 					    m_is3DInitialized;
};
