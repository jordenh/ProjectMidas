# ProjectMidas

A wearable interface for the Windows OS, replacing the functionality of a mouse (and optionally keyboard).

## Quick start

Two quick start options are available:

- [Download the latest release](https://github.com/jordenh/ProjectMidas/MidasInstall.zip).
- Clone the repo: `git clone https://github.com/jordenh/ProjectMidas`.

### What's included

A version of Midas primarily intended to replace the Mouse (MidasGUI.exe). Also includes a profile manager to quickly edit profiles (MidasProfileManager.exe), which should be used to edit profiles.xml if changes are desired.

### Developer Details

Currently setup to work with Myo Armband:
- SDK Win 0.9.0
- Firmware 1.4.1670
- Myo Connect V 0.14.0

#### To build MidasGU:

- Need Visual Studio 2013
- Download and install Qt 5.4.0 2013 x86 with opengl (qt-opensource-windows-x86-msvc2013_opengl-5.4.0.exe from http://download.qt.io/archive/qt/5.4/5.4.0/)
- Put the bin folder of the Qt SDK onto your PATH environment variable
- Download and install the most recent Qt Visual Studio Plugin (http://www.qt.io/download-open-source/#section-2)
- Download Boost 1.57.0 (http://www.boost.org/users/history/version_1_57_0.html)
- Unzip Boost so that your file system has the following structure: C:\Program Files\boost\boost_1_57_0* , where '*' holds the boost, docs, lib, etc folders
- You may need to copy Hub_console.lib from the Midas/MyoSim directory to MidasGUI/MyoSim, or remove the dependancy from the project file to Hub_console.lib, as it is only required if you use the MyoSim build settings.
- Build and enjoy :)
