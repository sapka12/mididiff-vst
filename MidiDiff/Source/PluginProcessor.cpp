/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <chrono>

//==============================================================================
MidiDiffAudioProcessor::MidiDiffAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    m_flogger = std::unique_ptr<juce::FileLogger>(juce::FileLogger::createDateStampedLogger("MidiDiffPlugin", "mididiff-log", ".txt", "Example message"));
}

MidiDiffAudioProcessor::~MidiDiffAudioProcessor()
{
}

//==============================================================================
const juce::String MidiDiffAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MidiDiffAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MidiDiffAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MidiDiffAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MidiDiffAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MidiDiffAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MidiDiffAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MidiDiffAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MidiDiffAudioProcessor::getProgramName (int index)
{
    return {};
}

void MidiDiffAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MidiDiffAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MidiDiffAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidiDiffAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

long toLong(double d) {
    return long(round(1000 * d));
}

void MidiDiffAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    using namespace std::chrono;
    long epoch = long(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());

    for (const auto midiMessage : midiMessages) {
        auto message = midiMessage.getMessage();
        auto messageTimestamp = toLong(message.getTimeStamp());

        auto isNoteOn = message.isNoteOn();
        auto isReferenceChannel = message.getChannel() == midiChannelReference;
        auto isPerformanceChannel = message.getChannel() == midiChannelPerformance;

        if (isNoteOn && (isReferenceChannel || isPerformanceChannel))
        {
            midiNoteCounter++;
            auto noteNumber = message.getNoteNumber();

            auto currentTimestamp = epoch + (messageTimestamp/1000);

            juce::MessageManager::callAsync([=]()
                {
                    updateMidiNotesLabel(noteNumber);
                });
            


            if (isReferenceChannel) {
                midiDiff.controlMidiEvents.push_back(make_tuple(currentTimestamp, noteNumber));
                //log("note [cont " + juce::String(epoch) + " " + juce::String(messageTimestamp) + "]: " + juce::String(noteNumber));
            }
            else if (isPerformanceChannel) {
                //log("note [pref " + juce::String(epoch) + " " + juce::String(messageTimestamp) + "]: " + juce::String(noteNumber));
                midiDiff.performanceMidiEvents.push_back(make_tuple(currentTimestamp, noteNumber));
            }
        }
    }

    lastBufferStartEpochMillis = epoch;
}

//==============================================================================
bool MidiDiffAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MidiDiffAudioProcessor::createEditor()
{
    return new MidiDiffAudioProcessorEditor (*this);
}

//==============================================================================
void MidiDiffAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MidiDiffAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiDiffAudioProcessor();
}
