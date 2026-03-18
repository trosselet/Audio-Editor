#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <RtMidi.h>

class RTMidiManager
{
public:
	RTMidiManager(unsigned int outPort = 0);
	~RTMidiManager();

	void PlayNote(int note, int velocity, int duration_ms, int channel = 0);
	void StopNote(int note, int velocity = 64, int channel = 0);
	void SendControlChange(int control, int value, int channel = 0);
	void ProgramChange(int program, int channel = 0);



private:
	RtMidiIn* m_pRtMidiIn = nullptr;
	RtMidiOut* m_pRtMidiOut = nullptr;

	void PrintAvailablePorts();
};