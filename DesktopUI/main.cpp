/* 
	CSOPESY S04 - Y2627T1 - GROUP 6
	This file contains the 'main' function.
*/

#include "Widget.h"
#include "UIManager.h"
#include "TaskManager.h"
#include "App2.h"
#include "App3.h"

// loading image 
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// GUI dependencies
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

// This helper function loads an image into a GPU texture
bool loadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
	int w = 0;
	int h = 0;

	// load from file
	unsigned char* image_data = stbi_load(filename, &w, &h, NULL, 4);
	if (image_data == NULL) return false;

	// create OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// upload pixels into texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	// free image memory
	stbi_image_free(image_data);

	// set output values
	*out_texture = image_texture;
	*out_width = w;
	*out_height = h;
	return true;
}


/**
* This function initializes GLFW and OpenGL context and creates a window for the 
* desktop emulator. Returns ptr to GLFWwindow (the main window) if successful, 
* and nullptr otherwise.
*/
GLFWwindow* initWindow() {
	// init GLFW library, if fail then end
	if (!glfwInit()) return nullptr;

	// set openGL ver 3.3 and core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

	// create the main window
	GLFWwindow* w = glfwCreateWindow(
		(1280 * main_scale), (720 * main_scale), // wxh window size
		"CSOPESY Desktop OS Emulator",  // window title
		glfwGetPrimaryMonitor(), // glfwGetPrimaryMonitor() for fullscreen, NULL for windowed
		NULL  // share context with other window?
	);

	if (w == nullptr) {
		glfwTerminate();
		return nullptr;
	}

	// setup openGL context and vsync
	glfwMakeContextCurrent(w);
	glfwSwapInterval(1); // Enable vsync

	// init glad library, if fail then end
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwDestroyWindow(w);
		glfwTerminate();
		return nullptr;
	}
	
	return w;
}

/**
* This function renders the splash screen / boot screen.
*/
void renderBootScreen() {

	ImGuiIO& io = ImGui::GetIO();

	// cover entire window w/ boot screen
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(io.DisplaySize);

	// boot screen flags: borderless, immovable, and locked to the background
	ImGui::Begin("Boot_Screen", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	// TODO: add loading animation / progress bar here
	// TODO: also add proj / group details idk
	ImGui::Text("CSOPESY Desktop OS Emulator");
	ImGui::Text("Booting up...");

	ImGui::End();
}
/**
* This function renders the desktop UI.
*/
void renderDesktop(GLFWwindow* w, GLuint bgTexture) {

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(io.DisplaySize);

	// desktop flags: borderless, immovable, and locked to the background
	ImGuiWindowFlags desktopFlags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::Begin("CSOPESY_Desktop_Group6", nullptr, desktopFlags);

	// draw bg image
	if (bgTexture != 0) {
		ImGui::GetWindowDrawList()->AddImage(
			(void*)(intptr_t)bgTexture,
			ImVec2(0, 0),
			io.DisplaySize
		);
	}

	// taskbar size and position 
	float barHeight = 60.0f;
	float screenW = io.DisplaySize.x;
	float screenH = io.DisplaySize.y;
	float barY = screenH - barHeight; // position taskbar at bottom of screen

	ImVec2 barMin = ImVec2(0.0f, barY);
	ImVec2 barMax = ImVec2(screenW, screenH);
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddRectFilled(barMin, barMax, IM_COL32(30, 80, 190, 255)); // color of taskbar

	// WIDGETS

	// power button
	ImVec2 btnSize = ImVec2(90.0f, 32.0f);
	float btnX = 8.0f;
	float btnY = barY + (barHeight - btnSize.y) * 0.5f;

	// set cursor position for button
	ImGui::SetCursorPos(ImVec2(btnX, btnY));
	// also set button colors for normal, hovered, and active states
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.15f, 0.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.55f, 0.05f, 0.05f, 1.0f));
	
	// if power button is clicked, close the window
	if (ImGui::Button("PWR", btnSize)) {
		glfwSetWindowShouldClose(w, GLFW_TRUE);
	}
	ImGui::PopStyleColor(3); // pop the 3 colors pushed for the button

	// widget buttons
	ImVec2 iconSize = ImVec2(24.0f, 24.0f);

	// create button for each widget in UIManager list
	for (auto& app : UIManager::getInstance().widgets) {
		ImGui::SameLine();

		// set cursor position for the button
		if (ImGui::ImageButton(app->getName().c_str(), (void*)(intptr_t)app->getIcon(), iconSize)) {
			app->toggleOpen();
		}

		// add tooltip hover
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", app->getName().c_str());
		}
	}

	// CLOCK AND DATE DISPLAY

	// get current time and date
	auto now = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

	// convert local time to tm struct for formatting
	struct tm timeInfo;
	localtime_s(&timeInfo, &currentTime);

	// split time and date into separate strings
	std::ostringstream timeStream, dateStream;
	timeStream << std::put_time(&timeInfo, "%I:%M:%S %p");
	dateStream << std::put_time(&timeInfo, "%m/%d/%Y");

	std::string timeStr = timeStream.str();
	std::string dateStr = dateStream.str();

	// align time and date to the right side of the taskbar
	ImVec2 timeSize = ImGui::CalcTextSize(timeStr.c_str());
	ImVec2 dateSize = ImGui::CalcTextSize(dateStr.c_str());

	// padding from the right edge of the taskbar
	float paddingX = 15.0f;
	float timeX = screenW - timeSize.x - paddingX;
	float dateX = screenW - dateSize.x - paddingX;

	// center vertically within the taskbar
	float totalHeight = timeSize.y + dateSize.y;
	float startY = barY + (barHeight - totalHeight) * 0.5f;

	// draw time
	ImGui::SetCursorPos(ImVec2(timeX, startY));
	ImGui::Text("%s", timeStr.c_str());

	// draw date
	ImGui::SetCursorPos(ImVec2(dateX, startY + timeSize.y));
	ImGui::Text("%s", dateStr.c_str());

	ImGui::End();

	// automatically draw widget when opened
	for (auto& app : UIManager::getInstance().widgets) {
		if (app->getIsOpen()) {
			app->draw();
		}
	}
}

/**
* This function shuts down the GLFW window and the Dear ImGui context.
* @param w The GLFWwindow to shut down.
*/
void shutdown(GLFWwindow* w) {
	// shutdown contexts
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// shutdown glfw window
	glfwDestroyWindow(w);
	glfwTerminate();
}

/**
* The main function of the application.
* @return 0 if successful, -1 otherwise.
*/
int main() {

	/* BOOTSTRAPPING */

	// create main "desktop" window
	GLFWwindow* window = initWindow();

	if (window == nullptr) {
		return -1;
	}

	// setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	float bootDuration = 10.0f; // 10 seconds boot (CHANGE IF WANT)
	float bootStartTime = glfwGetTime();
	bool isBooting = true;

	// LOAD TEXTURES
	
	// wallpaper
	GLuint wallpaper = 0;
	int imgW = 0, imgH = 0;
	loadTextureFromFile("windows_xp.jpg", &wallpaper, &imgW, &imgH);

	// app icons
	// TODO: use diff pngs per app
	GLuint icon1 = 0, icon2 = 0, icon3 = 0;
	int tw = 0, th = 0;
	loadTextureFromFile("sample_icon.png", &icon1, &tw, &th);
	loadTextureFromFile("sample_icon.png", &icon2, &tw, &th);
	loadTextureFromFile("sample_icon.png", &icon3, &tw, &th);	

	// add widgets to UIManager
	UIManager::getInstance().addWindow(std::make_shared<TaskManager>(icon1));
	UIManager::getInstance().addWindow(std::make_shared<App2>(icon2));
	UIManager::getInstance().addWindow(std::make_shared<App3>(icon3));

	/* MAIN LOOP */
	while (!glfwWindowShouldClose(window)) {
		// poll events (send events to callbacks/window)
		glfwPollEvents();

		// start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// switch between boot screen and desktop
		if (isBooting) {
			renderBootScreen();
			if (glfwGetTime() - bootStartTime > bootDuration) { 
				isBooting = false;
			}
		}
		else {
			renderDesktop(window, wallpaper);
		}

		//ImGui::ShowDemoWindow(); // Show demo window! :)

		// render frame to screen
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		// black screen for booting, blue screen for desktop
		if (isBooting) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		}
		else {
			glClearColor(0.15f, 0.45f, 0.65f, 1.0f);
		}
		glClear(GL_COLOR_BUFFER_BIT); // clear the screen

		// draw the ImGui elements and swap the buffers
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	/* SHUTDOWN */
	shutdown(window);

	return 0;
}