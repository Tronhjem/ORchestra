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

#include <vector>
#include <cstdint>
#include <algorithm>
#include "Defines.h"

namespace ORchestra {

    class NoteTracker
    {
    public:
        struct Note
        {
            bool isActive = false;
            bool isOn = false;
            uint32_t id = 0;
            int onTime = 0;
            int offTime = 0;
            DataUnit velocity = 0;
        };

        struct Event
        {
            enum Type { NoteOn, NoteOff, CC } type;
            int time = 0;
            DataUnit first = 0;
            DataUnit second = 0;
            DataUnit channel = 0;
            uint32_t noteId = 0;
        };

        NoteTracker();

        void postNoteOn(DataUnit pitch, DataUnit channel, int onTime, int offTime, DataUnit velocity);
        void postNoteOff(DataUnit pitch, DataUnit channel);
        void postCC(DataUnit first, DataUnit second, DataUnit channel, int time);

        void process(int bufferLength, std::vector<Event>& outEvents);
        void clear(std::vector<Event>& outEvents);

        const Note& getNote(DataUnit pitch, DataUnit channel) const;

    private:
        static int getIndex(DataUnit pitch, DataUnit channel);
        static void removeActiveIndex(std::vector<int>& indices, int index);

        Note mActiveNotes[128 * 16];
        std::vector<int> mActiveIndices;
        std::vector<Event> mPendingEvents;
        uint32_t mNextNoteId;
    };

} // namespace ORchestra
