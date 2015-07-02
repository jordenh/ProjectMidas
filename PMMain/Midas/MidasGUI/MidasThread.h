#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>

#ifdef BUILD_KEYBOARD
#include <vector>
class ringData;
class KeyboardWidget;
#endif

class MainGUI;
class ProfileManager;

class MidasThread : public QThread
{
    Q_OBJECT

public:
    ~MidasThread();
#ifdef BUILD_KEYBOARD
	MidasThread(std::vector<ringData> *kybrdRingData);
	std::vector<ringData>* getKybrdRingData();
#else
	MidasThread();
#endif

    void setMainGuiHandle(MainGUI *mainGui);
    void setProfileManagerHandle(ProfileManager *profileManager);
    void run();

private:
    MainGUI *mainGui; // not owned
    ProfileManager *profileManager; // not owned

#ifdef BUILD_KEYBOARD
	std::vector<ringData> *kybrdRingData; // not owned
#endif
signals:
	void emitUpdateKeyboard(int, double, bool, bool);  // kybdGUISel, angle, center, held
	void emitRssi(float);

signals:
    void emitVeloc(int, int);

    void emitDebugInfo(int, int);

    void emitDisconnect(bool);
};

#endif // MIDASTHREAD_H
