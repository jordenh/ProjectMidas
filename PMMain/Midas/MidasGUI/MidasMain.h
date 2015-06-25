#ifndef MIDAS_MAIN_H
#define MIDAS_MAIN_H

class MidasThread;
class MainGUI;
class ProfileManager;
class ringData;
#include <vector>

#ifdef BUILD_KEYBOARD
int midasMain(MidasThread *threadHandle, MainGUI *mainGui, ProfileManager *pm, std::vector<ringData> *kybrdRingData);
#else
int midasMain(MidasThread *threadHandle, MainGUI *mainGui, ProfileManager *pm);
#endif

#endif /* MIDAS_MAIN_H */