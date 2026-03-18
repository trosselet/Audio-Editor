#include "Display.h"
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "RTMidiManager.h"
#include <windows.h>
#include <map>

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main()
{
	// Setup window
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return 1;
	
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Bonne Chance - Music", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  // tie window context to glad's opengl funcs
		throw("Unable to context to OpenGL");
	
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);
	
	Display myimgui;
	myimgui.Init(window, glsl_version);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	
		glClear(GL_COLOR_BUFFER_BIT);
		myimgui.NewFrame();
		myimgui.Update();
		myimgui.Render();
		glfwSwapBuffers(window);
	
	}
	myimgui.Shutdown();

    //RTMidiManager midiManager;

    //midiManager.ProgramChange(44);
    //midiManager.SendControlChange(5, 200);
    // 
    //std::map<int, int> keyToNote = {
    //    {'A', 60}, 
    //    {'Z', 62}, 
    //    {'E', 64}, 
    //    {'R', 65}, 
    //    {'T', 67}, 
    //    {'Y', 69}, 
    //    {'U', 71}, 
    //    {'I', 72}, 
    //    {'O', 74}, 
    //    {'P', 75}, 
    //    {'S', 78}, 
    //    {'D', 80}, 
    //    {'F', 82}, 
    //    {'G', 83}, 
    //    {'H', 84}, 
    //    {'J', 86}, 
    //    {'K', 87}, 
    //    {'L', 90}, 
    //    {'M', 92}, 
    //};
    // 
    //std::map<int, bool> keyState;

    //while (true)
    //{
    //    for (auto& [key, note] : keyToNote)
    //    {
    //        SHORT state = GetAsyncKeyState(key);
    //        bool isPressed = (state & 0x8000) != 0;

    //        if (isPressed && !keyState[key])
    //        { 
    //            midiManager.PlayNote(note, 90, 500); 
    //            keyState[key] = true;
    //        }
    //        else if (!isPressed && keyState[key])
    //        { 
    //            midiManager.StopNote(note);
    //            keyState[key] = false;
    //        }
    //    }
    //     
    //    if (GetAsyncKeyState('Q') & 0x8000) break;
    //}
    // 
    //for (auto& [key, note] : keyToNote)
    //{
    //    midiManager.StopNote(note);
    //}

    //return 0;
}
