#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <cmath>
using namespace std;

typedef vector< tuple<long, int> > EventListType;

class MidiDiff
{
private:
    int differenceOfSameNotes(long controlTime, int controlMidiNote, EventListType currentMidiEvents, int threshold) {
        int minDistance = threshold;
        for (const tuple<long, int> midiEvt : currentMidiEvents) {
            long eventTime = std::get<0>(midiEvt);
            int midiNote = std::get<1>(midiEvt);
            int currentDistance = abs(controlTime - eventTime);
            if (controlMidiNote == midiNote && currentDistance < minDistance ) {
                minDistance = currentDistance;
            }
        }
        return minDistance;
    };


public:
    MidiDiff();
    ~MidiDiff();

    EventListType controlMidiEvents;
    EventListType performanceMidiEvents;

    double percentageOfPerformance(int thresholdMs){
        auto control = controlMidiEvents;
        auto perform = performanceMidiEvents;

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
};

