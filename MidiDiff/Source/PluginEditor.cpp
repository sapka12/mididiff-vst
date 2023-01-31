/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiDiffAudioProcessorEditor::MidiDiffAudioProcessorEditor (MidiDiffAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    startTimer(50);
    audioProcessor.updateMidiNotesLabel = [this](int noteNumber)
    {
        lastMidiNotesLabel.setText(juce::String(noteNumber), juce::dontSendNotification);
    };
}

MidiDiffAudioProcessorEditor::~MidiDiffAudioProcessorEditor()
{
}

//==============================================================================
void MidiDiffAudioProcessorEditor::paint (juce::Graphics& g)
{
    addAndMakeVisible(midiNoteCounterLabel);
    midiNoteCounterLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    midiNoteCounterLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);

    addAndMakeVisible(lastMidiNotesLabel);
    lastMidiNotesLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    lastMidiNotesLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
}

void MidiDiffAudioProcessorEditor::resized()
{
    midiNoteCounterLabel.setBounds(10, 10, getWidth() - 20, 30);
    lastMidiNotesLabel.setBounds(10, 50, getWidth() - 20, 30);
}
