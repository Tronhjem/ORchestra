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

#include "catch.hpp"
#include "NoteTracker.h"

using namespace ORchestra;

TEST_CASE("NoteTracker: single note emits note-on then note-off", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 100, 100);

    tracker.Process(50, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].second == 100);
    REQUIRE(events[0].channel == 1);

    tracker.Process(100, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].channel == 1);
}

TEST_CASE("NoteTracker: retrigger before note-on replaces old note", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 100, 110);
    tracker.PostNoteOn(60, 1, 50, 150, 100);

    tracker.Process(100, events);
    REQUIRE(events.size() == 3);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].second == 110);

    REQUIRE(events[1].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[1].time == -51);

    REQUIRE(events[2].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[2].time == -50);
}

TEST_CASE("NoteTracker: retrigger while sounding emits cancellation note-off", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 100, 100);

    tracker.Process(50, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);

    tracker.PostNoteOn(60, 1, 50, 150, 110);

    tracker.Process(100, events);
    REQUIRE(events.size() == 2);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].time == -51); // 50 - 1 - 100
    REQUIRE(events[1].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[1].first == 60);
    REQUIRE(events[1].second == 110);

    tracker.Process(100, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
}

TEST_CASE("NoteTracker: different pitch/channel pairs are independent", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 200, 100);
    tracker.PostNoteOn(64, 1, 0, 200, 100);
    tracker.PostNoteOn(60, 2, 0, 200, 100);

    tracker.Process(50, events);
    REQUIRE(events.size() == 3);
    for (const auto& event : events)
    {
        REQUIRE(event.type == NoteTracker::Event::NoteOn);
    }
}

TEST_CASE("NoteTracker: CC event is emitted", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostCC(74, 64, 1, 25);

    tracker.Process(50, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::CC);
    REQUIRE(events[0].first == 74);
    REQUIRE(events[0].second == 64);
    REQUIRE(events[0].channel == 1);
    REQUIRE(events[0].time == -25);
}

TEST_CASE("NoteTracker: retrigger at sample 0 still emits note-off before note-on", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 100, 100);
    tracker.Process(50, events); // NoteOn

    tracker.PostNoteOn(60, 1, 0, 200, 110);

    tracker.Process(100, events);
    REQUIRE(events.size() == 2);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff); // cancellation at sample 0
    REQUIRE(events[0].first == 60);
    REQUIRE(events[1].type == NoteTracker::Event::NoteOn);  // new note at sample 0
    REQUIRE(events[1].second == 110);
}

TEST_CASE("NoteTracker: channel 16 and pitch 127 are within bounds", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(127, 16, 0, 100, 100);

    tracker.Process(50, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[0].first == 127);
    REQUIRE(events[0].channel == 16);
}

TEST_CASE("NoteTracker: clear emits note-offs for sounding notes", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 100, 100);
    tracker.Process(50, events);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);

    tracker.Clear(events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].channel == 1);
}

TEST_CASE("NoteTracker: short note emits note-on and note-off in same buffer", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 50, 100);

    tracker.Process(100, events);
    REQUIRE(events.size() == 2);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[0].time == -100);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[1].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[1].time == -50);
    REQUIRE(events[1].first == 60);
}

TEST_CASE("NoteTracker: overlapping retrigger cancels old note one sample before new note", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 100, 100);
    tracker.Process(50, events); // old note now sounding, offTime == 50
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);

    tracker.PostNoteOn(60, 1, 40, 240, 110); // new note overlaps old

    tracker.Process(100, events);
    REQUIRE(events.size() == 2);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff); // cancellation at 40 - 1
    REQUIRE(events[0].time == -61);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[1].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[1].time == -60);
    REQUIRE(events[1].first == 60);
    REQUIRE(events[1].second == 110);
}

TEST_CASE("NoteTracker: non-overlapping retrigger preserves old note-off without cancellation", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 100, 100);
    tracker.Process(50, events); // old note now sounding, offTime == 50
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);

    tracker.PostNoteOn(60, 1, 60, 260, 110); // new note starts after old ends

    tracker.Process(100, events);
    REQUIRE(events.size() == 2);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff); // old note ends naturally
    REQUIRE(events[0].time == -50);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[1].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[1].time == -40);
    REQUIRE(events[1].first == 60);
    REQUIRE(events[1].second == 110);
}

TEST_CASE("NoteTracker: explicit note-off stops a sounding note", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 100, 100);
    tracker.Process(50, events);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);

    tracker.PostNoteOff(60, 1);

    tracker.Process(50, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].channel == 1);
}

TEST_CASE("NoteTracker: sub-step retrigger plays both notes", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.PostNoteOn(60, 1, 0, 40, 100);
    tracker.PostNoteOn(60, 1, 50, 90, 110);

    tracker.Process(100, events);
    REQUIRE(events.size() == 4);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[0].time == -100);
    REQUIRE(events[0].second == 100);
    REQUIRE(events[1].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[1].time == -60);
    REQUIRE(events[2].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[2].time == -50);
    REQUIRE(events[2].second == 110);
    REQUIRE(events[3].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[3].time == -10);
}
