#ifndef GENERIC_BYPASS_FILTER_H
#define GENERIC_BYPASS_FILTER_H

#include "Filter.h"

#include <vector>
#include <string>

/**
*
* This filter passes the input map directly 
* to the output. Used to take advantage of AdvancedFilterPipeline
* Architecture.
*
*/
class GenericBypassFilter : public Filter
{
public:
    /* Basic Construction/Destruction */
    GenericBypassFilter() {}
    ~GenericBypassFilter() {}

    /**
    * Take input data and place on outputDataMap. Filter by passThroughKeys
    * if non-empty.
    */
    void process();
    
    /**
    * Mutator
    */
    void setPassThroughKeys(std::vector<std::string> passThroughKeys) { this->passThroughKeys = passThroughKeys; }

    /**
    * Mutator
    */
    void addPassThroughKey(std::string key) { this->passThroughKeys.push_back(key); }

    /**
    * Accessor
    */
    std::vector<std::string> getPassThroughKeys() const { return this->passThroughKeys; }

private:
    std::vector<std::string> passThroughKeys;
};

#endif