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
#include "Defines.h"
#include <algorithm>

namespace ORchestra {

    #define CLAMP_TO_MIDI(x, min, max) \
    static_cast<DataUnit>(std::clamp(static_cast<int>(x), min, max))

    MidiScheduler::MidiScheduler()
    {
    }

    void MidiScheduler::PostMidi(ScheduledMidiMessage& message)
    {
        message.mFirst = CLAMP_TO_MIDI(message.mFirst, 0, MIDI_MAX_VALUE);
        message.mSecond = CLAMP_TO_MIDI(message.mSecond, 0, MIDI_MAX_VALUE);
        message.mChannel = CLAMP_TO_MIDI(message.mChannel, 0, MAX_MIDI_CHANNEL_NUMBER);
        mScheduledMidiMessages.emplace_back(message);

        // Generate corresponding NoteOff for NoteOn
        if (message.mMessageType == SequenceStepType::NoteOn)
        {
            const int timeStampOff = message.mScheduledTime + message.mDuration;
            mScheduledMidiMessages.emplace_back(ScheduledMidiMessage { SequenceStepType::NoteOff, 
                                                                       message.mFirst, 
                                                                       0, message.mChannel, 
                                                                       timeStampOff, 0 });
        }
    }

    void MidiScheduler::ProcessMidiPosts(juce::MidiBuffer& midiMessages,
        const int bufferLength)
    {
        // Process in insertion order so NoteOn/NoteOff pairs for the same pitch
        // are handled chronologically. Using a bool count and reverse iteration
        // left the active flag true after two same-pitch notes fired together.
        size_t keepCount = 0;

        for (size_t i = 0; i < mScheduledMidiMessages.size(); ++i)
        {
            ScheduledMidiMessage& message = mScheduledMidiMessages[i];
            message.mScheduledTime -= bufferLength;

            if (message.mScheduledTime <= 0)
            {
                const int relativePositionInBuffer = juce::jmax(0, bufferLength + message.mScheduledTime);

                switch (message.mMessageType)
                {
                    case SequenceStepType::NoteOn:
                    {
                        const int index = static_cast<int>(message.mFirst) * 16 + static_cast<int>(message.mChannel);

                        ++mActiveNoteCounts[index];
                        if (mActiveNoteCounts[index] == 1)
                        {
                            midiMessages.addEvent(juce::MidiMessage::noteOn(static_cast<int>(message.mChannel),
                                                    static_cast<int>(message.mFirst),
                                                    static_cast<unsigned char>(message.mSecond)), relativePositionInBuffer);
                        }

                        break;
                    }

                    case SequenceStepType::NoteOff:
                    {
                        const int index = static_cast<int>(message.mFirst) * 16 + static_cast<int>(message.mChannel);

                        if (mActiveNoteCounts[index] > 0)
                        {
                            --mActiveNoteCounts[index];
                            if (mActiveNoteCounts[index] == 0)
                            {
                                midiMessages.addEvent(juce::MidiMessage::noteOff(static_cast<int>(message.mChannel),
                                        static_cast<int>(message.mFirst),
                                        static_cast<unsigned char>(message.mSecond)), juce::jmax(0, relativePositionInBuffer - 1));
                            }
                        }

                        break;
                    }

                    case SequenceStepType::CC:
                    {
                        midiMessages.addEvent(juce::MidiMessage::controllerEvent(static_cast<int>(message.mChannel),
                            static_cast<int>(message.mFirst),
                            static_cast<unsigned char>(message.mSecond)), relativePositionInBuffer);

                        break;
                    }

                    default:
                        break;
                }
            }
            else
            {
                mScheduledMidiMessages[keepCount] = mScheduledMidiMessages[i];
                ++keepCount;
            }
        }

        mScheduledMidiMessages.erase(mScheduledMidiMessages.begin() + keepCount, mScheduledMidiMessages.end());
    }

    void MidiScheduler::ClearAllData(juce::MidiBuffer& midiMessages)
    {
        for (int pitch = 0; pitch < 128; ++pitch)
        {
            for (int ch = 0; ch < 16; ++ch)
            {
                const int index = pitch * 16 + ch;
                if (mActiveNoteCounts[index] > 0)
                {
                    midiMessages.addEvent(juce::MidiMessage::noteOff(ch, pitch, static_cast<uint8_t>(0)), 0);
                    mActiveNoteCounts[index] = 0;
                }
            }
        }

        mScheduledMidiMessages.clear();
    }
} // namespace ORchestra
