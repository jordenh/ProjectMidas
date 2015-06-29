#include "EMGImpulseFilter.h"

#include "MyoState.h"

#include "SharedCommandData.h"

EMGImpulseFilter::EMGImpulseFilter(MyoState* myoState) : myoStateHandle(myoState) {}
EMGImpulseFilter::~EMGImpulseFilter() {}

void EMGImpulseFilter::process()
{
    filterDataMap input = Filter::getInput();

    std::array<int8_t, 8> emgSamples = boost::any_cast<std::array<int8_t, 8>>(input[EMG_VECTOR]);

    emgSamplesDeque.push_back(emgSamples);
    std::array<int8_t, 8> absEmgSamples;
    for (int i = 0; i < 8; i++)
    {
        absEmgSamples[i] = abs(emgSamples[i]);
    }
    absEmgSamplesDeque.push_back(absEmgSamples);

    if (emgSamplesDeque.size() > FILTER_LEN)
    {
        emgSamplesDeque.pop_front();
    }

    updateImpulseStatus();

    filterDataMap output;

    output[IMPULSE_STATUS] = impulseStatus;

    Filter::setOutput(output);
}

void EMGImpulseFilter::updateImpulseStatus()
{
    // Step 1 - Take average of absolute value of EMG data as we 
    // want to determine if muscle activity is present (dont care about sign)
    // and want to counteract discrepencies
    std::array<float, 8> absAvg = calcAvg(absEmgSamplesDeque);

    // Step 2 - take the max acrrocess the average and raise it to EMG_POWER power
    maxAvg = 0;
    for (std::array<float, 8>::iterator it = absAvg.begin(); it != absAvg.end(); it++)
    {
        maxAvg = std::max(maxAvg, (*it));
    }
    maxAvg = std::pow(maxAvg, EMG_POWER);
    
    // Step 3 - perform logic with various counters to determine if the 
    // system is viewing an impulse
    impulseStatus = performImpulseLogic();
}

bool EMGImpulseFilter::performImpulseLogic()
{
    bool retVal = false;
    bool currentlyPosing = (myoStateHandle->getPoseHistory().size() > 0) ? (myoStateHandle->getPoseHistory().back().type() != myo::Pose::rest) : false;

    if (currMuscleActive == false && maxAvg > MAX_EMG_IMPULSE_THRESHOLD_HIGH && currentlyPosing == false)
    {
        // impulse detected in muscle activity when Myo not sending a pose signal yet.
        // Deem this an impulse
        currMuscleActive = true;
        impulseCount = IMPULSE_SIZE;
    }

    if (currMuscleActive && impulseCount > 0)
    {
        retVal = true;
        impulseCount--;
    }
    else
    {
        retVal = false;
    }

    if (impulseCount <= 0 && maxAvg <= MAX_EMG_IMPULSE_THRESHOLD_LOW)
    {
        // reset the currMuscleActive flag when the pose has been let go after IMPULSE_SIZE steps
        // so that it is searching for the next impulse in the system.
        currMuscleActive = false;
    }

    return retVal;
}

std::array<float, 8> EMGImpulseFilter::calcAvg(std::deque<std::array<int8_t, 8>>& dq)
{
    std::array<float, 8> avgs = { 0 };
    std::array<float, 8> sum = { 0 };
    float denom = dq.size();
    if (denom == 0)
    {
        return avgs;
    }

    std::deque<std::array<int8_t, 8>>::iterator it = dq.begin();
    while (it != dq.end())
    {
        std::array<int8_t, 8> currArr = *it;
        for (int i = 0; i < 8; i++)
        {
            sum[i] += currArr[i];
        }

        it++;
    }

    for (int i = 0; i < 8; i++)
    {
        avgs[i] = sum[i] / denom;
    }

    return avgs;
}