#include "MidiScheduler.h"
#include <algorithm>

#define CLAMP_TO_UCHAR(x, min, max) \
    static_cast<unsigned char>(std::clamp(static_cast<int>(x), min, max))

MidiScheduler::MidiScheduler()
{
}

void MidiScheduler::PostMidi(ScheduledMidiMessage& message)
{
    message.mFirstByte = CLAMP_TO_UCHAR(message.mFirstByte, 0, 127);
    message.mSecondByte = CLAMP_TO_UCHAR(message.mSecondByte, 0, 127);
    message.mChannel = CLAMP_TO_UCHAR(message.mChannel, 0, 16);
    mScheduledMidiMessages.emplace_back(message);
    
    if(message.mMessageType == MidiType::NoteOn)
    {
        const int timeStampOff = message.mScheduledTime + message.mDuration;
        ScheduledMidiMessage messageOff  {MidiType::NoteOff, message.mFirstByte, 0, message.mChannel, timeStampOff, 0};
        mScheduledMidiMessages.emplace_back(messageOff);
    }
}

void MidiScheduler::ProcessMidiPosts(juce::MidiBuffer& midiMessages,
                                     const int bufferLength,
                                     const int64_t endOfBufferPosition)
{
    for(int i = (int)mScheduledMidiMessages.size() - 1; i >= 0; --i)
    {
        const ScheduledMidiMessage& message = mScheduledMidiMessages[i];

        if (message.mScheduledTime <= endOfBufferPosition)
        {
            const int relativePositionInBuffer = static_cast<int>(message.mScheduledTime - (endOfBufferPosition - bufferLength));
            
            switch (message.mMessageType)
            {
                case MidiType::NoteOn:
                    midiMessages.addEvent(juce::MidiMessage::noteOn(message.mChannel, message.mFirstByte, message.mSecondByte), relativePositionInBuffer);
                    break;
                case MidiType::NoteOff:
                    midiMessages.addEvent(juce::MidiMessage::noteOff(message.mChannel, message.mFirstByte, message.mSecondByte), relativePositionInBuffer);
                    break;
                case MidiType::CC:
                    midiMessages.addEvent(juce::MidiMessage::controllerEvent(message.mChannel, message.mFirstByte, message.mSecondByte), relativePositionInBuffer);
                    break;
                default:
                    break;
            }

            mScheduledMidiMessages[i] = mScheduledMidiMessages.back();
            mScheduledMidiMessages.pop_back();
        }
    }
}

void MidiScheduler::ClearAllData(juce::MidiBuffer& midiMessages)
{
    for(const ScheduledMidiMessage& message : mScheduledMidiMessages)
    {
        if (message.mMessageType == MidiType::NoteOn || message.mMessageType == MidiType::NoteOff)
            midiMessages.addEvent(juce::MidiMessage::noteOff(message.mChannel, message.mFirstByte, static_cast<uint8_t>(0)), 0);
    }

    mScheduledMidiMessages.clear();
}
