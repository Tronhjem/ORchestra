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

    void NoteTracker::postNoteOn(DataUnit pitch, DataUnit channel, int onTime, int offTime, DataUnit velocity)
    {
        const int index = getIndex(pitch, channel);
        Note& oldNote = mActiveNotes[index];

        const bool hadOldNote = oldNote.isActive;
        const bool oldWasOn = oldNote.isOn;
        const uint32_t oldId = oldNote.id;

        if (oldNote.isActive && oldNote.isOn)
        {
            const int cancelTime = std::max(0, onTime - 1);
            mPendingEvents.push_back({ Event::NoteOff, cancelTime, pitch, 0, channel, oldId });
        }

        const uint32_t newId = mNextNoteId++;
        const int safeOffTime = (offTime > onTime) ? offTime : onTime + 1;
        oldNote = { true, false, newId, onTime, safeOffTime, velocity };

        if (!hadOldNote)
        {
            mActiveIndices.push_back(index);
        }
    }

    void NoteTracker::postNoteOff(DataUnit pitch, DataUnit channel)
    {
        const int index = getIndex(pitch, channel);
        Note& note = mActiveNotes[index];

        if (note.isActive && note.isOn)
        {
            mPendingEvents.push_back({ Event::NoteOff, 0, pitch, 0, channel, note.id });
        }

        if (note.isActive)
        {
            note = {};
            removeActiveIndex(mActiveIndices, index);
        }
    }

    void NoteTracker::postCC(DataUnit first, DataUnit second, DataUnit channel, int time)
    {
        mPendingEvents.push_back({ Event::CC, time, first, second, channel, 0 });
    }

    void NoteTracker::process(int bufferLength, std::vector<Event>& outEvents)
    {
        outEvents.clear();

        for (const int index : mActiveIndices)
        {
            Note& note = mActiveNotes[index];
            if (!note.isActive)
                continue;

            note.onTime -= bufferLength;
            note.offTime -= bufferLength;

            const int pitch = index / 16;
            const int channel = (index % 16) + 1;

            if (note.onTime <= 0 && !note.isOn)
            {
                outEvents.push_back({ Event::NoteOn, note.onTime, static_cast<DataUnit>(pitch), note.velocity, static_cast<DataUnit>(channel), note.id });
            }

            if (note.offTime <= 0 && note.isOn)
            {
                outEvents.push_back({ Event::NoteOff, note.offTime, static_cast<DataUnit>(pitch), 0, static_cast<DataUnit>(channel), note.id });
            }
        }

        for (auto& event : mPendingEvents)
        {
            event.time -= bufferLength;
            if (event.time <= 0)
            {
                outEvents.push_back(event);
            }
        }

        std::stable_sort(outEvents.begin(), outEvents.end(),
            [](const Event& a, const Event& b) {
                if (a.time != b.time)
                    return a.time < b.time;

                const int priorityA = (a.type == Event::NoteOff) ? 0 : (a.type == Event::NoteOn) ? 1 : 2;
                const int priorityB = (b.type == Event::NoteOff) ? 0 : (b.type == Event::NoteOn) ? 1 : 2;
                return priorityA < priorityB;
            });

        for (const auto& event : outEvents)
        {
            const int index = getIndex(event.first, event.channel);
            Note& note = mActiveNotes[index];

            if (event.type == Event::NoteOn)
            {
                if (note.isActive && note.id == event.noteId)
                {
                    note.isOn = true;
                }
            }
            else if (event.type == Event::NoteOff)
            {
                if (note.isActive && note.id == event.noteId)
                {
                    note = {};
                    removeActiveIndex(mActiveIndices, index);
                }
            }
        }

        mPendingEvents.erase(
            std::remove_if(mPendingEvents.begin(), mPendingEvents.end(),
                [](const Event& e) { return e.time <= 0; }),
            mPendingEvents.end());
    }

    void NoteTracker::clear(std::vector<Event>& outEvents)
    {
        outEvents.clear();

        for (const int index : mActiveIndices)
        {
            Note& note = mActiveNotes[index];
            if (note.isOn)
            {
                const int pitch = index / 16;
                const int channel = (index % 16) + 1;
                outEvents.push_back({ Event::NoteOff, 0, static_cast<DataUnit>(pitch), 0, static_cast<DataUnit>(channel), note.id });
            }
            note = {};
        }

        mActiveIndices.clear();
        mPendingEvents.clear();
        mNextNoteId = 1;
    }

    const NoteTracker::Note& NoteTracker::getNote(DataUnit pitch, DataUnit channel) const
    {
        return mActiveNotes[getIndex(pitch, channel)];
    }

    int NoteTracker::getIndex(DataUnit pitch, DataUnit channel)
    {
        return static_cast<int>(pitch) * 16 + static_cast<int>(channel) - 1;
    }

    void NoteTracker::removeActiveIndex(std::vector<int>& indices, int index)
    {
        indices.erase(std::remove(indices.begin(), indices.end(), index), indices.end());
    }

} // namespace ORchestra
