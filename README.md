# GDQT - Grim Dawn Quests Tracker

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

1. Navigate to the release build directory where `GDQT.exe` is located, for example:
   ```bash
   cd ../GDQT/build/Desktop_Qt_6_8_0_MinGW_64_bit-Release/
   ```

2. Create a directory for the release package:
   ```bash
   mkdir release_package
   ```

3. Run the deployment tool to copy all necessary dependencies:
   ```bash
   C:\Qt\6.8.0\mingw_64\bin\windeployqt6.exe --dir .\release_package .\GDQT.exe
   ```
   Note: The path to `windeployqt6.exe` may vary depending on the version and installation path of Qt. Adjust the path as needed.

This will package all required dependencies into the `release_package` directory, making GDQT ready for distribution as a standalone application.

## Copyright Notice

**GDQT - Grim Dawn Quests Tracker** is a fan-made utility created for **educational and personal use only**. This project is in no way intended to infringe upon the intellectual property rights of the developers and publishers of *Grim Dawn*, **Crate Entertainment**. I fully respect their rights and acknowledge that all assets, such as quest files, save files, and other in-game content, are the sole property of Crate Entertainment. GDQT simply reads data from these files to provide players with a convenient way to track their quest progress in the game.

### Icon Usage

The application icon used in GDQT is sourced from *Grim Dawn* game assets and is the property of **Crate Entertainment**. This icon is used strictly within the context of this free, fan-made software, with no intent to derive any profit or financial gain.

### Font License

This project includes the **LinBiolinum_R** font, created by the **Linux Libertine Open Fonts Project**. According to the terms of the **Open Font License (OFL)**, this font is free to use, modify, and distribute for both commercial and non-commercial purposes, provided that attribution to the original creators is maintained. I do not claim ownership of this font, nor am I responsible for any modifications that might have been made to it by third parties.

### Qt Library License

GDQT is developed using the **Qt framework** under the free **GNU Lesser General Public License (LGPL)**. The application is built with the **free version of Qt**, in compliance with LGPL requirements. This license allows for the free use of Qt libraries in open-source projects. GDQT itself is distributed free of charge and is intended solely for non-commercial use by *Grim Dawn* players.

Users of this project are reminded that they may need to comply with Qt’s licensing terms if they modify or redistribute this project, especially for commercial purposes. For more information about Qt licensing, please visit the [Qt website](https://www.qt.io/licensing).

### Non-Commercial, Free Distribution

GDQT is distributed **entirely free of charge** and without any warranty. It is provided "as-is," without any implied or explicit guarantee of functionality, accuracy, or safety. This tool is meant to assist *Grim Dawn* players who may find its functionality useful for personal use, and it is not endorsed by or affiliated with Crate Entertainment. I do not charge any fees, accept donations, or otherwise profit from GDQT. The project is offered purely for **educational purposes** and **non-commercial use** by the community of *Grim Dawn* players.

By using GDQT, you acknowledge and agree that I am not liable for any damages or issues arising from the use of this software. Use of GDQT is entirely at the user's own risk, and all responsibilities for compliance with copyright laws and software licenses rest with the user.

---

This disclaimer ensures compliance with the intellectual property rights of Crate Entertainment, the Qt framework, and the Linux Libertine Open Fonts Project, and emphasizes the educational and non-commercial nature of the project. If you have any questions or concerns regarding GDQT’s copyright policy, please feel free to contact me.
