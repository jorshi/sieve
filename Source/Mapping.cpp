/*
  ==============================================================================

    Mapping.cpp
    Created: 23 Aug 2017 10:52:06am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "Mapping.h"


using namespace mlpack::neighbor;

std::vector<int> Mapping::mapToGrid(std::vector<std::vector<double> > &columnMat)
{
    
    arma::mat data(2, columnMat.size());
    
    int gridDim = std::ceil(std::log2(columnMat.size()));
    if (gridDim > 8) gridDim = 8;
    
    double maxX = -INFINITY;
    double maxY = -INFINITY;
    double minX = INFINITY;
    double minY = INFINITY;
    
    for (int i = 0; i < columnMat.size(); i++)
    {
        data(0, i) = columnMat[i][0];
        data(1, i) = columnMat[i][1];
        
        if (data(0, i) > maxX) maxX = data(0, i);
        if (data(1, i) > maxY) maxY = data(1, i);
        if (data(0, i) < minX) minX = data(0, i);
        if (data(1, i) < minY) minY = data(1, i);
    }
    
    // Map all samples to a grid of this dimension
    double xDist = (maxX - minX) / gridDim;
    double yDist = (maxY - minY) / gridDim;
    
    // Calculate the grid positions based on the sample spacing
    arma::mat grid(2, columnMat.size());
    grid(0, 0) = minX + (xDist / 2);
    grid(1, 0) = maxY - (yDist / 2);
    for (int i = 1; i < columnMat.size(); i++)
    {
        grid(0, i) = grid(0, 0) + (xDist * (i % gridDim));
        grid(1, i) = grid(1, 0) - (yDist * (i / gridDim));
    }
    
    KNN knn(data);
    
    arma::Mat<size_t> resultingNeighbors;
    arma::mat resultingDistances;
    
    knn.Search(grid, columnMat.size(), resultingNeighbors, resultingDistances);
    
    Matrix<double> matrix(columnMat.size(), columnMat.size());
    for (int i = 0; i < columnMat.size(); i++)
    {
        for (int j = 0; j < columnMat.size(); j++)
        {
            matrix(i, j) = resultingDistances(j, i);
        }
    }
    
    // Kuhn-Munkres Algorithm https://github.com/saebyn/munkres-cpp
    Munkres<double> m;
    m.solve(matrix);
    
    std::vector<int> assignments(columnMat.size(), -1);


    for ( int row = 0 ; row < columnMat.size() ; row++ ) {
        for ( int col = 0 ; col < columnMat.size() ; col++ ) {
            if (matrix(row, col) == 0)
            {
                // Expand back out to 64 grid arrangement
                assignments[col] = row + ((row / gridDim) * (8 - gridDim));
            }
        }
    }
    
    return assignments;
}
