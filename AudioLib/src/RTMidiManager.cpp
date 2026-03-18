#include "RTMidiManager.h"
#include <iostream>

RTMidiManager::RTMidiManager(unsigned int outPort)
{
    try {
        m_pRtMidiIn = new RtMidiIn();
        m_pRtMidiOut = new RtMidiOut();
    }
    catch (RtMidiError& error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

	PrintAvailablePorts();
     
    unsigned int nPorts = m_pRtMidiOut->getPortCount();
    if (nPorts == 0) {
        std::cout << "No MIDI Output port available.\n";
        exit(EXIT_FAILURE);
    }

    if (outPort >= nPorts) 
    {
        outPort = 0;
    }

    m_pRtMidiOut->openPort(outPort);
}

RTMidiManager::~RTMidiManager()
{
	if (m_pRtMidiIn)
	{
		delete m_pRtMidiIn;
		m_pRtMidiIn = nullptr;
	}
	if (m_pRtMidiOut)
	{
		delete m_pRtMidiOut;
		m_pRtMidiOut = nullptr;
	}
}

void RTMidiManager::PrintAvailablePorts()
{
    unsigned int nPorts = m_pRtMidiIn->getPortCount();
    std::cout << "Input MIDI ports:\n";
    for (unsigned int i = 0; i < nPorts; ++i) {
        try {
            std::cout << "  Input #" << i << ": " << m_pRtMidiIn->getPortName(i) << '\n';
        }
        catch (RtMidiError& error) { error.printMessage(); }
    }

    nPorts = m_pRtMidiOut->getPortCount();
    std::cout << "Output MIDI ports:\n";
    for (unsigned int i = 0; i < nPorts; ++i) {
        try {
            std::cout << "  Output #" << i << ": " << m_pRtMidiOut->getPortName(i) << '\n';
        }
        catch (RtMidiError& error) { error.printMessage(); }
    }
}

void RTMidiManager::PlayNote(int note, int velocity, int duration_ms, int channel)
{
	if (!m_pRtMidiOut) 
        return;

    std::vector<unsigned char> message = 
    {
		static_cast<unsigned char>(0x90 | (channel & 0x0F)),
		static_cast<unsigned char>(note & 0x7F),
		static_cast<unsigned char>(velocity & 0x7F)
    };

	m_pRtMidiOut->sendMessage(&message);

    if (duration_ms > 0)
    {
        std::thread
        (
            [this, note, velocity, channel, duration_ms]() 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
                this->StopNote(note, velocity, channel);
            }
        ).detach();
    }
}

void RTMidiManager::StopNote(int note, int velocity, int channel)
{
    if (!m_pRtMidiOut) 
        return;
    std::vector<unsigned char> message = 
    {
        static_cast<unsigned char>(0x80 | (channel & 0x0F)),
        static_cast<unsigned char>(note & 0x7F),
        static_cast<unsigned char>(velocity & 0x7F)
    };

    m_pRtMidiOut->sendMessage(&message);
}

void RTMidiManager::SendControlChange(int control, int value, int channel)
{
    if (!m_pRtMidiOut) 
        return;

    std::vector<unsigned char> message = 
    {
        static_cast<unsigned char>(0xB0 | (channel & 0x0F)),
        static_cast<unsigned char>(control & 0x7F),
        static_cast<unsigned char>(value & 0x7F)
    };

    m_pRtMidiOut->sendMessage(&message);
}

void RTMidiManager::ProgramChange(int program, int channel)
{
    if (!m_pRtMidiOut) 
        return;

    std::vector<unsigned char> message = 
    {
        static_cast<unsigned char>(0xC0 | (channel & 0x0F)),
        static_cast<unsigned char>(program & 0x7F)
    };

    m_pRtMidiOut->sendMessage(&message);
}
