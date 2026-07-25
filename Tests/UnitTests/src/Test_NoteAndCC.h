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

#include "VM.h"
#include "ErrorReporting.h"
#include "SequenceStep.h"

using namespace ORchestra;

// --- note() compilation ---

TEST_CASE("Note: note(trig, note, vel, duration) compiles without channel (defaults to 1)", "[Note]")
{
    std::string file{"note(1, C4, 100, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("Note: note(trig, note, vel, duration, channel) compiles with explicit channel", "[Note]")
{
    std::string file{"note(1, C4, 100, n4, 3)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("Note: note with too few params (3) fails compilation", "[Note]")
{
    std::string file{"note(1, C4, 100)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Note: note with too few params (2) fails compilation", "[Note]")
{
    std::string file{"note(1, C4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

// --- note() channel value at runtime ---

TEST_CASE("Note: omitting channel defaults to channel 1 at runtime", "[Note]")
{
    std::string file{"note(1, C4, 100, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 1);
}

TEST_CASE("Note: explicit channel 1 matches default", "[Note]")
{
    std::string file{"note(1, C4, 100, n4, 1)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 1);
}

TEST_CASE("Note: explicit channel 5 is used at runtime", "[Note]")
{
    std::string file{"note(1, C4, 100, n4, 5)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 5);
}

TEST_CASE("Note: explicit channel 16 is used at runtime", "[Note]")
{
    std::string file{"note(1, C4, 100, n4, 16)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 16);
}

TEST_CASE("Note: channel from variable is used at runtime", "[Note]")
{
    std::string file{"ch = 10\nnote(1, C4, 100, n4, ch)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 10);
}

// --- cc() compilation ---

TEST_CASE("CC: cc(trig, ccNumber, ccValue) compiles without channel (defaults to 1)", "[CC]")
{
    std::string file{"cc(1, 74, 64)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("CC: cc(trig, ccNumber, ccValue, channel) compiles with explicit channel", "[CC]")
{
    std::string file{"cc(1, 74, 64, 2)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("CC: cc with too few params (2) fails compilation", "[CC]")
{
    std::string file{"cc(1, 74)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("CC: cc with too few params (1) fails compilation", "[CC]")
{
    std::string file{"cc(1)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

// --- cc() channel value at runtime ---

TEST_CASE("CC: omitting channel defaults to channel 1 at runtime", "[CC]")
{
    std::string file{"cc(1, 74, 64)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 1);
}

TEST_CASE("CC: explicit channel 1 matches default", "[CC]")
{
    std::string file{"cc(1, 74, 64, 1)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 1);
}

TEST_CASE("CC: explicit channel 3 is used at runtime", "[CC]")
{
    std::string file{"cc(1, 74, 64, 3)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 3);
}

TEST_CASE("CC: channel from variable is used at runtime", "[CC]")
{
    std::string file{"ch = 7\ncc(1, 74, 64, ch)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mChannel.GetValue(0) == 7);
}

// --- note() edge cases ---

TEST_CASE("Note: note with expression for trig 'note($ > 0, C4, 100, n4)' compiles", "[Note]")
{
    std::string file{"note($ > 0, C4, 100, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("Note: note with expression for velocity 'note(1, C4, 50 + 50, n4)' compiles", "[Note]")
{
    std::string file{"note(1, C4, 50 + 50, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("Note: note with variable for trig produces non-zero trigger", "[Note]")
{
    std::string file{"a = 1\nnote(a, C4, 100, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mShouldTrigger.GetValue(0) == 1);
}

TEST_CASE("Note: note with zero trig produces no step when triggered", "[Note]")
{
    std::string file{"note(0, C4, 100, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mShouldTrigger.GetValue(0) == 0);
}

TEST_CASE("Note: note with variable for note value produces correct MIDI note", "[Note]")
{
    std::string file{"n = 64\nnote(1, n, 100, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps[0].mFirst.GetValue(0) == 64);
}

TEST_CASE("Note: note with variable for velocity produces correct velocity", "[Note]")
{
    std::string file{"v = 64\nnote(1, C4, v, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps[0].mSecond.GetValue(0) == 64);
}

TEST_CASE("Note: note with $ as trig in Tick produces correct trigger", "[Note]")
{
    std::string file{"note($, C4, 100, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps0;
    REQUIRE(vm.Tick(steps0, 0));
    REQUIRE(steps0[0].mShouldTrigger.GetValue(0) == 0);

    std::vector<SequenceStep> steps1;
    REQUIRE(vm.Tick(steps1, 5));
    REQUIRE(steps1[0].mShouldTrigger.GetValue(0) == 5);
}

TEST_CASE("Note: two note() statements in sequence produce two steps", "[Note]")
{
    std::string file{"note(1, C4, 100, n4)\nnote(1, E4, 100, n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 2);
    REQUIRE(steps[0].mFirst.GetValue(0) == 60); // C4
    REQUIRE(steps[1].mFirst.GetValue(0) == 64); // E4
}

// --- cc() edge cases ---

TEST_CASE("CC: cc with expression for ccNumber compiles", "[CC]")
{
    std::string file{"cc(1, 70 + 4, 64)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("CC: cc with variable for ccNumber produces correct step", "[CC]")
{
    std::string file{"num = 74\ncc(1, num, 64)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mFirst.GetValue(0) == 74);
}

TEST_CASE("CC: cc with variable for ccValue produces correct step", "[CC]")
{
    std::string file{"val = 127\ncc(1, 74, val)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps[0].mSecond.GetValue(0) == 127);
}

TEST_CASE("CC: cc with zero trigger produces step regardless", "[CC]")
{
    std::string file{"cc(0, 74, 64)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 1);
}

TEST_CASE("CC: cc with $ as trigger", "[CC]")
{
    std::string file{"cc($, 74, 64)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 5));
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].mShouldTrigger.GetValue(0) == 5);
}
