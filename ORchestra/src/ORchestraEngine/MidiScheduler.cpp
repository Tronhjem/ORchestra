/*
 * Copyright (C) 2026 Christian Tronhjem
 *
 * This file is part of ORchestra.
 *
 * ORchestra is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORchestra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ORchestra. If not, see <https://www.gnu.org/licenses/>.
 */

#include "MidiScheduler.h"
#include <algorithm>

namespace ORchestra {

    using juce::MidiMessage;

#define CLAMP_TO_MIDI(x, min, max) \
static_cast<DataUnit>(std::clamp(static_cast<int>(x), min, max))

    MidiScheduler::MidiScheduler()
    {
    }

    void MidiScheduler::PostMidi(ScheduledMidiMessage& message)
    {
        message.mFirstByte = CLAMP_TO_MIDI(message.mFirstByte, DATA_UNIT_MIN_VALUE, DATA_UNIT_MAX_VALUE);
        message.mSecondByte = CLAMP_TO_MIDI(message.mSecondByte, DATA_UNIT_MIN_VALUE, DATA_UNIT_MAX_VALUE);
        message.mChannel = CLAMP_TO_MIDI(message.mChannel, DATA_UNIT_MIN_VALUE, 16);
        mScheduledMidiMessages.emplace_back(message);

        // Generate corresponding NoteOff for NoteOn
        if (message.mMessageType == SequenceStepType::NoteOn)
        {
            const int timeStampOff = message.mScheduledTime + message.mDuration;
            mScheduledMidiMessages.emplace_back(ScheduledMidiMessage { SequenceStepType::NoteOff, 
                                                                       message.mFirstByte, 
                                                                       0, message.mChannel, 
                                                                       timeStampOff, 0 
                                                                     });
        }
    }

    void MidiScheduler::ProcessMidiPosts(juce::MidiBuffer& midiMessages,
        const int bufferLength,
        const int64_t endOfBufferPosition)
    {
        for (int i = (int)mScheduledMidiMessages.size() - 1; i >= 0; --i)
        {
            const ScheduledMidiMessage& message = mScheduledMidiMessages[static_cast<unsigned long>(i)];

            if (message.mScheduledTime <= endOfBufferPosition)
            {
                const int relativePositionInBuffer = static_cast<int>(message.mScheduledTime - (endOfBufferPosition - bufferLength));

                switch (message.mMessageType)
                {
                case SequenceStepType::NoteOn:
                {
                    midiMessages.addEvent(juce::MidiMessage::noteOn(static_cast<int>(message.mChannel),
                        static_cast<int>(message.mFirstByte),
                        static_cast<unsigned char>(message.mSecondByte)), relativePositionInBuffer);
                    break;
                }

                case SequenceStepType::NoteOff:
                {
                    midiMessages.addEvent(juce::MidiMessage::noteOff(static_cast<int>(message.mChannel),
                        static_cast<int>(message.mFirstByte),
                        static_cast<unsigned char>(message.mSecondByte)), 0);
                    break;
                }

                case SequenceStepType::CC:
                {
                    midiMessages.addEvent(juce::MidiMessage::controllerEvent(static_cast<int>(message.mChannel),
                        static_cast<int>(message.mFirstByte),
                        static_cast<unsigned char>(message.mSecondByte)), relativePositionInBuffer);
                    break;
                }

                default:
                    break;
                }

                mScheduledMidiMessages[static_cast<unsigned long>(i)] = mScheduledMidiMessages.back();
                mScheduledMidiMessages.pop_back();
            }
        }
    }

    void MidiScheduler::ClearAllData(juce::MidiBuffer& midiMessages)
    {
        for (const ScheduledMidiMessage& message : mScheduledMidiMessages)
        {
            if (message.mMessageType == SequenceStepType::NoteOn || message.mMessageType == SequenceStepType::NoteOff)
                midiMessages.addEvent(juce::MidiMessage::noteOff(message.mChannel, message.mFirstByte, static_cast<uint8_t>(0)), 0);
        }

        mScheduledMidiMessages.clear();
    }
} // namespace ORchestra
