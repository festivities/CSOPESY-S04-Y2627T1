/*
    CSOPESY S04 - Y2627T1 - GROUP 6
    This file contains the logic for the Search widget.
*/

#include "App3.h"
#include <cctype>

// lowercase a copy of the string
static std::string toLower(std::string s) {
    for (int i = 0; i < (int)s.size(); i++) {
        s[i] = (char)tolower((unsigned char)s[i]);
    }
    return s;
}

App3::App3(GLuint icon) : Widget("Search", icon) {}

void App3::draw() {
    // dummy apps and files to search through
    static const char* items[] = {
        "Calculator", "Notepad", "Paint", "Task Manager", "Command Prompt",
        "Calendar", "Clock", "Photos", "Music Player", "Settings",
        "resume.docx", "budget.xlsx", "notes.txt", "photo.jpg", "setup.exe"
    };
    // text typed in the search box
    static char query[128] = "";

    // window size on first open
    ImGui::SetNextWindowSize(ImVec2(360, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin(name.c_str(), &isOpen);

    // search box
    ImGui::InputText("Query", query, IM_ARRAYSIZE(query));
    ImGui::Separator();

    // list items that contain the query, ignoring case
    std::string q = toLower(query);
    int shown = 0;
    for (int i = 0; i < (int)IM_ARRAYSIZE(items); i++) {
        if (toLower(items[i]).find(q) != std::string::npos) {
            ImGui::BulletText("%s", items[i]);
            shown++;
        }
    }

    // nothing matched
    if (shown == 0) {
        ImGui::Text("No results found.");
    }

    ImGui::End();
}
