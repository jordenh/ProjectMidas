#ifndef _GENERIC_AVERAGING_FILTER_H
#define _GENERIC_AVERAGING_FILTER_H

#include "Filter.h"
#include <deque>

/**
*
* This filter registers an input key and averages it's values
* as a float
*
*/
class GenericAveragingFilter : public Filter
{
public:
    /* Basic Construction/Destruction */
    GenericAveragingFilter(unsigned int avgCount, std::string inputMapKey) : avgCount(avgCount), inputMapKey(inputMapKey) {}
    ~GenericAveragingFilter() {}

    /**
    * Access data from Inputs, average the data
    * based on avgCount then
    * output the averaged value.
    */
    void process();

    /**
    * Setter
    */
    void setAvgCount(unsigned int avgCount) { this->avgCount = avgCount; }

    /**
    * Accessor
    */
    unsigned int getAvgCount() const { return this->avgCount; }

    /**
    * Setter
    */
    void setInputMapKey(std::string inputMapKey) { this->inputMapKey = inputMapKey; }

    /**
    * Accessor
    */
    std::string getInputMapKey() const { return this->inputMapKey; }

private:
    /*
    * Helper for process()
    */
    void processFloat(float datum);

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

    // Number of floats to average. This value dictates max size of deques.
    unsigned int avgCount;

    std::string inputMapKey;

    std::deque<float> dataDeque;
};

#endif /* _AVERAGING_FILTER_H */