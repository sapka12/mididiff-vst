# MidiDiff

this project is created by [JUCE](https://juce.com/), based on the [MidiLoggerPluginDemo](https://github.com/juce-framework/JUCE/blob/master/examples/Plugins/MidiLoggerPluginDemo.h)

## About
This VST plugin calculates the differences of two midi channels and provides a result in percentage. 

## Download
- [wiki/Download](https://github.com/sapka12/mididiff-vst/wiki/Download)

## Usage
### Cakewalk
- Copy MidiDiff.vst into your VST folder
- Insert Audio Track
- Insert Audio FX: arnfarkas/MidiDiff
- On the plugin's UI, under VST3: check Enable MIDI Input
- Insert MIDI Track and set its output to MidiDiff
- Record/quantize or copy a reference MIDI
  - Optionally: change all the reference MIDI events to a specific channel (you can use Cakewalk's Event Inspector)
- Play the same notes on another MIDI channel
- MidiDiff's UI shows the accurance of the performance in percentage
- Reset on click the percentage

### Reaper
- Copy MidiDiff.vst into your VST folder
- Insert virtual instrument on new track
- add FX: arnfarkas/MidiDiff
- Record/quantize or copy a reference MIDI
  - Optionally: change all the reference MIDI events to a specific channel (select all, right click, event properties, change channel)
- Play the same notes on another MIDI channel
- MidiDiff's UI shows the accurance of the performance in percentage
- Reset on click the percentage

## Score Calculating
For each "onNote" reference MIDI event finds timely the closest MIDI event on the performance channel with the same note. The maximum of the difference will be the threshold given by the UI. The percentage is calculated based on the average difference inside the threshold.

## UI Elements
### Last Used Channel
helps to find the source midi inputs for the control/reference and the performance
### Control MIDI Channel
channel of the reference MIDI notes
### Performance MIDI Channel
channel of the performance MIDI notes
### Threshold
the algorithm is looking for a match for each reference MIDI note inside this timeframe
### Percentage Button
displays the result score in percentage (it can be reset on click)
