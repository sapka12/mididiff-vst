/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
using namespace std;
typedef vector< tuple<long, int> > EventListType;

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
    
    int midiChannelReference = 1;
    int midiChannelPerformance = 2;

    std::unique_ptr<juce::FileLogger> m_flogger;

    void resetMidiCounters() {
        controlMidiEvents.clear();
        performanceMidiEvents.clear();
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


    EventListType controlMidiEvents;
    EventListType performanceMidiEvents;

    double percentageOfPerformance(int thresholdMs) {
        auto& control = controlMidiEvents;
        auto& perform = performanceMidiEvents;

        long sumOfDistances = 0;
        for (const tuple<long, int> controlEvt : control) {
            long eventTime = std::get<0>(controlEvt);
            int midiNote = std::get<1>(controlEvt);
            sumOfDistances += differenceOfSameNotes(eventTime, midiNote, perform, thresholdMs);
        }

        auto noControl = control.size() == 0;
        if (noControl) {
            return 0;
        }

        double averageDistance = sumOfDistances * 1.0 / control.size();
        double percentage = 100 - (averageDistance * 100.0 / thresholdMs);
        return percentage;
    };

private:
    long currentBufferEventTimeStartEpochMillis;

    int differenceOfSameNotes(long controlTime, int controlMidiNote, EventListType currentMidiEvents, int threshold) {
        int minDistance = threshold;
        for (const tuple<long, int> midiEvt : currentMidiEvents) {
            long eventTime = std::get<0>(midiEvt);
            int midiNote = std::get<1>(midiEvt);
            int currentDistance = abs(controlTime - eventTime);
            if (controlMidiNote == midiNote && currentDistance < minDistance) {
                minDistance = currentDistance;
            }
        }
        return minDistance;
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiDiffAudioProcessor)

    void log(juce::String message) {
        if (m_flogger) {
            m_flogger->logMessage(message);
        }
    }
};
