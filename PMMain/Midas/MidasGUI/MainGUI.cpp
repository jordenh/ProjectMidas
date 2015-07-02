#define NOMINMAX

#include "MainGUI.h"


#include <QApplication.h>
#include <QDesktopWidget.h>
#include <algorithm>
#include <QEvent.h>
#include <QDialog>
#include <qlayout.h>
#include "MouseIndicator.h"
#include "SequenceDisplayer.h"
#include "InfoIndicator.h"
#include "GestureSignaller.h"
#include "PoseDisplayer.h"
#include "ProfileDisplayer.h"
#include "ProfileIcon.h"
#include "ProfileDisplayer.h"
#include "ProfileSignaller.h"
#include "ProfileManager.h"
#include "SettingsDisplayer.h"
#include "SettingsSignaller.h"
#include "MidasThread.h"


#ifdef BUILD_KEYBOARD
#include "KeyboardWidget.h"
#include "DistanceWidget.h"
#endif

#define SCREEN_RIGHT_BUFFER    20 
#define SCREEN_BOTTOM_BUFFER   30

MainGUI::MainGUI(MidasThread *mainThread, ProfileManager *pm, int deadZoneRad)
    : DraggableWidget(NULL, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint)
{
	infoIndicator = new InfoIndicator(INFO_INDICATOR_WIDTH, INFO_INDICATOR_HEIGHT, this);
    sequenceDisplayer = new SequenceDisplayer(this);
	poseDisplayer = new PoseDisplayer(MOUSE_INDICATOR_SIZE, MOUSE_INDICATOR_SIZE, this);
#ifdef BUILD_KEYBOARD
//	distanceDisplayer = new DistanceWidget(mainThread, INFO_INDICATOR_WIDTH,
//		DISTANCE_DISPLAY_HEIGHT, this);
#endif

    numProfiles = pm->getProfiles()->size();
	setupProfileIcons();

    // Ensure Midas stays on top even when other applications have popups, etc
    this->setFocus();
    //setWindowFlags(windowFlags() | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);

	// create main layout and add sequences (they are at the top and constantly go in/out of view)
    layout = new QVBoxLayout;
	layout->addWidget(sequenceDisplayer);

#ifdef SHOW_PROFILE_BUTTONS
    std::vector<profile>* profiles = pm->getProfiles();
    std::vector<profile>::iterator it;
    int profileHeights = 0;
    for (it = profiles->begin(); it != profiles->end(); it++)
    {
        ProfileDisplayer* displayer = new ProfileDisplayer(it->profileName, PROF_INDICATOR_WIDTH, PROF_INDICATOR_HEIGHT, this);
        profileHeights += displayer->height();
        profileWidgets.push_back(displayer);
        layout->addWidget(displayer, 0, Qt::AlignRight);
    }
#endif

    settingsDisplayer = new SettingsDisplayer(PROF_INDICATOR_WIDTH, 2*INFO_INDICATOR_HEIGHT, this);
    layout->addWidget(settingsDisplayer, 0, Qt::AlignRight);

	// create HBox for specific profile icons: Change this icon to be specific to your app
	QHBoxLayout *profileIconLayout = new QHBoxLayout;
	profileIconLayout->addWidget(icon0);
	profileIconLayout->addWidget(icon1);

	QVBoxLayout *leftBoxLayout = new QVBoxLayout;
	leftBoxLayout->addWidget(infoIndicator);
	leftBoxLayout->addItem(profileIconLayout);

	QHBoxLayout *mainBoxLayout = new QHBoxLayout;
	mainBoxLayout->addItem(leftBoxLayout);
	mainBoxLayout->addWidget(poseDisplayer);
	mainBoxLayout->setAlignment(Qt::AlignRight);

	layout->addItem(mainBoxLayout);
     
    setLayout(layout);

#ifdef BUILD_KEYBOARD
	keyboard = new KeyboardWidget(mainThread);
	keyboard->addWheels(mainThread->getKybrdRingData());
#endif

    totalWidth = std::max(sequenceDisplayer->width(), 
                        (infoIndicator->width() + poseDisplayer->width()));
    totalHeight = sequenceDisplayer->height() + poseDisplayer->height() +
#ifdef SHOW_PROFILE_BUTTONS
        profileHeights + 
#endif
        settingsDisplayer->height(); 

    QRect screen = QApplication::desktop()->availableGeometry(this);
    setGeometry(screen.right() - totalWidth - SCREEN_RIGHT_BUFFER,
        screen.bottom() - totalHeight - SCREEN_BOTTOM_BUFFER,
        totalWidth, totalHeight);
}

#ifdef BUILD_KEYBOARD
void MainGUI::toggleKeyboard()
{
	if (keyboard->isVisible())
	{
		keyboard->setVisible(false);
	}
	else
	{
		keyboard->setVisible(true);
	}
}
#endif

MainGUI::~MainGUI()
{
    delete mouseIndicator;
    mouseIndicator = NULL;
    delete infoIndicator;
    infoIndicator = NULL;
    delete sequenceDisplayer;
    sequenceDisplayer = NULL;
    delete poseDisplayer;
    poseDisplayer = NULL;
    delete settingsDisplayer;
    settingsDisplayer = NULL;
    delete layout;
    layout = NULL;
	delete icon0;
	icon0 = NULL;
	delete icon1;
	icon1 = NULL;

#ifdef SHOW_PROFILE_BUTTONS
    for (int i = 0; i < profileWidgets.size(); i++)
    {
        delete profileWidgets.at(i); profileWidgets.at(i) = NULL;
    }
    profileWidgets.clear();
#endif
}

#ifdef BUILD_KEYBOARD
void MainGUI::connectSignallerToKeyboardToggle(GestureSignaller *signaller)
{
	QObject::connect(signaller, SIGNAL(emitToggleKeyboard()),
			this, SLOT(toggleKeyboard()));
}
#endif

void MainGUI::connectSignallerToSettingsDisplayer(SettingsSignaller *signaller)
{
    QObject::connect(settingsDisplayer, SIGNAL(emitSliderValues(unsigned int, unsigned int)),
        signaller, SLOT(handleSliderValues(unsigned int, unsigned int)));
    QObject::connect(settingsDisplayer, SIGNAL(emitBuzzFeedbackChange(unsigned int)),
        signaller, SLOT(handleBuzzFeedbackChange(unsigned int)));
}

void MainGUI::connectSignallerToProfileWidgets(ProfileSignaller* signaller)
{
	QMetaObject::Connection conn;
    for (int i = 0; i < profileWidgets.size(); i++)
    {
		conn = QObject::connect(profileWidgets[i], SIGNAL(emitChangeProfile(QString)), signaller, SLOT(handleProfilePress(QString)));
    }
}

void MainGUI::connectSignallerToInfoIndicator(GestureSignaller *signaller)
{
    QObject::connect(signaller, SIGNAL(emitStateString(QString)),
        infoIndicator, SLOT(handleUpdateState(QString)));
    QObject::connect(infoIndicator, SIGNAL(emitShowAllToggle(bool)),
        signaller, SLOT(handleShowAllToggle(bool)));
}

void MainGUI::connectSignallerToSequenceDisplayer(GestureSignaller *signaller)
{
    QObject::connect(signaller, SIGNAL(emitRegisterSequence(int, QString, std::vector<sequenceImageSet>)),
        sequenceDisplayer, SLOT(registerSequenceImages(int, QString, std::vector<sequenceImageSet>)));

    QObject::connect(signaller, SIGNAL(emitShowSequences(std::vector<sequenceProgressData>)),
        sequenceDisplayer, SLOT(showSequences(std::vector<sequenceProgressData>)));
}

void MainGUI::connectSignallerToPoseDisplayer(GestureSignaller *signaller)
{
    QObject::connect(signaller, SIGNAL(emitPoseImages(std::vector<sequenceImageSet>)),
        poseDisplayer, SLOT(handlePoseImages(std::vector<sequenceImageSet>)));
}

void MainGUI::connectSignallerToProfileIcons(GestureSignaller *signaller)
{
    QObject::connect(signaller, SIGNAL(emitProfileChange(bool)),
        this, SLOT(handleChangeProfile(bool)));
}

void MainGUI::setupProfileIcons()
{
	QImage icon0Active(QString(PROFILE_ICON0_ACTIVE));
	QImage icon0Inactive(QString(PROFILE_ICON0_INACTIVE));
	QImage icon1Active(QString(PROFILE_ICON1_ACTIVE));
	QImage icon1Inactive(QString(PROFILE_ICON1_INACTIVE));

	icon0IsActive = true;
    activeProfile = 0;
	icon0 = new ProfileIcon(SPECIFIC_PROFILE_ICON_SIZE, SPECIFIC_PROFILE_ICON_SIZE, true, QPixmap::fromImage(icon0Active), QPixmap::fromImage(icon0Inactive), this);
	icon1 = new ProfileIcon(SPECIFIC_PROFILE_ICON_SIZE, SPECIFIC_PROFILE_ICON_SIZE, false, QPixmap::fromImage(icon1Active), QPixmap::fromImage(icon1Inactive), this);
}

void MainGUI::handleChangeProfile(bool progressForward)
{
    if (progressForward)
    {
        activeProfile++;
        activeProfile %= numProfiles;
    }
    else if (!progressForward && activeProfile > 0)
    {
        activeProfile--;
    }
    else if (!progressForward && activeProfile <= 0)
    {
        activeProfile = numProfiles - 1;
    }

    if (activeProfile == 0)
    {
        icon0->setImgActiveSel(true);
        icon1->setImgActiveSel(false);
    }
    else if (activeProfile == 1)
    {
        icon0->setImgActiveSel(false);
        icon1->setImgActiveSel(true);
    }
    else if (activeProfile > 1)
    {
        icon0->setImgActiveSel(false);
        icon1->setImgActiveSel(false);
    }
}

void MainGUI::handleFocusMidas()
{

}