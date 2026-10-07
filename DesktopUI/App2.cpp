/*
    CSOPESY S04 - Y2627T1 - GROUP 6
    This file contains the logic for the App 2 widget.
*/

#include "App2.h"

App2::App2(GLuint icon) : Widget("App 2", icon) {}

void App2::draw() {
    // TODO: implement widget here
    ImGui::Begin(name.c_str(), &isOpen);
    ImGui::Text("TODO!");
    ImGui::End();
}