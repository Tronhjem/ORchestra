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
        message.mFirstByte = CLAMP_TO_MIDI(message.mFirstByte, 0, MIDI_MAX_VALUE);
        message.mSecondByte = CLAMP_TO_MIDI(message.mSecondByte, 0, MIDI_MAX_VALUE);
        message.mChannel = CLAMP_TO_MIDI(message.mChannel, 0, MAX_MIDI_CHANNEL_NUMBER);
        mScheduledMidiMessages.emplace_back(message);

        // Generate corresponding NoteOff for NoteOn
        if (message.mMessageType == SequenceStepType::NoteOn)
        {
            const int timeStampOff = message.mScheduledTime + message.mDuration;
            mScheduledMidiMessages.emplace_back(ScheduledMidiMessage { SequenceStepType::NoteOff, 
                                                                       message.mFirstByte, 
                                                                       0, message.mChannel, 
                                                                       timeStampOff, 0 });
        }
    }

    void MidiScheduler::ProcessMidiPosts(juce::MidiBuffer& midiMessages,
        const int bufferLength)
    {
        for (int i = (int)mScheduledMidiMessages.size() - 1; i >= 0; --i)
        {
            ScheduledMidiMessage& message = mScheduledMidiMessages[static_cast<unsigned long>(i)];
            message.mScheduledTime -= bufferLength;

            if (message.mScheduledTime <= 0)
            {
                const int relativePositionInBuffer = juce::jmax(0, bufferLength + message.mScheduledTime);

                switch (message.mMessageType)
                {
                case SequenceStepType::NoteOn:
                {
                    const int index = static_cast<int>(message.mFirstByte) * 16 + static_cast<int>(message.mChannel);

                    // Send note off before note on for same note.
                    if (mActiveNoteCounts[index] > 0)
                    {
                        midiMessages.addEvent(juce::MidiMessage::noteOff(static_cast<int>(message.mChannel),
                            static_cast<int>(message.mFirstByte), static_cast<uint8_t>(0)),
                            juce::jmax(0, relativePositionInBuffer - 1));
                    }

                    midiMessages.addEvent(juce::MidiMessage::noteOn(static_cast<int>(message.mChannel),
                                            static_cast<int>(message.mFirstByte),
                                            static_cast<unsigned char>(message.mSecondByte)), relativePositionInBuffer);

                    mActiveNoteCounts[index] = 1;

                    break;
                }

                case SequenceStepType::NoteOff:
                {
                    const int index = static_cast<int>(message.mFirstByte) * 16 + static_cast<int>(message.mChannel);

                    midiMessages.addEvent(juce::MidiMessage::noteOff(static_cast<int>(message.mChannel),
                            static_cast<int>(message.mFirstByte),
                            static_cast<unsigned char>(message.mSecondByte)), relativePositionInBuffer);

                    mActiveNoteCounts[index] = 0;

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
