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
        message.mChannel = CLAMP_TO_MIDI(message.mChannel, 1, MAX_MIDI_CHANNEL_NUMBER);

        switch (message.mMessageType)
        {
            case SequenceStepType::NoteOn:
            {
                const int offTime = message.mScheduledTime + message.mDuration;
                mNoteTracker.PostNoteOn(message.mFirst, message.mChannel,
                    message.mScheduledTime, offTime, message.mSecond);
                break;
            }

            case SequenceStepType::NoteOff:
            {
                mNoteTracker.PostNoteOff(message.mFirst, message.mChannel);
                break;
            }

            case SequenceStepType::CC:
            {
                mNoteTracker.PostCC(message.mFirst, message.mSecond, message.mChannel,
                    message.mScheduledTime);
                break;
            }

            default:
                break;
        }
    }

    void MidiScheduler::ProcessMidiPosts(juce::MidiBuffer& midiMessages,
        const int bufferLength)
    {
        mNoteTracker.Process(bufferLength, mEventBuffer);

        for (const auto& event : mEventBuffer)
        {
            const int relativePosition = juce::jmax(0, bufferLength + event.time);

            switch (event.type)
            {
                case NoteTracker::Event::NoteOn:
                {
                    midiMessages.addEvent(juce::MidiMessage::noteOn(static_cast<int>(event.channel),
                        static_cast<int>(event.first),
                        static_cast<unsigned char>(event.second)), relativePosition);
                    break;
                }

                case NoteTracker::Event::NoteOff:
                {
                    midiMessages.addEvent(juce::MidiMessage::noteOff(static_cast<int>(event.channel),
                        static_cast<int>(event.first),
                        static_cast<unsigned char>(event.second)), relativePosition);
                    break;
                }

                case NoteTracker::Event::CC:
                {
                    midiMessages.addEvent(juce::MidiMessage::controllerEvent(static_cast<int>(event.channel),
                        static_cast<int>(event.first),
                        static_cast<unsigned char>(event.second)), relativePosition);
                    break;
                }

                default:
                    break;
            }
        }
    }

    void MidiScheduler::ClearAllData(juce::MidiBuffer& midiMessages)
    {
        mNoteTracker.Clear(mEventBuffer);

        for (const auto& event : mEventBuffer)
        {
            midiMessages.addEvent(juce::MidiMessage::noteOff(static_cast<int>(event.channel),
                static_cast<int>(event.first),
                static_cast<unsigned char>(event.second)), 0);
        }
    }
} // namespace ORchestra
