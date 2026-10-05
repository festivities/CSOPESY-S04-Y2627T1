/* 
	CSOPESY S04 - Y2627T1 - GROUP 6
	This file contains the 'main' function.
*/

// GUI dependencies
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

/**
* This function initializes GLFW and OpenGL context and creates a window for the 
* desktop emulator. Returns ptr to GLFWwindow if successful, nullptr otherwise.
*/
GLFWwindow* initWindow() {
	// init GLFW library, if fail then end
	if (!glfwInit()) return nullptr;

	// set openGL ver 3.3 and core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create main window (fullscreen)
	GLFWwindow* window = glfwCreateWindow(
		1280, 720,						// wxh window size
		"CSOPESY Desktop OS Emulator",  // window title
		glfwGetPrimaryMonitor(),	    // fullscreen. NULL for windowed
		NULL							// share context with other window?
	);

	if (window == nullptr) {
		glfwTerminate();
		return nullptr;
	}

	// setup openGL context and vsync
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// init glad library, if fail then end
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return nullptr;
	}
	
	return window;
}

/**
* This function shuts down the GLFW window and the Dear ImGui context.
* @param w The GLFWwindow to shut down.
*/
void shutdown(GLFWwindow* w) {
	// shutdown imgui context
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

	/* MAIN LOOP */
	while (!glfwWindowShouldClose(window)) {
		// poll events (send events to callbacks/window)
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow(); // Show demo window! :)

		// render frame to screen
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		// Clear the background (Dark Gray)
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the ImGui elements and swap the buffers
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	/* SHUTDOWN */
	shutdown(window);

	return 0;
}