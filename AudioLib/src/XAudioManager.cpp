#include "XAudioManager.h"
#include <iostream>

#pragma comment(lib,"XAudio2.lib")

XAudioPlayer::XAudioPlayer() : m_xaudio(nullptr), m_masterVoice(nullptr), m_sourceVoice(nullptr), m_is3DInitialized(false)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

XAudioPlayer::~XAudioPlayer()
{
    Stop();

    if (m_masterVoice)
        m_masterVoice->DestroyVoice();

    if (m_xaudio)
        m_xaudio->Release();

    CoUninitialize();
}

bool XAudioPlayer::Initialize()
{
    if (FAILED(XAudio2Create(&m_xaudio, 0)))
        return false;

    if (FAILED(m_xaudio->CreateMasteringVoice(&m_masterVoice)))
        return false;

    DWORD dwChannelMask;
    if (FAILED(m_masterVoice->GetChannelMask(&dwChannelMask)))
        return false;

    if (FAILED(X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance)))
        return false;

    return true;
}

bool XAudioPlayer::Play(const WAVEFORMATEX& format, const std::vector<char>& audioData)
{
    m_format = format;

    Stop();

    if (FAILED(m_xaudio->CreateSourceVoice(&m_sourceVoice, &format)))
        return false;

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = static_cast<UINT32>(audioData.size());
    buffer.pAudioData = reinterpret_cast<const BYTE*>(audioData.data());
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    if (FAILED(m_sourceVoice->SubmitSourceBuffer(&buffer)))
        return false;

    m_sourceVoice->Start();
    return true;
}

bool XAudioPlayer::Play3DSound(const WAVEFORMATEX& format, const std::vector<char>& audioData)
{

    m_format = format;

    Stop();

    if (FAILED(m_xaudio->CreateSourceVoice(&m_sourceVoice, &format)))
        return false;

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = static_cast<UINT32>(audioData.size());
    buffer.pAudioData = reinterpret_cast<const BYTE*>(audioData.data());
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    if (FAILED(m_sourceVoice->SubmitSourceBuffer(&buffer)))
        return false;


    float* pEmitterAzimuths = nullptr;

    // Initialize listener
    m_listener = {};
    m_listener.pCone = NULL;

    _SetListenerPos({ 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

    //Initialize DSP Settings

    m_dspSettings = {};

    XAUDIO2_VOICE_DETAILS masterVoiceDetails;
    m_masterVoice->GetVoiceDetails(&masterVoiceDetails);

    XAUDIO2_VOICE_DETAILS sourceVoiceDetails;
    m_sourceVoice->GetVoiceDetails(&sourceVoiceDetails);

    FLOAT32* p3dAudioMatrix = new float[sourceVoiceDetails.InputChannels * masterVoiceDetails.InputChannels];
    memset(p3dAudioMatrix, 0, sizeof(float) * sourceVoiceDetails.InputChannels * masterVoiceDetails.InputChannels);

    m_dspSettings.SrcChannelCount     = sourceVoiceDetails.InputChannels;
    m_dspSettings.DstChannelCount     = masterVoiceDetails.InputChannels;
    m_dspSettings.pMatrixCoefficients = p3dAudioMatrix;
    m_dspSettings.pDelayTimes         = NULL;


    //Initialize emitter
    m_emitter = {};
    m_emitter.pLFECurve           = NULL;
    m_emitter.pLPFDirectCurve     = NULL;
    m_emitter.pLPFReverbCurve     = NULL;
    m_emitter.pVolumeCurve        = NULL;
    m_emitter.pReverbCurve        = NULL;

    _SetEmitterPos({0.0f, 0.0f, -1.0f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

    m_emitter.InnerRadius         = 2.0f;
    m_emitter.InnerRadiusAngle    = X3DAUDIO_PI / 4.0f;

    m_emitter.CurveDistanceScaler = 1.0f;
    m_emitter.DopplerScaler       = 1.0f;

    m_emitter.ChannelRadius       = 10.0f;
    m_emitter.ChannelCount        = format.nChannels;

    if (format.nChannels == 1)
    {
        m_emitter.pCone = NULL;
    }
    else
    {
        pEmitterAzimuths  = new float[m_emitter.ChannelCount];
        memset(pEmitterAzimuths, 0, m_emitter.ChannelCount * sizeof(float));
        m_emitter.pChannelAzimuths = pEmitterAzimuths;
    }

    X3DAudioCalculate(m_X3DInstance, &m_listener, &m_emitter, X3DAUDIO_CALCULATE_MATRIX, &m_dspSettings);

    m_masterVoice->GetVoiceDetails(&masterVoiceDetails);


    HRESULT hr = m_sourceVoice->SetOutputMatrix(m_masterVoice, format.nChannels, masterVoiceDetails.InputChannels, m_dspSettings.pMatrixCoefficients);

    if (FAILED(hr))
        return false;
    
    delete[] p3dAudioMatrix;

    if (pEmitterAzimuths != nullptr)
        delete[] pEmitterAzimuths;

    m_is3DInitialized = true;

    m_sourceVoice->Start();
    return true;
}

void XAudioPlayer::Stop()
{
    if (m_sourceVoice)
    {
        m_sourceVoice->Stop();
        m_sourceVoice->DestroyVoice();
        m_sourceVoice = nullptr;
    }
}

void XAudioPlayer::SetEmitterPos(X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR top, X3DAUDIO_VECTOR pos, X3DAUDIO_VECTOR velocity)
{
    if (!m_is3DInitialized || m_sourceVoice == nullptr)
        return;

    _SetEmitterPos(front, top, pos, velocity);

    X3DAudioCalculate(m_X3DInstance, &m_listener, &m_emitter, X3DAUDIO_CALCULATE_MATRIX, &m_dspSettings);

    XAUDIO2_VOICE_DETAILS masterVoiceDetails;
    m_masterVoice->GetVoiceDetails(&masterVoiceDetails);


    HRESULT hr = m_sourceVoice->SetOutputMatrix(m_masterVoice, m_format.nChannels, masterVoiceDetails.InputChannels, m_dspSettings.pMatrixCoefficients);

    if (FAILED(hr))
        return;

    Sleep(1);

}

void XAudioPlayer::SetListenerPos(X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR top, X3DAUDIO_VECTOR pos, X3DAUDIO_VECTOR velocity)
{
    if(!m_is3DInitialized || m_sourceVoice == nullptr)
		return;

    _SetListenerPos(front, top, pos, velocity);

    X3DAudioCalculate(m_X3DInstance, &m_listener, &m_emitter, X3DAUDIO_CALCULATE_MATRIX, &m_dspSettings);

    XAUDIO2_VOICE_DETAILS masterVoiceDetails;
    m_masterVoice->GetVoiceDetails(&masterVoiceDetails);


    HRESULT hr = m_sourceVoice->SetOutputMatrix(m_masterVoice, m_format.nChannels, masterVoiceDetails.InputChannels, m_dspSettings.pMatrixCoefficients);

    if (FAILED(hr))
        return;

    Sleep(1);
}

void XAudioPlayer::_SetEmitterPos(X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR top, X3DAUDIO_VECTOR pos, X3DAUDIO_VECTOR velocity)
{
    m_emitter.OrientFront   = front;
    m_emitter.OrientTop     = top;
    m_emitter.Position      = pos;
    m_emitter.Velocity      = velocity;
}

void XAudioPlayer::_SetListenerPos(X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR top, X3DAUDIO_VECTOR pos, X3DAUDIO_VECTOR velocity)
{
    m_listener.OrientFront  = front;
    m_listener.OrientTop    = top;
    m_listener.Position     = pos;
    m_listener.Velocity     = velocity;
}
