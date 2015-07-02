#ifndef _AVERAGING_FILTER_H
#define _AVERAGING_FILTER_H

#include "Filter.h"
#include "FilterKeys.h"
#include <deque>
#include <cstdint>

#define DEFAULT_AVG_COUNT 20

/**
 * Consult Filter.h for concepts regarding Filters.
 * 
 * The purpose of this filter is to take Accelerometer,
 * Gyroscope, and Quaternion data as input, and average
 * said data over a set number of iterations. This will 
 * flush out any spikes.
 *
 * TODO: 1) Determine proper values of avgCount for good usability
 * 2) As soon as Many to one filters are feasible, change this filter 
 * to only filter one float value, and then implement as many as needed.
 */
class AveragingFilter : public Filter
{
public:
    /* Basic Construction/Destruction */
    AveragingFilter();
    AveragingFilter(unsigned int avgCount);
    ~AveragingFilter();

    /**
     * Access data from Inputs, average the data
     * based on avgCount, and current state, then
     * output the averaged value.
     */
    void process();

    /** 
     * Setter 
     */
    void setAvgCount(unsigned int avgCount);

    /** 
     * Accessor 
     */
    unsigned int getAvgCount() const;

private:
    /**
     * Inject elem into dq and if necessary, pop
     * front of dq to ensure only max avgCount elements
     *
     * @param elem - a float to be added to deque
     * @param dq -  a deque containing <= avgCount floats
     */
    void insertAvgElement(float elem, std::deque<float>& dq);

    /**
     * Calculate the average of the contents in the deque.
     *
     * @param dq - a deque containing <= avgCount floats
     * @return float - the average of the contents of the deque                  
     */
    float calcAvg(std::deque<float>& dq);

    /**
     * Replace the last element of dq with elem
     *
     * @param elem - the value that will replace the last element of the deque
     */
    void replaceLastElement(float elem, std::deque<float>& dq);

    // Number of floats to average. This value dictates max size of deques.
    unsigned int avgCount;

    //Arrays to store the avg data. -- TODO - when we get Filter
    //  Many-to-one working, genericize this to just filtering one float
    // Deques to hold sensor data
    std::deque<float> quatXDeque;
    std::deque<float> quatYDeque;
    std::deque<float> quatZDeque;
    std::deque<float> quatWDeque;
    std::deque<float> accelXDeque;
    std::deque<float> accelYDeque;
    std::deque<float> accelZDeque;
    std::deque<float> gyroXDeque;
    std::deque<float> gyroYDeque;
    std::deque<float> gyroZDeque;
    std::deque<float> rssiDeque;
};

#endif /* _AVERAGING_FILTER_H */