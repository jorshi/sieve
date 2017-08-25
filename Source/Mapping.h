/*
  ==============================================================================

    Mapping.h
    Created: 23 Aug 2017 10:52:06am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include <mlpack/methods/neighbor_search/neighbor_search.hpp>
#include "munkres/munkres.h"

class Mapping
{
public:
    
    // Constructor
    Mapping() {};
    
    // Deconstructor
    ~Mapping() {};
    
    // Run KNN mapping
    std::vector<int> mapToGrid(std::vector<std::vector<double>>& columnMat);
    
private:
    
    
    
    
    
    
};
