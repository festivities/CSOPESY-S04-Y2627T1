### Y2627T1 CSOPESY S04

# OS Emulator
This project is a simple OS emulator that simulates basic operating system functionalities. It provides GUI for users to interact with the emulator.

### Group 6 Members:
- Ong, David Zachary
- Ho, Denise Liana
- Valle, Jose Edgardo
- Biacora, Luis Gabriel

### Entry file: 
- main.cpp (contains main())


## Setup and Build Instructions:

This project uses vcpkg in Manifest Mode (`vcpkg.json`) to manage dependencies (GLFW, GLAD, Dear ImGui). You must have vcpkg installed and integrated for Visual Studio to compile the project.

1. If you do not have `vcpkg`, clone it and run the bootstrapper:
   ```
	git clone https://github.com/microsoft/vcpkg.git
   .\vcpkg\bootstrap-vcpkg.bat
   ```

2. Open a terminal and run the integration command so Visual Studio detects the package manifest:
   vcpkg integrate install

3. Open `DesktopUI.sln` file in Visual Studio 2022.

4. Ensure the build config is set to "x64"

5. Build the solution (Ctrl + Shift + B)
   *Note: the first vuild will take a few mins since vcpkg automatically downloads and compiles dependencies from source.*

6. Run the Local Windows Debugger.