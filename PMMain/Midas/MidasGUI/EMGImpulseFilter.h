#ifndef EMG_IMPULSE_FILTER_H
#define EMG_IMPULSE_FILTER_H

#include "Filter.h"

#include <array>
#include <deque>
#include <myo/myo.hpp>

#define FILTER_LEN 5 // Averaging EMG data with weighted filter.
#define ABS_MAX 128 // EMG data ranges from -128 to +127. 
#define IMPULSE_SIZE 40 // Length of impulse once detected (should be 200ms at 200Hz)
#define EMG_POWER 3.0
#define MAX_EMG_IMPULSE_THRESHOLD_HIGH 0.2
#define MAX_EMG_IMPULSE_THRESHOLD_LOW 0.01

#define NOMINMAX

class MyoState;

class EMGImpulseFilter : public Filter
{
public:
    /* Basic Construction/Destruction */
    EMGImpulseFilter(MyoState* myoState);
    ~EMGImpulseFilter();

    /**
    *
    */
    void process();

private:

    void updateImpulseStatus();

    bool performImpulseLogic();

    /**
    * Calculate the average of the contents in the deque.
    *
    * @param dq - a deque containing <= avgCount std::array<int8_t, 8>
    * @return std::array<float, 8> - the average of the contents of the deque
    */
    std::array<float, 8> calcAvg(std::deque<std::array<int8_t, 8>>& dq);

    // store the maximum avg accross all 8 channels taken to EMG_POWER power
    float maxAvg;

    // the impulse status of the system
    bool impulseStatus;

    // store FILTER_LEN EMG samples at a time
    std::deque<std::array<int8_t, 8>> emgSamplesDeque;
    std::deque<std::array<int8_t, 8>> absEmgSamplesDeque;

    int impulseCount = 0;
    bool currMuscleActive = false;

    MyoState *myoStateHandle;
};

#endif /* EMG_IMPULSE_FILTER_H */