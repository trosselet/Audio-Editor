# Audio Editor Application

## Overview
This project is a custom **audio editor** built from scratch using low-level and lightweight libraries. It focuses on providing real-time audio manipulation capabilities with an intuitive interface and support for MIDI input.

The application leverages:
- **XAudio2** for audio processing and playback
- **ImGui** for the graphical user interface
- **RTMidi** for MIDI keyboard integration

---

## Architecture

### 1. Audio Core (XAudio2)
The **Audio Core** is responsible for handling all audio playback and processing using XAudio2.

#### Features:
- Real-time audio playback
- Multiple audio track support
- Buffer management
- Audio speed modification
- Reverse playback

This module ensures low-latency and efficient audio processing.

---

### 2. Track System
The editor allows manipulation of multiple audio tracks independently.

#### Features:
- Multi-track editing
- Copy / Cut / Paste operations
- Non-destructive editing workflow (depending on implementation)
- Timeline-based audio organization
- Per-track transformations

This system provides flexibility similar to basic digital audio workstations (DAWs).

---

### 3. Audio Effects & Processing
The project includes several built-in audio manipulation features.

#### Features:
- Reverse audio segments
- Speed adjustment (time scaling)
- 3D spatialization
- Envelope control (volume shaping over time)

These features allow users to creatively modify audio directly within the editor.

---

### 4. MIDI Integration (RTMidi)
The editor supports MIDI input via RTMidi.

#### Features:
- MIDI keyboard input detection
- Real-time interaction with audio
- Potential for triggering sounds or controlling parameters

This enables integration with external hardware for more interactive workflows.

---

### 5. User Interface (ImGui)
The entire interface is built using **ImGui**, providing a fast and responsive editor experience.

#### Features:
- Timeline visualization
- Track management UI
- Interactive controls for editing operations
- Debug-friendly interface

The UI is designed for rapid iteration and ease of use.

---

## Build System

The project uses **CMake** for configuration and building.

### Requirements:
- CMake (3.x or higher)
- Visual Studio (with C++ support)
- Windows OS
- XAudio2 SDK (included with Windows)
- ImGui (included)
- RTMidi library

### Build Instructions:

```bash
# Clone the repository
git clone <repository_url>
cd <project_folder>
```

Build with CMake to run the project.
