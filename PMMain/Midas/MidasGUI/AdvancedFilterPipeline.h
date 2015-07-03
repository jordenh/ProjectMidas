#ifndef ADVANCED_FILTER_PIPELINE_H
#define ADVANCED_FILTER_PIPELINE_H

#include <vector>
#include "Filter.h"

/**
* The AdvancedFilterPipeline class represents a 1-to-1, many-to-1, 1-to-many, or many-to-many 
* sequence of filters through which data can flow through. The data enters the pipeline and 
* is passed into the first level of filters. The output of the first level is piped into the 
* second level, and so on.
* Not all levels need consume the entire filterDataMap contents, but will manipulate and pass
* along
*/
class AdvancedFilterPipeline
{
public:
    /* Basic Construction/Destruction */
    AdvancedFilterPipeline() { filtersOwned = false; }
    ~AdvancedFilterPipeline();

    /**
    * Registers a new filter with the pipeline at a specified level. 
    *
    * @param filter The filter to add to the pipeline. 
    * @param level The desired level to register the filter. If this level 
    * exceeds the current depth of the pipeline, it will register at the 
    * deepest level possible.
    */
    void registerFilterAtLevel(Filter* filter, unsigned int level);

    /**
    * Registers a new filter with the pipeline. Adds it to the current deepest level.
    *
    * @param filter The filter to add to the pipeline.
    */
    void registerFilterAtDeepestLevel(Filter* filter);

    /**
    * Registers a new filter with the pipeline. Adds it to a new level, hooking its inputs
    * up to all outputs of the prior level.
    *
    * @param filter The filter to add to the pipeline.
    */
    void registerFilterAtNewLevel(Filter* filter);

    /**
    * Starts the pipeline with the given input.
    *
    * @param input A map of name-value pairs that will be passed as the input to the first
    * filter in the pipeline.
    */
    void startPipeline(filterDataMap input);

    /**
    * Returns the vector of registered filters at a given level.
    *
    * @return The vector of registered filters at a given level.
    * returns NULL if level is not populated.
    */
    std::vector<Filter*>* getFiltersAtLevel(unsigned int level);

    /**
    * Returns the vector of vectors of registered filters.
    *
    * @return The vector of vectors of registered filters.
    */
    std::vector<std::vector<Filter*>>* getFiltersHandle(void);

    void setFiltersOwned(bool owned) { this->filtersOwned = owned; }

    /**
    * Update all filters internal mechanisms if the profile changes.
    *
    * @return The error code of the filter after completion.
    */
    filterError updateFiltersBasedOnProfile(ProfileManager& pm, std::string name);

private:
    std::vector<std::vector<Filter*>> filters;

    bool filtersOwned;
};

#endif ADVANCED_FILTER_PIPELINE_H