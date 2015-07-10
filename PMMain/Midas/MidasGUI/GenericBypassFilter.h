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
    GenericBypassFilter(std::string passThroughKey);
    GenericBypassFilter(std::vector<std::string> passThroughKeys);
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