/*
  ==============================================================================

    Mapping.cpp
    Created: 23 Aug 2017 10:52:06am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "Mapping.h"


using namespace mlpack::neighbor;

void Mapping::mapToGrid(std::vector<std::vector<double> > &columnMat)
{
    arma::mat data(2, columnMat.size());
    
    for (int i = 0; i < columnMat.size(); i++)
    {
        data(0, i) = columnMat[i][0];
        data(1, i) = columnMat[i][1];
    }
    
    KNN knn(data);
    
    arma::Mat<size_t> resultingNeighbors;
    arma::mat resultingDistances;
    
    knn.Search(8, resultingNeighbors, resultingDistances);
    
    
    
    std::cout << "done";
}
