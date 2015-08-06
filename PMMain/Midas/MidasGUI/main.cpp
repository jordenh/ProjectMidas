/*
    Copyright (C) 2015 Midas

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

#include "MidasThread.h"
#include "MouseCtrl.h"
#include "MainGUI.h"

#include <QtWidgets/QApplication>
#include <qicon.h>
#include <qlabel.h>

#include "ProfileManager.h"
#include "KeyboardWidget.h"
#include "KeyboardSettingsReader.h"

#define DEFAULT_PROFILES "profiles.xml"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	std::vector<ringData> kybrdRingData;
	KeyboardSettingsReader readFile;
    std::string fileName = "keyboardData.txt";
    readFile.readKeyboardSetupFile(kybrdRingData,fileName);
	
	MidasThread midasThread(&kybrdRingData);

    qRegisterMetaType<std::vector<sequenceImageSet> >("std::vector<sequenceImageSet>");
    qRegisterMetaType<std::vector<sequenceProgressData> >("std::vector<sequenceProgressData>");

    ProfileManager pm;
    pm.loadProfilesFromFile(DEFAULT_PROFILES);

    MainGUI mainDisplay(&midasThread, &pm, MOVE_RATE_DEADZONE);
    midasThread.setProfileManagerHandle(&pm);
    midasThread.setMainGuiHandle(&mainDisplay);
    midasThread.start();

    // TODO: This is a hack. It fixes the issue with the widgets not staying on top, but
    // we still have to investigate why this works. Could be a bug with how we are creating
    // our widgets, or it could be an issue in Qt (or we do not fully understand how Qt handles 
    // showing widgets and windows that are always on top).
    QLabel dummyLabel;

    dummyLabel.show();
    dummyLabel.setVisible(false);
    // End hack

    QIcon *icon = new QIcon(TASK_BAR_ICON_PATH);
    mainDisplay.setWindowIcon(*icon);
    mainDisplay.show();
    return a.exec();
    system("PAUSE");
}
