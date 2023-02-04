/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string> 

//==============================================================================
MidiDiffAudioProcessorEditor::MidiDiffAudioProcessorEditor (MidiDiffAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    audioProcessor.updateMidiNotesLabel = [this](int noteNumber)
    {
        lastMidiNotesDisplay.setText(juce::String(noteNumber), juce::dontSendNotification);
    };

    //controlMidiChannel
    addAndMakeVisible(controlMidiChannelLabel);
    initLabel(controlMidiChannelLabel);

    addAndMakeVisible(controlMidiChannelSelector);
    initChannels(controlMidiChannelSelector, audioProcessor.midiChannelReference);
    controlMidiChannelSelector.onChange = [this] {
        audioProcessor.midiChannelReference = controlMidiChannelSelector.getText().getIntValue();
    };

    //performanceMidiChannel
    addAndMakeVisible(performanceMidiChannelLabel);
    initLabel(performanceMidiChannelLabel);

    addAndMakeVisible(performanceMidiChannelSelector);
    initChannels(performanceMidiChannelSelector, audioProcessor.midiChannelPerformance);
    performanceMidiChannelSelector.onChange = [this] { 
        audioProcessor.midiChannelPerformance = performanceMidiChannelSelector.getText().getIntValue();
    };

    //thresholdMidiChannel
    addAndMakeVisible(thresholdLabel);
    initLabel(thresholdLabel);

    addAndMakeVisible(thresholdSelector);
    thresholdSelector.addItem(std::to_string(100), 1);
    thresholdSelector.addItem(std::to_string(200), 2);
    thresholdSelector.addItem(std::to_string(500), 3);
    thresholdSelector.addItem(std::to_string(1000), 4);
    thresholdSelector.setSelectedId(2);
    thresholdSelector.onChange = [this] {
        threshold = thresholdSelector.getText().getIntValue();
    };

    addAndMakeVisible(percentageButton);

    percentageButton.addListener(this);

    startTimer(1000);
}

MidiDiffAudioProcessorEditor::~MidiDiffAudioProcessorEditor()
{
}

//==============================================================================
void MidiDiffAudioProcessorEditor::paint (juce::Graphics& g)
{
    addAndMakeVisible(lastMidiNotesLabel);
    initLabel(lastMidiNotesLabel);

    addAndMakeVisible(lastMidiNotesDisplay);
    initLabel(lastMidiNotesDisplay);

    addAndMakeVisible(midiNoteCounterLabel);
    initLabel(midiNoteCounterLabel);

    addAndMakeVisible(midiNoteCounterDisplay);
    initLabel(midiNoteCounterDisplay);
}

void MidiDiffAudioProcessorEditor::resized()
{
    lastMidiNotesLabel.setBounds(10, 10, 240, 30);
    lastMidiNotesDisplay.setBounds(260, 10, 80, 30);

    midiNoteCounterLabel.setBounds(10, 50, 240, 30);
    midiNoteCounterDisplay.setBounds(260, 50, 80, 30);

    controlMidiChannelLabel.setBounds(10, 90, 240, 30);
    controlMidiChannelSelector.setBounds(260, 90, 80, 30);

    performanceMidiChannelLabel.setBounds(10, 130, 240, 30);
    performanceMidiChannelSelector.setBounds(260, 130, 80, 30);

    thresholdLabel.setBounds(10, 170, 240, 30);
    thresholdSelector.setBounds(260, 170, 80, 30);

    percentageButton.setBounds(10, 210, 120, 40);
}
