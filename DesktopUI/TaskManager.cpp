/*
	CSOPESY S04 - Y2627T1 - GROUP 6
	This file contains the logic for the task manager widget.
*/
#include "TaskManager.h"

// dummy process data
struct Process {
    const char* name;
    const char* status;
    float cpu;     // percent
    float memory;  // MB
};

static Process processes[] = {
    { "System",       "Running",    0.4f,   12.3f },
    { "csopesy.exe",  "Running",    3.2f,   45.1f },
    { "explorer.exe", "Running",    1.8f,  128.7f },
    { "Task Manager", "Running",    0.9f,   38.5f },
    { "chrome.exe",   "Running",   12.5f,  512.0f },
    { "Discord.exe",  "Suspended",  0.0f,   96.4f },
    { "svchost.exe",  "Running",    0.6f,   22.8f },
    { "Spotify.exe",  "Running",    2.1f,  210.6f },
    { "OneDrive.exe", "Suspended",  0.0f,   18.2f },
    { "Notepad.exe",  "Running",    0.1f,    6.7f },
};

// how many dummy processes we have
static int processCount = sizeof(processes) / sizeof(processes[0]);

TaskManager::TaskManager(GLuint icon) : Widget("Task Manager", icon) {}

void TaskManager::draw() {
    // default size the first time it opens
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(name.c_str(), &isOpen);

    // tabs like the real task manager
    if (ImGui::BeginTabBar("taskManagerTabs")) {

        // processes tab, the main one
        if (ImGui::BeginTabItem("Processes")) {

            // table with borders, row bg and a scroll bar
            if (ImGui::BeginTable("processTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0.0f, 200.0f))) {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Status");
                ImGui::TableSetupColumn("CPU");
                ImGui::TableSetupColumn("Memory");
                ImGui::TableHeadersRow();

                // one row per dummy process
                for (int i = 0; i < processCount; i++) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", processes[i].name);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", processes[i].status);
                    ImGui::TableNextColumn();
                    ImGui::Text("%.1f%%", processes[i].cpu);
                    ImGui::TableNextColumn();
                    ImGui::Text("%.1f MB", processes[i].memory);
                }
                ImGui::EndTable();
            }

            // add up cpu and memory for the summary line
            float totalCpu = 0.0f;
            float totalMem = 0.0f;
            for (int i = 0; i < processCount; i++) {
                totalCpu += processes[i].cpu;
                totalMem += processes[i].memory;
            }
            ImGui::Text("Total CPU: %.1f%%   Total Memory: %.1f MB", totalCpu, totalMem);

            // placeholder, does nothing yet
            ImGui::Button("End task");

            ImGui::EndTabItem();
        }

        // placeholder tabs
        if (ImGui::BeginTabItem("Performance")) {
            ImGui::Text("Performance graphs will go here.");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Details")) {
            ImGui::Text("Process details will go here.");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
