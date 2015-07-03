#include "GenericAveragingFilter.h"

void GenericAveragingFilter::process()
{
    filterDataMap input = Filter::getInput();

    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::clearOutput();

    if (input.find(inputMapKey) != input.end())
    {
        boost::any value = input[inputMapKey];
        if (value.type() != typeid(float))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
        }
        else
        {
            processFloat(boost::any_cast<float>(value));
        }
    }
}

void GenericAveragingFilter::processFloat(float datum)
{
    insertAvgElement(datum, dataDeque);

    filterDataMap output;

    output[inputMapKey] = calcAvg(dataDeque);

    Filter::setOutput(output);
}

void GenericAveragingFilter::insertAvgElement(float elem, std::deque<float>& dq)
{
    dq.push_back(elem);
    while (dq.size() > avgCount) {
        // discard element so that average is only based on
        // avgCount elements.
        dq.pop_front();
    }
}

float GenericAveragingFilter::calcAvg(std::deque<float>& dq)
{
    float sum = 0;
    float denom = dq.size();
    if (denom == 0)
    {
        return 0;
    }

    std::deque<float>::iterator it = dq.begin();
    while (it != dq.end())
    {
        sum += *it++;
    }
    return (float)sum / denom;
}


