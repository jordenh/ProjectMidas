#ifndef SHARED_COMMAND_DATA_TEST
#define SHARED_COMMAND_DATA_TEST

#include <iostream>
#include <string>
#include "WearableDevice.h"
#include "FilterPipeline.h"
#include "AdvancedFilterPipeline.h"
#include <vector>
#include <thread>
using namespace std;

class TestFilter : public Filter {
public:
    void process()
    {
        filterDataMap input = Filter::getInput();
        int cmd = boost::any_cast<int>(input["cmd"]);
        CommandData command;

        if (cmd == 0)
        {
            command.setType(KYBRD_CMD);
            command.setActionKybd(UNDO);
        }
        else if (cmd == 1)
        {
            command.setType(KYBRD_CMD);
            command.setActionKybd(COPY);
        }
        else
        {
            command.setType(MOUSE_CMD);
            command.setActionMouse(LEFT_CLICK);
        }

        filterDataMap output;
        output[COMMAND_INPUT] = command;
        Filter::setOutput(output);
    }
};

class TestWearableClass : public WearableDevice
{
public:
    TestWearableClass(SharedCommandData* sharedCommandData) : WearableDevice(sharedCommandData), done(false)
    {
        commandFilter = new TestFilter();
        commandList.push_back(0);
        commandList.push_back(1);
        commandList.push_back(2);
    }

    ~TestWearableClass()
    {
        delete commandFilter;
    }

    void runDeviceLoop()
    {
        AdvancedFilterPipeline commandPipeline, coordPipeline;
        vector<int>::iterator it;

        commandPipeline.registerFilterAtDeepestLevel(commandFilter);
        commandPipeline.registerFilterAtNewLevel(WearableDevice::sharedData);

        coordPipeline.registerFilterAtDeepestLevel(WearableDevice::sharedData);

        filterDataMap coordInput, commandInput;
        coordInput[VELOCITY_INPUT] = point(100, 50);

        coordPipeline.startPipeline(coordInput);
        for (it = commandList.begin(); it != commandList.end(); it++)
        {
            commandInput["cmd"] = *it;
            commandPipeline.startPipeline(commandInput);
            this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        coordInput[VELOCITY_INPUT] = point(10, 10);
        coordPipeline.startPipeline(coordInput);

        done = true;
    }

    int getDeviceError()
    {
        return 0;
    }

    bool isDone() { return done; }
private:
    vector<int> commandList;
    bool done;
    Filter* commandFilter;
};

class SharedCommandDataTest
{
public:
    static void testQueue(void);
};

#endif /* SHARED_COMMAND_DATA_TEST */