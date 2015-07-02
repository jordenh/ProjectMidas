#include "AdvancedFilterPipeline.h"

#include <exception>

void AdvancedFilterPipeline::registerFilterAtLevel(Filter* filter, unsigned int level)
{
    if (level < filters.size())
    {
        filters.at(level).push_back(filter);
    }
    else
    {
        registerFilterAtDeepestLevel(filter);
    }
}

void AdvancedFilterPipeline::registerFilterAtDeepestLevel(Filter* filter)
{
    int deepestIdx = filters.size() - 1;

    filters.at(deepestIdx).push_back(filter);
}

void AdvancedFilterPipeline::registerFilterAtNewLevel(Filter* filter)
{
    std::vector<Filter*> newLevel;
    newLevel.push_back(filter);
    filters.push_back(newLevel);
}

void AdvancedFilterPipeline::startPipeline(filterDataMap input)
{
    // process a level at a time, passing each filter the same input map, then
    // concatenating output maps to build the input map for the next level.
    // Throw an exception if there is a collision in outputs from a level.

    // traverses the vector of filters at each level
    std::vector<std::vector<Filter*>>::iterator levelIt;
    // traverses the given filters in each level
    std::vector<Filter*>::iterator filterIt;
    filterDataMap nextLevelInput = input;
    filterDataMap levelOutput;

    for (levelIt = filters.begin(); levelIt != filters.end(); levelIt++)
    {
        std::vector<Filter*> levelFilters = *levelIt;
        levelOutput.clear();

        for (filterIt = levelFilters.begin(); filterIt != levelFilters.end(); filterIt++)
        {
            Filter* currentFilter = *filterIt;
            currentFilter->setInput(nextLevelInput);
            currentFilter->process();
            if (currentFilter->getFilterStatus() == filterStatus::END_CHAIN)
            {
                break;
            }
            else if (currentFilter->getFilterStatus() == filterStatus::FILTER_ERROR)
            {
                throw("AdvancedFilterPipeline Filter Error.");
            }

            if (Filter::mapCollision(levelOutput, currentFilter->getOutput()))
            {
                throw("AdvancedFilterPipeline Map Output Error. Collision Detected");
            }

            levelOutput = Filter::joinFilterDataMaps(levelOutput, currentFilter->getOutput());
        }

        nextLevelInput = levelOutput;
    }
}

std::vector<Filter*>* AdvancedFilterPipeline::getFiltersAtLevel(unsigned int level)
{
    if (level < filters.size())
    {
        return &filters.at(level);
    }
    
    return NULL;
}

std::vector<std::vector<Filter*>>* AdvancedFilterPipeline::getFiltersHandle(void)
{
    return &filters;
}
