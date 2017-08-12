/*
  ==============================================================================

    DimensionReduction.cpp
    Created: 8 Aug 2017 12:49:23pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "DimensionReduction.h"

#include "tnt/tnt2vector.h"
#include "tnt/jama_lu.h"
#include "tnt/jama_eig.h"

using namespace essentia;
using namespace standard;

DimensionReduction::DimensionReduction(const DBConnector& db, const ReferenceCountedArray<SampleFolder>& f) :
    Thread("Dimension Reduction Thread"), db_(db), sampleFolders_(f)
{
    if (sampleFolders_.size())
    {
        startThread();
    }
    
    sampleClasses_.add(new SampleClassPCA(1, 0.2, 0.1));    // Kick Drum PCA Segmentations
    sampleClasses_.add(new SampleClassPCA(2, 0.5, 0.25));   // Snare Drum PCA Segmentations
    
    essentia::init();
    AlgorithmFactory& factory = AlgorithmFactory::instance();
    
    pca_ = factory.create("PCA",
                          "dimensions", 2,
                          "namespaceIn", "feature",
                          "namespaceOut", "pca");
    
}


DimensionReduction::~DimensionReduction()
{
    stopThread(4000);
}


void DimensionReduction::runDimensionReduction()
{
    startThread();
}


void DimensionReduction::run()
{
    while(!threadShouldExit())
    {
        if (std::all_of(sampleFolders_.begin(), sampleFolders_.end(), [](SampleFolder::Ptr f){ return f->getStatus() == 3; }))
        {
            pca();
        }
        
        else if (std::all_of(sampleFolders_.begin(), sampleFolders_.end(), [](SampleFolder::Ptr f){ return f->getStatus() == 4; }))
        {
            // Everything has already been run through dimension reduction
            signalThreadShouldExit();
        }
            
        wait(1000);
    }
}

void DimensionReduction::pca()
{
    for (auto sampleClass = sampleClasses_.begin(); sampleClass != sampleClasses_.end(); ++sampleClass)
    {
        if (!threadShouldExit())
        {
            // Get all samples for a sample type
            String sql = "SELECT a.* FROM analysis a JOIN samples s ON a.sample_id = s.id " \
                "WHERE s.sample_type = " + String((*sampleClass)->sampleType) + \
                " AND s.exclude = 0;";

            analysisMatrix_.clear();
            sampleOrder_.clear();
            db_.runCommand(sql, selectAnalysisPoolCallback, this);
            
            // Check to see if we need to exit the thread
            if (threadShouldExit()) return;
            
            // how many dimensions are there?
            int bands = analysisMatrix_[0].size();
            
            // calculate covariance for this songs frames
            // before there was an implementation for covariance from Vincent akkerman. I (eaylon) think it is better
            // and more maintainable to use an algorithm that computes covariance.
            // Using singleGaussian algo seems to give slightly different results for variances (8 decimal places)
            TNT::Array2D<Real> matrix, covMatrix, icov;
            std::vector<Real> means;
            matrix = vecvecToArray2D(analysisMatrix_);
            Algorithm* sg = AlgorithmFactory::create("SingleGaussian");
            sg->input("matrix").set(matrix);
            sg->output("mean").set(means);
            sg->output("covariance").set(covMatrix);
            sg->output("inverseCovariance").set(icov);
            sg->compute();
            delete sg;
            
            // calculate eigenvectors, get the eigenvector matrix
            JAMA::Eigenvalue<Real> eigMatrixCalc(covMatrix);
            Array2D<Real>    eigMatrix;
            eigMatrixCalc.getV(eigMatrix);
            
            int nFrames = analysisMatrix_.size();
            for (int row=0; row<nFrames; row++) {
                for (int col=0; col<bands; col++) {
                    analysisMatrix_[row][col] -= means[col];
                }
            }
            
            // reduce dimensions of eigMatrix
            int requiredDimensions = bands;
            if (requiredDimensions > eigMatrix.dim2() || requiredDimensions < 1)
                requiredDimensions = eigMatrix.dim2();
            Array2D<Real> reducedEig(eigMatrix.dim1(), requiredDimensions);
            
            for (int row=0; row<eigMatrix.dim1(); row++) {
                for (int column=0; column<requiredDimensions; column++) {
                    reducedEig[row][column] = eigMatrix[row][column+eigMatrix.dim2()-requiredDimensions];
                }
            }
            
            // transform all the frames and add to the output
            Array2D<Real> featVector(1,bands, 0.0);
            vector<Real> results = vector<Real>(requiredDimensions, 0.0);
            for (int row=0; row<nFrames; row++) {
                for (int col=0; col<bands; col++) {
                    analysisMatrix_[0][col] = analysisMatrix_[row][col];
                }
                featVector = matmult(featVector, reducedEig);
                for (int i=0; i<requiredDimensions; i++) {
                    results[i] = featVector[0][i];
                }
            }
            
            std::cout << "done";

            

            
            
            
        }
    }
}
