#include "MidasThread.h"
#include "MouseCtrl.h"
#include "MainGUI.h"

#include <QtWidgets/QApplication>
#include <qicon.h>
#include <qlabel.h>

#include "ProfileManager.h"

#define DEFAULT_PROFILES "profiles.xml"

#ifdef BUILD_KEYBOARD
#include "KeyboardWidget.h"
#include "KeyboardSettingsReader.h"
#endif

#define TASK_BAR_ICON_PATH "Resources\\ProjectMidasLogo.ico"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef BUILD_KEYBOARD
	std::vector<ringData> kybrdRingData;
	KeyboardSettingsReader readFile;
    std::string fileName = "keyboardData.txt";
    readFile.readKeyboardSetupFile(kybrdRingData,fileName);
	
	MidasThread midasThread(&kybrdRingData);
#else
    MidasThread midasThread;
#endif

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
