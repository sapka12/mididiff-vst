/*
  ==============================================================================

   This file is part of the JUCE examples.
   Copyright (c) 2022 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:                  MIDILogger
 version:               1.0.0
 vendor:                JUCE
 website:               http://juce.com
 description:           Logs incoming MIDI messages.

 dependencies:          juce_audio_basics, juce_audio_devices, juce_audio_formats,
                        juce_audio_plugin_client, juce_audio_processors,
                        juce_audio_utils, juce_core, juce_data_structures,
                        juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:             xcode_mac, vs2022, linux_make

 moduleFlags:           JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:                  AudioProcessor
 mainClass:             MidiLoggerPluginDemoProcessor

 useLocalCopy:          1

 pluginCharacteristics: pluginWantsMidiIn, pluginProducesMidiOut, pluginIsMidiEffectPlugin

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include <iterator>
using namespace std;
using namespace std::chrono;
typedef vector< tuple<long, int> > EventListType;


class MidiDiffResult
{
private:
    int percentage;
    int inThreshold;
    int lastUsedMidiChannel;
public:
    MidiDiffResult(int percentage, int lastUsedMidiChannel, int inThreshold) {
        this->percentage = percentage;
        this->lastUsedMidiChannel = lastUsedMidiChannel;
        this->inThreshold = inThreshold;
    }
    ~MidiDiffResult() {}

    juce::String getPerformance() {
        return juce::String(percentage);
    }

    juce::String getInThreshold() {
        return juce::String(inThreshold);
    }

    int getLastUsedMidiChannel () {
        return lastUsedMidiChannel;
    }
};


class MidiDiffModel
{
public: 
    //mididiff variables begin
    int threshold = 100;
    EventListType controlMidiEvents;
    EventListType performanceMidiEvents;

    int lastUsedMidiChannel = -1;
    int midiChannelReference = 1;
    int midiChannelPerformance = 10;

    void resetMidiCounters() {
        controlMidiEvents.clear();
        performanceMidiEvents.clear();
    }

    MidiDiffResult calculateResult() {
        auto& control = controlMidiEvents;
        auto& perform = performanceMidiEvents;

        long sumOfDistances = 0;
        int inThresholdCount = 0;
        for (const tuple<long, int> controlEvt : control) {
            long eventTime = std::get<0>(controlEvt);
            int midiNote = std::get<1>(controlEvt);
            auto currentDiff = differenceOfSameNotes(eventTime, midiNote, perform);
            if (currentDiff < threshold) { inThresholdCount++; }
            sumOfDistances += currentDiff;
        }

        auto controlNoteCount = control.size();
        auto noControl = controlNoteCount == 0;
        if (noControl) {
            return MidiDiffResult(0, lastUsedMidiChannel, 0);
        }

        int inThreshold = inThresholdCount * 100.0 / controlNoteCount;
        double averageDistance = sumOfDistances * 1.0 / controlNoteCount;
        int percentage = 100 - (averageDistance * 100.0 / threshold);
        return MidiDiffResult(percentage, lastUsedMidiChannel, inThreshold);
    };

private:

    int differenceOfSameNotes(long controlTime, int controlMidiNote, EventListType currentMidiEvents) {
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

};



//==============================================================================
class MidiDiffPluginProcessor  : public AudioProcessor
{
public:
    MidiDiffPluginProcessor()
        : AudioProcessor (getBusesLayout())
    {
        //startTimerHz (60);
    }

    ~MidiDiffPluginProcessor() override { 
    //    stopTimer(); 
    }

    long currentBufferEventTimeStartEpochMillis;

    long toLong(double d) {
        return long(round(1000 * d));
    }
    //mididiff variables end

    void processBlock (AudioBuffer<float>& audio,  MidiBuffer& midi) override { process (audio, midi); }
    void processBlock (AudioBuffer<double>& audio, MidiBuffer& midi) override { process (audio, midi); }

    bool isBusesLayoutSupported (const BusesLayout&) const override           { return true; }
    bool isMidiEffect() const override                                        { return true; }
    bool hasEditor() const override                                           { return true; }
    AudioProcessorEditor* createEditor() override                             { return new Editor (*this); }

    const String getName() const override                                     { return "MIDI Logger"; }
    bool acceptsMidi() const override                                         { return true; }
    bool producesMidi() const override                                        { return true; }
    double getTailLengthSeconds() const override                              { return 0.0; }

    int getNumPrograms() override                                             { return 0; }
    int getCurrentProgram() override                                          { return 0; }
    void setCurrentProgram (int) override                                     {}
    const String getProgramName (int) override                                { return "None"; }
    void changeProgramName (int, const String&) override                      {}

    void prepareToPlay (double, int) override                                 {}
    void releaseResources() override                                          {}

    void getStateInformation (MemoryBlock& destData) override
    {
        if (auto xmlState = state.createXml())
            copyXmlToBinary (*xmlState, destData);
    }

    void setStateInformation (const void* data, int size) override
    {
        if (auto xmlState = getXmlFromBinary (data, size))
            state = ValueTree::fromXml (*xmlState);
    }

private:

    class Editor  : public AudioProcessorEditor, juce::Button::Listener,
        private Value::Listener, Timer
    {
    public:

        // inputs
        juce::ComboBox controlMidiChannelSelector;
        juce::ComboBox performanceMidiChannelSelector;
        juce::ComboBox thresholdSelector;

        // outputs
        juce::TextButton percentageButton = juce::TextButton("Reset");

        juce::Label lastUsedMidiChannelLabel{ {}, "Last Used Channel" };
        juce::Label lastUsedMidiChannelText{ {}, "...1" };

        juce::Label performanceLabel{ {}, "Performance" };
        juce::Label performanceText{ {}, "...2" };

        juce::Label inThresholdLabel{ {}, "In Threshold" };
        juce::Label inThresholdText{ {}, "...3" };

        //operations
        void setData(MidiDiffResult result) {
            performanceText
                .setText(juce::String(result.getPerformance()) + "%", juce::dontSendNotification);
            lastUsedMidiChannelText
                .setText(juce::String(result.getLastUsedMidiChannel()), juce::dontSendNotification);
            inThresholdText
                .setText(juce::String(result.getInThreshold() + "%"), juce::dontSendNotification);
        }

        void buttonClicked(juce::Button* button) override
        {
            owner.model.resetMidiCounters();
        }

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

        ~Editor() override {
                stopTimer(); 
        }

        explicit Editor (MidiDiffPluginProcessor& ownerIn)
            : AudioProcessorEditor (ownerIn),
              owner (ownerIn)
        {
            setSize(19 * s, 11 * s);

            addAndMakeVisible(lastUsedMidiChannelLabel);
            initLabel(lastUsedMidiChannelLabel);
            addAndMakeVisible(lastUsedMidiChannelText);
            initLabel(lastUsedMidiChannelText);
            lastUsedMidiChannelText.setJustificationType(juce::Justification::centredRight);


            addAndMakeVisible(performanceLabel);
            initLabel(performanceLabel);
            addAndMakeVisible(performanceText);
            initLabel(performanceText);
            performanceText.setJustificationType(juce::Justification::centredRight);

            addAndMakeVisible(inThresholdLabel);
            initLabel(inThresholdLabel);
            addAndMakeVisible(inThresholdText);
            initLabel(inThresholdText);
            inThresholdText.setJustificationType(juce::Justification::centredRight);

            //controlMidiChannel
            addAndMakeVisible(controlMidiChannelSelector);
            initChannels(controlMidiChannelSelector, owner.model.midiChannelReference);
            controlMidiChannelSelector.onChange = [this] {
                owner.model.midiChannelReference = controlMidiChannelSelector.getText().getIntValue();
            };

            //performanceMidiChannel
            addAndMakeVisible(performanceMidiChannelSelector);
            initChannels(performanceMidiChannelSelector, owner.model.midiChannelPerformance);
            performanceMidiChannelSelector.onChange = [this] {
                owner.model.midiChannelPerformance = performanceMidiChannelSelector.getText().getIntValue();
            };

            //thresholdMidiChannel
            addAndMakeVisible(thresholdSelector);
            thresholdSelector.addItem(std::to_string(100), 1);
            thresholdSelector.addItem(std::to_string(200), 2);
            thresholdSelector.addItem(std::to_string(500), 3);
            thresholdSelector.addItem(std::to_string(1000), 4);
            thresholdSelector.setSelectedId(2);
            thresholdSelector.onChange = [this] {
                owner.model.threshold = thresholdSelector.getText().getIntValue();
            };

            addAndMakeVisible(percentageButton);

            percentageButton.addListener(this);

            controlMidiChannelSelector.setHelpText("Reference MIDI Channel");
            performanceMidiChannelSelector.setHelpText("Performance MIDI Channel");
            thresholdSelector.setHelpText("Threshold (ms)");

            startTimer(1000);
        }

        void paint (Graphics& g) override
        {
            g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        }

        void resized() override
        {
            controlMidiChannelSelector.setBounds     ( 1 * s,  1 * s,  5 * s, 1 * s);
            performanceMidiChannelSelector.setBounds ( 7 * s,  1 * s,  5 * s, 1 * s);
            thresholdSelector.setBounds              (13 * s,  1 * s,  5 * s, 1 * s);


            lastUsedMidiChannelLabel.setBounds(1 * s, 3 * s, 5 * s, 1 * s);
            lastUsedMidiChannelText.setBounds(7 * s, 3 * s, 11 * s, 1 * s);

            performanceLabel.setBounds(1 * s, 5 * s, 5 * s, 1 * s);
            performanceText.setBounds(7 * s, 5 * s, 11 * s, 1 * s);

            inThresholdLabel.setBounds(1 * s, 7 * s, 5 * s, 1 * s);
            inThresholdText.setBounds(7 * s, 7 * s, 11 * s, 1 * s);


            percentageButton.setBounds               (1 * s, 9 * s, 17 * s, 1 * s);
        }

        void timerCallback() override
        {
            setData(owner.model.calculateResult());
        }
    private:
        void valueChanged (Value&) override
        {
            setData(owner.model.calculateResult());
        }

        MidiDiffPluginProcessor& owner;
        int s = 25;
    };

    template <typename Element>
    void process (AudioBuffer<Element>& audio, MidiBuffer& midi)
    {
        audio.clear();

        long epoch = long(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
        auto rate = getSampleRate();

        for (const auto midiMessage : midi) {
            auto message = midiMessage.getMessage();
            auto isNoteOn = message.isNoteOn();
            auto channel = message.getChannel();
            auto isReferenceChannel = channel == model.midiChannelReference;
            auto isPerformanceChannel = channel == model.midiChannelPerformance;
            model.lastUsedMidiChannel = channel;

            if (isNoteOn && (isReferenceChannel || isPerformanceChannel))
            {
                long messageTimestamp = toLong(message.getTimeStamp());
                int noteNumber = message.getNoteNumber();
                long messageTimestampSec = toLong(messageTimestamp / rate);
                long midiEventTimestamp = currentBufferEventTimeStartEpochMillis + (messageTimestampSec / 1000);

                if (isReferenceChannel) {
                    model.controlMidiEvents.push_back(make_tuple(midiEventTimestamp, noteNumber));
                }
                else if (isPerformanceChannel) {
                    model.performanceMidiEvents.push_back(make_tuple(midiEventTimestamp, noteNumber));
                }
            }
        }

        long end = long(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
        currentBufferEventTimeStartEpochMillis = epoch;
    }

    static BusesProperties getBusesLayout()
    {
        // Live and Cakewalk don't like to load midi-only plugins, so we add an audio output there.
        const PluginHostType host;
        return host.isAbletonLive() || host.isSonar()
             ? BusesProperties().withOutput ("out", AudioChannelSet::stereo())
             : BusesProperties();
    }

    ValueTree state { "state" };
    MidiDiffModel model;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiDiffPluginProcessor)
};
