/*
  ==============================================================================

    Mapping.h
    Created: 23 Aug 2017 10:52:06am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#include <mlpack/methods/neighbor_search/neighbor_search.hpp>

class Mapping
{
public:
    
    // Constructor
    Mapping() {};
    
    // Deconstructor
    ~Mapping() {};
    
    // Run KNN mapping
    void mapToGrid(std::vector<std::vector<double>>& columnMat);
    
private:
    
    
    
    
    
    
};
