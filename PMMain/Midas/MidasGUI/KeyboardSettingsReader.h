#ifdef BUILD_KEYBOARD
#pragma once
#include <vector>

class ringData;

class KeyboardSettingsReader
{
    enum currentRing
    {
        ringIn,
        ringOut
    };

public:
    KeyboardSettingsReader();
    ~KeyboardSettingsReader();
    void readKeyboardSetupFile(std::vector<ringData> &ringDataHandle, std::string fileName);
};


#endif