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
class MidiDiffAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer,
    public juce::Button::Listener
{
public:
    MidiDiffAudioProcessorEditor (MidiDiffAudioProcessor&);
    ~MidiDiffAudioProcessorEditor() override;

    void buttonClicked(juce::Button* button) override
    {
        audioProcessor.resetMidiCounters();
    }
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MidiDiffAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDiffAudioProcessorEditor)

    juce::Label controlMidiChannelLabel{ {}, "Control MIDI Channel" };
    juce::ComboBox controlMidiChannelSelector;

    juce::Label performanceMidiChannelLabel{ {}, "Performance MIDI Channel" };
    juce::ComboBox performanceMidiChannelSelector;

    juce::Label thresholdLabel{ {}, "Threshold (ms)" };
    juce::ComboBox thresholdSelector;
    int threshold = 100;

    juce::TextButton percentageButton = juce::TextButton("percentage");

    void initLabel(juce::Label& label) {
        label.setFont(juce::Font(16.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    }

    void initChannels(juce::ComboBox& menu, int selected) {
        for (int i = 1; i <= 16; i++)
        {
            menu.addItem(std::to_string(i), i);
        }
        menu.setSelectedId(selected);
    }

    void timerCallback() final {
        auto percentage = int(audioProcessor.percentageOfPerformance(threshold));
        percentageButton.setButtonText(juce::String(percentage) + "%");
    }
};
