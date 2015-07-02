#include "GenericBypassFilter.h"

void GenericBypassFilter::process()
{
    filterDataMap output;
    filterDataMap input = Filter::getInput();

    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::clearOutput();

    if (passThroughKeys.size() == 0)
    {
        output = input;
        Filter::setOutput(output);
    }
    else
    {
        for (int i = 0; i < passThroughKeys.size(); i++)
        {
            if (input.find(passThroughKeys[i]) != input.end())
            {
                output[passThroughKeys[i]] = input[passThroughKeys[i]];
            }
        }
        Filter::setOutput(output);
    }
}