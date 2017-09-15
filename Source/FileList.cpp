/*
  ==============================================================================

    FileList.cpp
    Created: 12 Sep 2017 1:38:27pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FileList.h"

//==============================================================================
FileList::FileList(SampleManager* m) : manager_(m)
{
}

FileList::~FileList()
{
}

void FileList::paint (Graphics& g)
{
    // Draw the table entries with alternating colours
    ReferenceCountedArray<SampleFolder>& folders = manager_->getSampleFolders();
    for (int i = 0; i < folders.size(); i++)
    {
        if (i % 2 == 0)
        {
            g.setGradientFill(CustomLookAndFeel::Colours::background);
        }
        else
        {
            g.setColour(CustomLookAndFeel::Colours::darkerTable);
        }
        
        g.fillRect(0, (i*40), 582, 40);
        
        g.setColour(CustomLookAndFeel::Colours::headerText);
        g.drawText(folders.getUnchecked(i)->getFile().getFileName(), 5, (i*40), 400, 39, Justification::centredLeft);
        
        String status = folders.getUnchecked(i)->getStatusStr();
        
        if (folders.getUnchecked(i)->getStatus() == 2)
        {
            status += " - " + String(folders.getUnchecked(i)->getPercentAnalyzed(), 2) + "%";
        }
        
        g.drawText(status, 455, (i*40), 150, 39, Justification::centredLeft);
    }
}

void FileList::resized()
{
}


int FileList::getRequiredHeight()
{
    size_t numSampleFolders = manager_->getSampleFolders().size();
    return (int)numSampleFolders * 40;
}
