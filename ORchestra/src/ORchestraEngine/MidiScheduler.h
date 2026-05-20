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

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "Defines.h"

namespace ORchestra {

    struct ScheduledMidiMessage
    {
        SequenceStepType mMessageType;
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
        int mActiveNoteCounts[128 * 16] = {};
    };

} // namespace ORchestra
