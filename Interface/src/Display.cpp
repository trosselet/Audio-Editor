#include "Display.h"
#include <Windows.h>
#include <iostream>
#include <unordered_set>
#include <ImGuiFileDialog.h>

#include "WaveFile.h"
#include "AudioMemory.h"
#include "XAudioManager.h"
#include "Utils.h"
#include "AudioModificator.h"

Display::~Display()
{
	for (TRACK* pTrack : m_tracks)
	{
		delete pTrack;
		pTrack = nullptr;
	}

	if (m_pRtMidiManager != nullptr)
	{
		delete m_pRtMidiManager;
		m_pRtMidiManager = nullptr;
	}
}

void Display::Init(GLFWwindow* window, const char* glsl_version) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();

	m_player.Initialize();
}

void Display::NewFrame() {
	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void Display::Update() {
	ImGui::DockSpaceOverViewport();

	_MainMenu();
	ImGui::Begin("Tools");
	_CreateTools();
	ImGui::End();
	ImGui::Begin("View");
	if (!isUsingMIDI)
	{
		_DisplayGraphs();
		_DisplayMarker();

		if (m_show3DSoundWindow)
			_Display3DSoundWindow();
	}
	else
	{
		_DisplayMidiKeyboard();
	}
	ImGui::End();
}

void Display::Render() {
	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Display::Shutdown() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


void Display::_MainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//if (ImGui::MenuItem("New"))
			//{

			//}

			if (ImGui::MenuItem("Open"))
			{
				IGFD::FileDialogConfig config; config.path = "../res";
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".wav", config);
			}
			ImGui::EndMenu();
		}

		_CreateEditMenu();
		_CreateSecretMenu();

		ImGui::EndMainMenuBar();
	}
}

void Display::_DisplayMarker()
{
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	ImVec2 origin = ImGui::GetCursorScreenPos();
}

void Display::_Display3DSoundWindow()
{
	if (!ImGui::Begin("3D Sound", &m_show3DSoundWindow))
	{
		ImGui::End();
		return;
	}

	ImVec2 size(200, 200);
	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImDrawList* draw = ImGui::GetWindowDrawList();
	 
	draw->AddRectFilled(origin, ImVec2(origin.x + size.x, origin.y + size.y), IM_COL32(40, 40, 40, 255));
	draw->AddRect(origin, ImVec2(origin.x + size.x, origin.y + size.y), IM_COL32(255, 255, 255, 255));
	 
	ImGui::InvisibleButton("##3DSlider", size);
	if (ImGui::IsItemActive())
	{
		ImVec2 mouse = ImGui::GetIO().MousePos;
		float nx = ((mouse.x - origin.x) / size.x) * 2.0f - 1.0f;
		float ny = (1.0f - (mouse.y - origin.y) / size.y) * 2.0f - 1.0f;

		m_3DSound[0] = AudioUtils::Clamp(nx, -1.0f, 1.0f);
		m_3DSound[1] = AudioUtils::Clamp(ny, -1.0f, 1.0f);
	}
	 
	if (ImGui::IsItemHovered())
	{
		m_3DSound[2] += ImGui::GetIO().MouseWheel * 0.05f;
		m_3DSound[2] = AudioUtils::Clamp(m_3DSound[2], -1.0f, 1.0f);
	}
	 
	ImVec2 point
	(
		origin.x + ((m_3DSound[0] + 1.0f) * 0.5f) * size.x,
		origin.y + ((1.0f - (m_3DSound[1] + 1.0f) * 0.5f)) * size.y
	);

	draw->AddCircleFilled(point, 6.0f, IM_COL32(255, 100, 100, 255));

	float multiplicator = 20.f;

	ImGui::Spacing();
	ImGui::Text("X %.2f  Y %.2f  Z %.2f", m_3DSound[0] * multiplicator, m_3DSound[1] * multiplicator, m_3DSound[2]);

	ImGui::Text("Z Position");
	ImGui::SliderFloat("##zPosition", &m_3DSound[2], -1.0f * multiplicator, 1.0f * multiplicator);

	m_player.SetEmitterPos({ 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { m_3DSound[0] * multiplicator, m_3DSound[1] * multiplicator, m_3DSound[2] }, { 0.0f, 0.0f, 0.0f });

	ImGui::End();
}

void Display::_DisplayMidiKeyboard()
{
	if (!m_pRtMidiManager)
		return;

	std::vector<int> notes = { 60, 62, 64, 65, 67, 69, 71, 72, 74, 75, 77, 79, 81, 83, 84 };
	static int currentInstrumentIndex = 0;
	static float volume = 90.0f;          

	ImGui::Text("MIDI Keyboard");
	ImGui::NewLine();

	ImGui::Text("Volume");
	ImGui::SliderFloat("##volume", &volume, 0.0f, 127.0f);

	 
	 
	ImGui::Text("Instrument");
	if (ImGui::BeginCombo("##instrument", sMidiInstrumentNames[currentInstrumentIndex]))
	{
		for (int n = 0; n < IM_ARRAYSIZE(sMidiInstrumentNames); n++)
		{
			bool is_selected = (currentInstrumentIndex == n);
			if (ImGui::Selectable(sMidiInstrumentNames[n], is_selected))
			{
				currentInstrumentIndex = n; 
				if (m_pRtMidiManager)
					m_pRtMidiManager->ProgramChange(static_cast<int>(MidiInstrument::AcousticGrandPiano) + n);
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	 

	ImGui::BeginGroup();

	for (int note : notes)
	{
		std::string buttonLabel = std::to_string(note);

		if (ImGui::Button(buttonLabel.c_str(), ImVec2(40, 100)))
		{
			m_pRtMidiManager->PlayNote(note, static_cast<int>(volume), 100);
		}

		ImGui::SameLine();
	}

	ImGui::EndGroup();
}


void Display::_CreateEditMenu()
{
	bool editEnabled = m_selection.pSelectedTrack != nullptr;
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Cut", 0, false, &editEnabled))
		{
			if (m_selection.pSelectedTrack != nullptr)
			{
				float sampleRate = m_selection.pSelectedTrack->pAudioMemory->pWaveFile->GetWaveForm().format.frequence + 1;
				AudioModificator::Cut(*m_selection.pSelectedTrack->pAudioMemory, AudioSelection(m_selection.selection[0] * sampleRate, m_selection.selection[1] * sampleRate));
				m_selection.pSelectedTrack->pAudioMemory->ToWaveFile();
			}
		}
		if (ImGui::MenuItem("Copy", 0, false, &editEnabled))
		{

		}
		if (ImGui::MenuItem("Paste"))
		{

		}
		if (ImGui::BeginMenu("Effects"))
		{
			if (ImGui::MenuItem("Reverse", 0, false, &editEnabled))
			{
				if (m_selection.pSelectedTrack != nullptr)
				{
					float sampleRate = m_selection.pSelectedTrack->pAudioMemory->pWaveFile->GetWaveForm().format.frequence + 1;
					AudioModificator::Reverse(*m_selection.pSelectedTrack->pAudioMemory, AudioSelection(m_selection.selection[0] * sampleRate, m_selection.selection[1] * sampleRate));
					m_selection.pSelectedTrack->pAudioMemory->ToWaveFile();
				}
			}
			if (ImGui::MenuItem("3D Sound", 0, false, &editEnabled))
			{
				m_show3DSoundWindow = !m_show3DSoundWindow;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

void Display::_CreateSecretMenu()
{
	if (ImGui::BeginMenu("Secret"))
	{
		if (ImGui::MenuItem("MIDI Input Test"))
		{
			if (m_pRtMidiManager == nullptr)
			{
				m_pRtMidiManager = new RTMidiManager(0);
			}

			if (!isUsingMIDI)
			{
				isUsingMIDI = true;
			}
			else
			{
				isUsingMIDI = false;
				delete m_pRtMidiManager;
				m_pRtMidiManager = nullptr;
			}
		}
		ImGui::EndMenu();
	}
}

void Display::_CreateTools()
{

	if (ImGui::Button("Play", ImVec2(100, 100)))
	{
		if (m_tracks.size() == 0)
			return;

		//AudioMemory all(*m_tracks[0]->pAudioMemory);
		//for (TRACK* track : m_tracks)
		//{
		//	AudioModificator::Mix(all, *track->pAudioMemory);
		//}
		//WaveFile allFile;
		//all.ToWaveFile(allFile);
		if (!m_show3DSoundWindow)
			m_player.Play(m_tracks[0]->pAudioMemory->pWaveFile->GetWaveFormat(), m_tracks[0]->pAudioMemory->pWaveFile->GetAudioData());
		else
			m_player.Play3DSound(m_tracks[0]->pAudioMemory->pWaveFile->GetWaveFormat(), m_tracks[0]->pAudioMemory->pWaveFile->GetAudioData());
	}

	ImGui::SameLine();
	ImGui::Button("Pause", ImVec2(100, 100));
	ImGui::SameLine();
	if(ImGui::Button("Stop", ImVec2(100, 100)))
	{
		m_player.Stop();
	}

	_FileDialog();
}

void Display::_DisplayGraphs()
{

	for (int i = 0; i < m_tracks.size(); ++i)
	{
		ImGui::PushID(i);
		ImGui::BeginGroup();
		ImGui::BeginGroup();

		if (ImGui::Button("Mute", ImVec2(50, 97))) {}
		ImGui::Button("Solo", ImVec2(50, 100));
		ImGui::EndGroup();
		ImGui::SameLine();
		ImDrawList* pDrawList = ImGui::GetWindowDrawList();
		ImVec2 origin = ImGui::GetCursorScreenPos();
		ImVec2 size = ImGui::GetContentRegionMax();
		pDrawList->AddRectFilled(origin, ImVec2(size.x, origin.y + 200), IM_COL32(50, 50, 50, 255));
		pDrawList->AddRect(origin, ImVec2(size.x, origin.y + 200), IM_COL32(255, 255, 255, 255), 0.0f, 0, 0.5f);

		_GenerateGraph(origin, i);
		pDrawList->AddPolyline(m_tracks[i]->pPoints, m_tracks[i]->size, IM_COL32(255, 255, 255, 255), 0, 2.f);
		pDrawList->AddRectFilled(ImVec2(m_tracks[i]->pPoints[0].x, origin.y), ImVec2(m_tracks[i]->pPoints[m_tracks[i]->size/2].x, origin.y + 200), IM_COL32(0, 0, 255, 50));
		pDrawList->AddRect(ImVec2(m_tracks[i]->pPoints[0].x, origin.y), ImVec2(m_tracks[i]->pPoints[m_tracks[i]->size / 2].x, origin.y + 200), IM_COL32(0, 0, 255, 100));
		
		_ManageSelection(i, origin, size);

		ImGui::PopID();
		ImGui::EndGroup();
		ImGui::Separator();

	}
	
}

void Display::_FileDialog()
{
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) { // => will show a dialog
		if (ImGuiFileDialog::Instance()->IsOk()) 
		{ // action if OK
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			_OpenFile(filePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void Display::_OpenFile(std::string path)
{
	WaveFile* file = new WaveFile();
	file->Open(path);

	AudioMemory* pMem = new AudioMemory();
	pMem->FromWaveFile(*file);
	TRACK* pTrack = new TRACK();
	pTrack->pAudioMemory = pMem;

	m_tracks.push_back(pTrack);
}

inline float linear_interpolation(float y1, float y2, float x)
{
	return y1 + (x * (y2 - y1));
}

void Display::_GenerateGraph(ImVec2 const& origin, int index)
{
	AudioMemory* pAudioMemory = m_tracks[index]->pAudioMemory;
	ImVec2 p0 = ImVec2(origin.x, origin.y + 200 / 2);

	size_t size = pAudioMemory->audioChannels[0].size();

	float ratio = m_tracks[index]->pAudioMemory->pWaveFile->GetWaveForm().format.frequence - 1;
	int num = size / ratio * 2;
	ImVec2* points = new ImVec2[num];
	points[0] = p0;
	int bufSize = 0;
	for (int i = 1; i < num / 2+ 1; ++i)
	{
		float y1 = 0;
		float y2 = 0;
		if (i * ratio > pAudioMemory->audioChannels[0].size())
		{
			y1 = pAudioMemory->audioChannels[0][i / ratio];
			y2 = pAudioMemory->audioChannels[0][i];
		}
		else
		{
			y1 = pAudioMemory->audioChannels[0][i];
			y2 = pAudioMemory->audioChannels[0][i * ratio];
		}
		float yPoint = max(0,linear_interpolation(y1, y2, i));
		points[i] = ImVec2(origin.x + i, origin.y + 200 / 2 - yPoint * 5);
		points[num - i] = ImVec2(origin.x + i, origin.y + 200 / 2 + yPoint * 5);
		bufSize += 2;
	}
	points[num - 1] = p0;

	m_tracks[index]->pPoints = points;
	m_tracks[index]->size = bufSize;
}

void Display::_ManageSelection(int index, ImVec2 const& origin, ImVec2 const& size)
{
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	ImGui::InvisibleButton("canvas", ImVec2(size.x, origin.y), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool is_hovered = ImGui::IsItemHovered();
	const bool is_active = ImGui::IsItemActive();
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

	if(is_hovered)
		ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_TextInput);

	//if (!is_hovered && !m_tracks[index]->adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	//{
	//	m_selection.pSelectedTrack = nullptr;
	//	m_selection.selection[0] = 0;
	//	m_selection.selection[1] = 0;
	//	m_tracks[index]->points[0] = ImVec2(0,0);
	//	m_tracks[index]->points[1] = ImVec2(0,0);
	//	m_tracks[index]->points[2] = ImVec2(0,0); 
	//	m_tracks[index]->points[3] = ImVec2(0,0);
	//}

	if (is_hovered && !m_tracks[index]->adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_tracks[index]->points[0] = ImVec2(origin.x + mouse_pos_in_canvas.x, origin.y);
		m_tracks[index]->points[1] = ImVec2(origin.x + mouse_pos_in_canvas.x, origin.y + 200);
		m_tracks[index]->points[2] = ImVec2(mouse_pos_in_canvas.x, 200);
		m_tracks[index]->points[3] = ImVec2(mouse_pos_in_canvas.x, 0 );
		m_tracks[index]->adding_line = true;
	}

	if (m_tracks[index]->adding_line)
	{
		m_tracks[index]->points[2] = ImVec2(origin.x + mouse_pos_in_canvas.x, origin.y + 200);
		m_tracks[index]->points[3] = ImVec2(origin.x + mouse_pos_in_canvas.x, origin.y);

		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			m_selection.pSelectedTrack = m_tracks[index];
			m_selection.selection[0] = m_tracks[index]->points[0].x - origin.x;
			m_selection.selection[1] = m_tracks[index]->points[2].x - origin.x;

			m_tracks[index]->adding_line = false;
		}
	}

	pDrawList->AddConvexPolyFilled(m_tracks[index]->points, 4, IM_COL32(80, 80, 80, 80));
}