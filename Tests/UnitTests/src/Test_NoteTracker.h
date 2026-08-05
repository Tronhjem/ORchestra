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

    tracker.postNoteOn(60, 1, 0, 100, 100);

    tracker.process(50, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].second == 100);
    REQUIRE(events[0].channel == 1);

    tracker.process(100, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].channel == 1);
}

TEST_CASE("NoteTracker: retrigger before note-on replaces old note", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.postNoteOn(60, 1, 0, 100, 100);
    tracker.postNoteOn(60, 1, 50, 150, 110);

    tracker.process(100, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].second == 110);

    tracker.process(100, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
}

TEST_CASE("NoteTracker: retrigger while sounding emits cancellation note-off", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.postNoteOn(60, 1, 0, 100, 100);

    tracker.process(50, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);

    tracker.postNoteOn(60, 1, 50, 150, 110);

    tracker.process(100, events);
    REQUIRE(events.size() == 2);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].time == -51); // 50 - 1 - 100
    REQUIRE(events[1].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[1].first == 60);
    REQUIRE(events[1].second == 110);

    tracker.process(100, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
}

TEST_CASE("NoteTracker: different pitch/channel pairs are independent", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.postNoteOn(60, 1, 0, 200, 100);
    tracker.postNoteOn(64, 1, 0, 200, 100);
    tracker.postNoteOn(60, 2, 0, 200, 100);

    tracker.process(50, events);
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

    tracker.postCC(74, 64, 1, 25);

    tracker.process(50, events);
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

    tracker.postNoteOn(60, 1, 0, 100, 100);
    tracker.process(50, events); // NoteOn

    tracker.postNoteOn(60, 1, 0, 100, 110);

    tracker.process(100, events);
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

    tracker.postNoteOn(127, 16, 0, 100, 100);

    tracker.process(50, events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);
    REQUIRE(events[0].first == 127);
    REQUIRE(events[0].channel == 16);
}

TEST_CASE("NoteTracker: clear emits note-offs for sounding notes", "[NoteTracker]")
{
    NoteTracker tracker;
    std::vector<NoteTracker::Event> events;

    tracker.postNoteOn(60, 1, 0, 100, 100);
    tracker.process(50, events);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOn);

    tracker.clear(events);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == NoteTracker::Event::NoteOff);
    REQUIRE(events[0].first == 60);
    REQUIRE(events[0].channel == 1);
}
