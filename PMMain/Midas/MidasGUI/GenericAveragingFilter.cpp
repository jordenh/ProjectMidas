#include "GenericAveragingFilter.h"

void GenericAveragingFilter::process()
{
    filterDataMap input = Filter::getInput();

    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::clearOutput();

    if (input.find(inputMapKey) != input.end())
    {
        boost::any value = input[inputMapKey];
        if (value.type() != typeid(double))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
        }
        else
        {
            processDouble(boost::any_cast<double>(value));
        }
    }
}

void GenericAveragingFilter::processDouble(double datum)
{
    insertAvgElement(datum, dataDeque);

    filterDataMap output;

    output[inputMapKey] = calcAvg(dataDeque);

    Filter::setOutput(output);
}

void GenericAveragingFilter::insertAvgElement(double elem, std::deque<double>& dq)
{
    dq.push_back(elem);
    while (dq.size() > avgCount) {
        // discard element so that average is only based on
        // avgCount elements.
        dq.pop_front();
    }
}

double GenericAveragingFilter::calcAvg(std::deque<double>& dq)
{
    double sum = 0;
    double denom = dq.size();
    if (denom == 0)
    {
        return 0;
    }

    std::deque<double>::iterator it = dq.begin();
    while (it != dq.end())
    {
        sum += *it++;
    }
    return (double)sum / denom;
}


