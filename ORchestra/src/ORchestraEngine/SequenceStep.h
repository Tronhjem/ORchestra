#pragma once
#include "Types.h"
#include "StepData.h"

class SequenceStep
{
public:
    SequenceStep(MidiType midiType,
                 StepData shouldTrigger,
                 const StepData firstData,
                 const StepData secondData,
                 const StepData channel,
                 const int duration) : mType(midiType),
                                       mShouldTrigger(shouldTrigger),
                                       mFirst(firstData),
                                       mSecond(secondData),
                                       mChannel(channel),
                                       mDuration(duration)
    {
    }

    MidiType mType;
    StepData mShouldTrigger;
    StepData mFirst;
    StepData mSecond;
    StepData mChannel;
    int mDuration;

private:
    SequenceStep() = delete;
};
