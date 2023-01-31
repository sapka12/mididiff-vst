/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MidiDiffAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    MidiDiffAudioProcessorEditor (MidiDiffAudioProcessor&);
    ~MidiDiffAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MidiDiffAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDiffAudioProcessorEditor)

    juce::Label lastMidiNotesLabel = juce::Label();
    juce::Label midiNoteCounterLabel = juce::Label();

    void timerCallback() final {
        midiNoteCounterLabel.setText(juce::String(audioProcessor.midiNoteCounter), juce::dontSendNotification);
    }
};
