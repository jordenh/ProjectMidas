/*
    Copyright (C) 2015 Midas

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

#include "AdvancedFilterPipeline.h"

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

AdvancedFilterPipeline::~AdvancedFilterPipeline()
{
    // If filters ever become owned by this pipeline, code to handle mem is as follows
    if (false)
    {
        // traverses the vector of filters at each level
        std::vector<std::vector<Filter*>>::iterator levelIt;
        // traverses the given filters in each level
        std::vector<Filter*>::iterator filterIt;

        for (levelIt = filters.begin(); levelIt != filters.end(); levelIt++)
        {
            std::vector<Filter*> levelFilters = *levelIt;

            for (filterIt = levelFilters.begin(); filterIt != levelFilters.end(); filterIt++)
            {
                Filter* currentFilter = *filterIt;
                delete *filterIt; *filterIt = NULL;
            }
            levelFilters.clear();
        }
        filters.clear();
    }
}

void AdvancedFilterPipeline::registerFilterAtDeepestLevel(Filter* filter)
{
    int deepestIdx = filters.size() - 1;

    if (deepestIdx < 0)
    {
        std::vector<Filter*> firstLevel;
        filters.push_back(firstLevel);
        deepestIdx = 0;
    }

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
                // TODO - handle error
                //throw("AdvancedFilterPipeline Filter Error.");
            }

            if (Filter::mapCollision(levelOutput, currentFilter->getOutput()))
            {
                // TODO - handle error
                //throw("AdvancedFilterPipeline Map Output Error. Collision Detected");
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

filterError AdvancedFilterPipeline::updateFiltersBasedOnProfile(ProfileManager& pm, std::string name)
{
    int error = (int)filterError::NO_FILTER_ERROR;

    // traverses the vector of filters at each level
    std::vector<std::vector<Filter*>>::iterator levelIt;
    // traverses the given filters in each level
    std::vector<Filter*>::iterator filterIt;

    for (levelIt = filters.begin(); levelIt != filters.end(); levelIt++)
    {
        std::vector<Filter*> levelFilters = *levelIt;

        for (filterIt = levelFilters.begin(); filterIt != levelFilters.end(); filterIt++)
        {
            Filter* currentFilter = *filterIt;
            error |= (int)currentFilter->updateBasedOnProfile(pm, name);
        }
    }
    return (filterError)error;
}