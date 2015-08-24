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

#ifndef WINDOW_MAX_FILTER_H
#define WINDOW_MAX_FILTER_H

#include "Filter.h"
#include <deque>

/**
*
* This filter registers an input key and outputs the max 
* of the last windowSize values
*
*/
class GenericWindowMaxFilter : public Filter
{
public:
    /* Basic Construction/Destruction */
    GenericWindowMaxFilter(unsigned int windowSize, std::string inputMapKey) : windowSize(windowSize), inputMapKey(inputMapKey) {}
    ~GenericWindowMaxFilter();

    /**
    * 
    */
    void process();

    /**int
    * Setter
    */
    void setWindowSize(unsigned int windowSize) { this->windowSize = windowSize; }

    /**
    * Accessor
    */
    unsigned int getWindowSize() const { return this->windowSize; }

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
    void processInt(int datum);

    /**
    * Inject elem into dq and if necessary, pop
    * front of dq to ensure only max windowSize elements
    *
    * @param elem - a int to be added to deque
    * @param dq -  a deque containing <= windowSize ints
    */
    void insertElement(int elem, std::deque<int>& dq);

    /**
    * Calculate the max of the contents in the deque.
    *
    * @param dq - a deque containing <= windowSize ints
    * @return int - the average of the contents of the deque
    */
    int calc(std::deque<int>& dq);

    // Number of ints to average. This value dictates max size of deques.
    unsigned int windowSize;

    std::string inputMapKey;

    std::deque<int> dataDeque;
};

#endif /* _AVERAGING_FILTER_H */