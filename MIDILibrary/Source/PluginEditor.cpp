/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MIDILibraryAudioProcessorEditor::MIDILibraryAudioProcessorEditor (MIDILibraryAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(loadButton);
    addAndMakeVisible(midiFilesList);
    addAndMakeVisible(filesDirectoryLabel);
    loadButton.setButtonText("Load midi files folder");
    loadButton.onClick = [this](){loadUserMIDIFiles();};
    midiFilesList.onChange = [this](){
        auto midiFileStream = FileInputStream(audioProcessor.userMIDIFiles.getReference(midiFilesList.getSelectedId()));
        audioProcessor.currentMidiFile.readFrom(midiFileStream);
        DBG("success");
    };
    setSize (400, 300);
}

MIDILibraryAudioProcessorEditor::~MIDILibraryAudioProcessorEditor()
{
}

//==============================================================================
void MIDILibraryAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MIDILibraryAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    filesDirectoryLabel.setBounds(bounds.removeFromTop(proportionOfHeight(0.1)));
    loadButton.setBounds(bounds.removeFromTop(proportionOfHeight(0.5)));
    midiFilesList.setBounds(bounds);
    
}

void MIDILibraryAudioProcessorEditor::loadUserMIDIFiles()
{
    fileChooser = std::make_unique<FileChooser>(
                                                 "Please select MIDI folder"
                                                 );
    
    fileChooser->launchAsync(FileBrowserComponent::canSelectDirectories, [&](const FileChooser& chooser)
                             {
        const auto result = chooser.getResult();
        if(result.exists() && result.isDirectory())
        {
            userMidiFilesDirectory = result;
            auto resultArray = result.findChildFiles(File::TypesOfFileToFind::findFiles, true, "*.mid");
            audioProcessor.userMIDIFiles = resultArray;
            populateChoiceBox();
            if(!audioProcessor.userMIDIFiles.isEmpty())
            {
                MidiFilesLoaded = true;
                filesDirectoryLabel.setText(userMidiFilesDirectory.getFullPathName(), dontSendNotification);
                loadButton.setButtonText("change folder");
            }
        }
        
    });
}

StringArray MIDILibraryAudioProcessorEditor::getAllUserMidiFiles() const
{
    StringArray midiFiles;
    for(const auto& file : audioProcessor.userMIDIFiles)
    {
        midiFiles.add(file.getFileNameWithoutExtension());
    }
    return midiFiles;
}

void MIDILibraryAudioProcessorEditor::populateChoiceBox()
{
    midiFilesList.clear(dontSendNotification);
    const auto allFiles =  getAllUserMidiFiles();
//    const auto currentPreset = presetManager.getCurrentPreset();
    if(!allFiles.isEmpty())
    {
        midiFilesList.addItemList(allFiles, 1);
        midiFilesList.setSelectedItemIndex(0, dontSendNotification);
        midiFilesList.setSelectedId(0);
    }
}
