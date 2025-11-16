#pragma once

#include <JuceHeader.h>
#include <vector>

#include "StepData.h"
#include "Defines.h"

namespace ORchestra {

    struct ScheduledMidiMessage
    {
        MidiType mMessageType;
        DataUnit mFirstByte;
        DataUnit mSecondByte;
        DataUnit mChannel;
        int mScheduledTime;
        int mDuration;
    };

    class MidiScheduler
    {
    public:
        MidiScheduler();
        void PostMidi(ScheduledMidiMessage& message);
        void ProcessMidiPosts(juce::MidiBuffer& midiMessages, const int bufferLength, const int64_t endOfBufferPosition);
        void ClearAllData(juce::MidiBuffer& midiMessages);

    private:
        std::vector<ScheduledMidiMessage> mScheduledMidiMessages;
    };


} // namespace ORchestra
