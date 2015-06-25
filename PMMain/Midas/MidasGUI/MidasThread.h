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
#endif
	MidasThread();

    void setMainGuiHandle(MainGUI *mainGui);
    void setProfileManagerHandle(ProfileManager *profileManager);
    void run();

private:
    MainGUI *mainGui;
    ProfileManager *profileManager;

#ifdef BUILD_KEYBOARD
	std::vector<ringData> *kybrdRingData;
signals:
	void emitUpdateKeyboard(int, double, bool, bool);  // kybdGUISel, angle, center, held
	void emitRssi(float);
#endif

signals:
    void emitVeloc(int, int);

    void emitDebugInfo(int, int);

    void emitDisconnect(bool);
};

#endif // MIDASTHREAD_H
