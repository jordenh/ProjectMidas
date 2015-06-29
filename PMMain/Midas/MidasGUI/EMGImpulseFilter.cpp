#include "EMGImpulseFilter.h"

#include "MyoState.h"

#include "SharedCommandData.h"

bool EMGImpulseFilter::impulseStatus = false;

EMGImpulseFilter::EMGImpulseFilter(MyoState* myoState) : myoStateHandle(myoState) {
    maxAvg = 0;
    impulseStatus = false;
    risingLogicImpulseCount = 0;
    risingLogicMuscleActive = false;
    fallingLogicFallDetection = FALLING_DETECTION_COUNT;
    fallingLogicRiseDetection = FALLING_DETECTION_COUNT;
}
EMGImpulseFilter::~EMGImpulseFilter() {}

void EMGImpulseFilter::process()
{
    filterDataMap input = Filter::getInput();

    std::array<int8_t, 8> emgSamples = boost::any_cast<std::array<int8_t, 8>>(input[EMG_VECTOR]);

    // keep track of raw emg samples, but dont use them for now.
    emgSamplesDeque.push_back(emgSamples);
    // store normalized absolute values of emg data
    std::array<float, 8> absEmgSamplesNorm;
    for (int i = 0; i < 8; i++)
    {
        absEmgSamplesNorm[i] = (float)abs(emgSamples[i]) / ABS_MAX;
    }
    absEmgSamplesNormDeque.push_back(absEmgSamplesNorm);

    if (emgSamplesDeque.size() > FILTER_LEN)
    {
        emgSamplesDeque.pop_front();
    }
    if (absEmgSamplesNormDeque.size() > FILTER_LEN)
    {
        absEmgSamplesNormDeque.pop_front();
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
    std::array<float, 8> absAvg = calcAvg(absEmgSamplesNormDeque);

    // Step 2 - take the max acrrocess the average and raise it to EMG_POWER power
    maxAvg = 0;
    for (std::array<float, 8>::iterator it = absAvg.begin(); it != absAvg.end(); it++)
    {
        maxAvg = std::max(maxAvg, (*it));
    }
    maxAvg = std::pow(maxAvg, EMG_POWER);
    
    // Step 3 - perform logic with various counters to determine if the 
    // system is viewing an impulse
    impulseStatus = risingEdgeImpulseLogic();
    impulseStatus |= fallingEdgeImpulseLogic();
}

bool EMGImpulseFilter::risingEdgeImpulseLogic()
{
    bool retVal = false;
    bool currentlyPosing = (myoStateHandle->getPoseHistory().size() > 0) ? (myoStateHandle->getPoseHistory().back().type() != myo::Pose::rest) : false;

    if (risingLogicMuscleActive == false && maxAvg > RISING_EMG_IMPULSE_THRESHOLD_HIGH && currentlyPosing == false)
    {
        // impulse detected in muscle activity when Myo not sending a pose signal yet.
        // Deem this an impulse
        risingLogicMuscleActive = true;
        risingLogicImpulseCount = IMPULSE_SIZE;
    }

    if (risingLogicMuscleActive && risingLogicImpulseCount > 0)
    {
        retVal = true;
        risingLogicImpulseCount--;
    }
    else
    {
        retVal = false;
    }

    if (risingLogicImpulseCount <= 0 && maxAvg <= RISING_EMG_IMPULSE_THRESHOLD_LOW)
    {
        // reset the risingLogicMuscleActive flag when the pose has been let go after IMPULSE_SIZE steps
        // so that it is searching for the next impulse in the system.
        risingLogicMuscleActive = false;
    }

    return retVal;
}

bool EMGImpulseFilter::fallingEdgeImpulseLogic()
{
    bool retVal = false;

    // Count down to find 'rises'
    if (maxAvg >= FALLING_EMG_IMPULSE_THRESHOLD_HIGH)
    {
        fallingLogicRiseDetection--;
    }
    else
    {
        fallingLogicRiseDetection = FALLING_DETECTION_COUNT;
    }
    if (fallingLogicRiseDetection <= 0)
    {
        fallingLogicMuscleActive = true;
    }

    // Count down to find 'falls'
    if (maxAvg <= FALLING_EMG_IMPULSE_THRESHOLD_LOW)
    {
        fallingLogicFallDetection--;
    }
    else
    {
        fallingLogicFallDetection = FALLING_DETECTION_COUNT;
    }
    if (fallingLogicMuscleActive == true && fallingLogicFallDetection <= 0)
    {
        fallingLogicMuscleActive = false;
        fallingLogicImpulseCount = IMPULSE_SIZE;
    }

    // found impulse signal if impulseCount > 0 when the muscles are inactive
    if (fallingLogicMuscleActive == false && fallingLogicImpulseCount > 0)
    {
        retVal = true;
        fallingLogicImpulseCount--;
    }

    return retVal;
}

std::array<float, 8> EMGImpulseFilter::calcAvg(std::deque<std::array<float, 8>>& dq)
{
    std::array<float, 8> avgs = { 0 };
    std::array<float, 8> sum = { 0 };
    float denom = dq.size();
    if (denom == 0)
    {
        return avgs;
    }

    std::deque<std::array<float, 8>>::iterator it = dq.begin();
    while (it != dq.end())
    {
        std::array<float, 8> currArr = *it;
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