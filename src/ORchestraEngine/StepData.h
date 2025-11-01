#pragma once
#include "Types.h"

class SequenceStep
{
public:
    SequenceStep(MidiType type, StepData shouldTrigger, const StepData firstData, const StepData secondData, const StepData channel, const int duration) :
        mType(type),
        mShouldTrigger(shouldTrigger),
        mFirst(firstData),
        mSecond(secondData),
        mChannel(channel),
        mDuration(duration)
    {
    }
    
    ~SequenceStep() {}
    
    MidiType mType;
    StepData mShouldTrigger;
    StepData mFirst;
    StepData mSecond;
    StepData mChannel;
    int mDuration;
    
private:
    SequenceStep() = delete;
};
