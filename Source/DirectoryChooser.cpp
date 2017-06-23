/*
  ==============================================================================

    DirectoryChooser.cpp
    Created: 12 Jun 2017 4:35:56pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "DirectoryChooser.h"

DirectoryChooser::DirectoryChooser()
{
    fileChooser_ = new FileChooser("Select sample directory");
    
}

bool DirectoryChooser::getDirectory(File& file)
{
    if (fileChooser_->browseForDirectory())
    {
        file = fileChooser_->getResult();
        return true;
    }
    
    return false;
}
