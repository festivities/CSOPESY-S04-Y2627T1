/*
	CSOPESY S04 - Y2627T1 - GROUP 6
	This file contains the logic for the task manager widget.
*/
#include "TaskManager.h"

TaskManager::TaskManager    (GLuint icon) : Widget("Task Manager", icon) {}

void TaskManager::draw() {
	// TODO: implement task manager here
    ImGui::Begin(name.c_str(), &isOpen);
    ImGui::Text("TODO!");
    ImGui::End();
}