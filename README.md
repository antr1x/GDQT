# GDQT - Grim Dawn Quest Tracker

**GDQT** is a quest-tracking tool for **Grim Dawn**.

## Requirements

- **Qt**: The software is built with Qt and requires the Qt SDK to compile and run.
- **Resources**: The project includes a `resources` folder with necessary pre-generated quests JSON data for the tracker to function correctly. This folder should be placed alongside the compiled version of GDQT.

## Resource Information

The `resources` folder is already included in the Git repository for convenience. Place it in the same directory as the compiled GDQT executable. Alternatively, if the `resources` folder or `quests.json` file is missing, you can generate it directly within the application. Instructions on how to generate this data are provided in the GDQT software under **"How to Extract and Generate Quests Data"**.

## Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/antr1x/GDQT.git
   ```

2. Open **Qt Creator**.

3. Go to `File` > `Open File or Project...` and select the `CMakeLists.txt` file in the cloned `GDQT` directory.

4. When prompted, configure the project by selecting only the **Release** and **Debug** kits.

5. After the kits are selected, the project will be ready for compilation.

## Building a Standalone Version

To build a standalone version of GDQT, use the **windeployqt** tool that comes with the Qt Creator. Follow these steps:

1. Create a directory for the release package:
   ```bash
   mkdir release_package
   ```

2. Run the deployment tool to copy all necessary dependencies:
   ```bash
   C:\Qt\6.8.0\mingw_64\bin\windeployqt6.exe --dir .\release_package .\GDQT.exe
   ```
   Note: The path to `windeployqt6.exe` may vary depending on the version and installation path of Qt. Adjust the path as needed.

This will package all required dependencies into the `release_package` directory, making GDQT ready for distribution as a standalone application.