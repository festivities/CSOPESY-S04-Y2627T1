/*
    CSOPESY S04 - Y2627T1 - GROUP 6
    This file contains the logic for the File Explorer widget.
*/

#include "App2.h"

App2::App2(GLuint icon) : Widget("File Explorer", icon) {}

void App2::draw() {
    // dummy folders and files (name, type, size)
    static const char* folders[] = { "Desktop", "Documents", "Downloads", "Pictures" };
    static const char* files[4][3][3] = {
        { {"shortcut.lnk", "Shortcut", "1 KB"}, {"notes.txt", "Text File", "2 KB"}, {"screenshot.png", "PNG Image", "340 KB"} },
        { {"report.docx", "Word Document", "88 KB"}, {"budget.xlsx", "Excel File", "24 KB"}, {"resume.pdf", "PDF File", "156 KB"} },
        { {"setup.exe", "Application", "12.4 MB"}, {"movie.mp4", "MP4 Video", "1.2 GB"}, {"archive.zip", "ZIP Archive", "9.8 MB"} },
        { {"beach.jpg", "JPEG Image", "2.1 MB"}, {"wallpaper.png", "PNG Image", "5.6 MB"}, {"logo.svg", "SVG Image", "4 KB"} }
    };
    // index of the picked folder, starts on Documents
    static int selected = 1;

    // window size on first open
    ImGui::SetNextWindowSize(ImVec2(560, 360), ImGuiCond_FirstUseEver);
    ImGui::Begin(name.c_str(), &isOpen);

    // fake address bar, follows the selected folder
    std::string path = "C:\\Users\\CSOPESY\\" + std::string(folders[selected]);
    ImGui::Text("%s", path.c_str());
    ImGui::Separator();

    // left column with fake folders
    ImGui::BeginChild("folders", ImVec2(140, 0));
    for (int i = 0; i < (int)IM_ARRAYSIZE(folders); i++) {
        if (ImGui::Selectable(folders[i], selected == i)) {
            selected = i;
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // right side table with the files of the selected folder
    if (ImGui::BeginTable("files", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Size");
        ImGui::TableHeadersRow();

        // one row per dummy file
        for (int i = 0; i < 3; i++) {
            ImGui::TableNextRow();
            for (int j = 0; j < 3; j++) {
                ImGui::TableSetColumnIndex(j);
                ImGui::Text("%s", files[selected][i][j]);
            }
        }
        ImGui::EndTable();
    }

    ImGui::End();
}
