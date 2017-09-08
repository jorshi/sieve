/*
  ==============================================================================

    Mapping.h
    Created: 23 Aug 2017 10:52:06am
    Author:  Jordie Shier 

  ==============================================================================
*/

#pragma once

#ifdef DEBUG
#define J_DEBUG
#undef DEBUG
#endif

#include <mlpack/methods/neighbor_search/neighbor_search.hpp>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <mlpack/core/util/log.hpp>
#include <mlpack/core/util/nulloutstream.hpp>
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
    
    // Run K-Means clustering
    std::vector<std::pair<std::vector<int>, int>> cluster(std::vector<std::vector<double>>& columnMat);
    
private:
    
};

#ifdef J_DEBUG
#define DEBUG
#endif
