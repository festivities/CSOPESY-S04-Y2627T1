/*
    CSOPESY S04 - Y2627T1 - GROUP 6
    This file contains the logic for the App 3 widget.
*/

#include "App3.h"

App3::App3(GLuint icon) : Widget("App 3", icon) {}

void App3::draw() {
    // TODO: implement widget here
    ImGui::Begin(name.c_str(), &isOpen);
    ImGui::Text("TODO!");
    ImGui::End();
}