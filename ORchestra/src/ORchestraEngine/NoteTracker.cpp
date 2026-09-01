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

#include "NoteTracker.h"

namespace ORchestra {

    NoteTracker::NoteTracker()
        : mNextNoteId(1)
    {
    }

    void NoteTracker::PostNoteOn(DataUnit pitch, DataUnit channel, int onTime, int offTime, DataUnit velocity)
    {
        for (size_t i = 0; i < mActiveNotes.size(); ++i)
        {
            Note& oldNote = mActiveNotes[i];
            if (oldNote.pitch != pitch || oldNote.channel != channel)
                continue;

            if (onTime <= oldNote.offTime)
                oldNote.offTime = std::min(oldNote.offTime, onTime - 1);
        }

        const uint32_t newId = mNextNoteId++;
        const int safeOffTime = (offTime > onTime) ? offTime : onTime + 1;
        mActiveNotes.push_back({ false, newId, pitch, channel, onTime, safeOffTime, velocity });
    }

    void NoteTracker::PostNoteOff(DataUnit pitch, DataUnit channel)
    {
        for (size_t i = 0; i < mActiveNotes.size(); ++i)
        {
            Note& note = mActiveNotes[i];
            if (note.pitch != pitch || note.channel != channel)
                continue;

            if (note.isOn)
                note.offTime = 0;
            else
                mActiveNotes.erase(mActiveNotes.begin() + static_cast<std::ptrdiff_t>(i));
        }
    }

    void NoteTracker::PostCC(DataUnit first, DataUnit second, DataUnit channel, int time)
    {
        mPendingEvents.push_back({ Event::CC, time, first, second, channel, 0 });
    }

    void NoteTracker::Process(int bufferLength, std::vector<Event>& outEvents)
    {
        outEvents.clear();

        for (auto it = mActiveNotes.begin(); it != mActiveNotes.end();)
        {
            Note& note = *it;
            note.onTime -= bufferLength;
            note.offTime -= bufferLength;

            if (note.onTime <= 0 && !note.isOn)
            {
                outEvents.push_back({ Event::NoteOn, note.onTime, note.pitch, note.velocity, note.channel, note.id });
                note.isOn = true;
            }

            if (note.offTime <= 0 && note.isOn)
            {
                outEvents.push_back({ Event::NoteOff, note.offTime, note.pitch, 0, note.channel, note.id });
                it = mActiveNotes.erase(it);
            }
            else
            {
                ++it;
            }
        }

        for (auto it = mPendingEvents.begin(); it != mPendingEvents.end();)
        {
            it->time -= bufferLength;
            if (it->time <= 0)
            {
                outEvents.push_back(*it);
                it = mPendingEvents.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void NoteTracker::Clear(std::vector<Event>& outEvents)
    {
        outEvents.clear();

        for (const Note& note : mActiveNotes)
        {
            if (note.isOn)
            {
                outEvents.push_back({ Event::NoteOff, 0, note.pitch, 0, note.channel, note.id });
            }
        }

        mActiveNotes.clear();
        mPendingEvents.clear();
        mNextNoteId = 1;
    }

} // namespace ORchestra
