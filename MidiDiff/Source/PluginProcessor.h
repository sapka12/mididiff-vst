/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MidiDiff.h"

//==============================================================================
/**
*/
class MidiDiffAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    MidiDiffAudioProcessor();
    ~MidiDiffAudioProcessor() override;
    
    std::function<void(int)> updateMidiNotesLabel;
    int midiNoteCounter = 0;

    int midiChannelReference = 1;
    int midiChannelPerformance = 2;

    std::unique_ptr<juce::FileLogger> m_flogger;

    MidiDiff midiDiff;

    void resetMidiCounters() {
        midiNoteCounter = 0;
        midiDiff.controlMidiEvents.clear();
        midiDiff.performanceMidiEvents.clear();
    }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    long currentBufferEventTimeStartEpochMillis;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiDiffAudioProcessor)

    void log(juce::String message) {
        if (m_flogger) {
            m_flogger->logMessage(message);
        }
    }
};
