Note, Include folder contains all Qt Source code as per Qt LGPL requirements, seen concisely at: http://www.qt.io/faq/ . Dlls are not included on repo, but can be obtained at Qt's site.

If this folder does not contain any dlls, or lib/plugins folders, you must get the following files and populate the folder for the installer project to work correctly. Note all files are hosted by Qt at http://www.qt.io/ and are required to be from version 5.4.0 32 Bit:

	icudt53.dll
	icuin53.dll
	icuuc53.dll
	Qt5Core.dll
	Qt5Cored.dll
	Qt5Gui.dll
	Qt5Guid.dll
	Qt5Widgets.dll
	Qt5Widgetsd.dll
	
	lib\*
	plugins\platforms\*
	
where * indicates the entire folder from Qt 5.4.0 was copied.