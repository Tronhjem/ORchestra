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

// --- bpm() ---

TEST_CASE("BuiltInFunctions: bpm(120) compiles successfully", "[BuiltInFunctions]")
{
    std::string file{"bpm(120)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("BuiltInFunctions: bpm with variable argument compiles", "[BuiltInFunctions]")
{
    std::string file{"tempo = 140\nbpm(tempo)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("BuiltInFunctions: bpm() produces BPM SequenceStep at Tick", "[BuiltInFunctions]")
{
    std::string file{"bpm(120)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE_FALSE(steps.empty());
    REQUIRE(steps[0].mType == SequenceStepType::BPM);
    REQUIRE(steps[0].mFirst.GetValue(0) == 120);
}

TEST_CASE("BuiltInFunctions: bpm(0) fails (zero BPM is invalid)", "[BuiltInFunctions]")
{
    // bpm(0) should still compile but produce a 0 value step
    std::string file{"bpm(0)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE_FALSE(steps.empty());
    REQUIRE(steps[0].mType == SequenceStepType::BPM);
}

TEST_CASE("BuiltInFunctions: bpm(255) compiles and runs (max constant value)", "[BuiltInFunctions]")
{
    std::string file{"bpm(255)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE_FALSE(steps.empty());
    REQUIRE(steps[0].mType == SequenceStepType::BPM);
    REQUIRE(steps[0].mFirst.GetValue(0) == 255);
}

TEST_CASE("BuiltInFunctions: bpm with no params fails", "[BuiltInFunctions]")
{
    std::string file{"bpm()\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("BuiltInFunctions: bpm with too many params fails", "[BuiltInFunctions]")
{
    std::string file{"bpm(120, 60)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

// --- beat() ---

TEST_CASE("BuiltInFunctions: beat(n4) compiles successfully", "[BuiltInFunctions]")
{
    std::string file{"beat(n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("BuiltInFunctions: beat with variable argument compiles", "[BuiltInFunctions]")
{
    std::string file{"div = n8\nbeat(div)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("BuiltInFunctions: beat(n4) produces BEAT SequenceStep at Tick", "[BuiltInFunctions]")
{
    std::string file{"beat(n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE_FALSE(steps.empty());
    REQUIRE(steps[0].mType == SequenceStepType::BEAT);
    REQUIRE(steps[0].mFirst.GetValue(0) == 3); // n4 -> 3
}

TEST_CASE("BuiltInFunctions: beat(n1) produces value 1", "[BuiltInFunctions]")
{
    std::string file{"beat(n1)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps[0].mType == SequenceStepType::BEAT);
    REQUIRE(steps[0].mFirst.GetValue(0) == 1);
}

TEST_CASE("BuiltInFunctions: beat(n16) produces value 5", "[BuiltInFunctions]")
{
    std::string file{"beat(n16)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps[0].mType == SequenceStepType::BEAT);
    REQUIRE(steps[0].mFirst.GetValue(0) == 5);
}

TEST_CASE("BuiltInFunctions: beat with no params fails", "[BuiltInFunctions]")
{
    std::string file{"beat()\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("BuiltInFunctions: beat with too many params fails", "[BuiltInFunctions]")
{
    std::string file{"beat(n4, n2)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

// --- transpose() ---

TEST_CASE("BuiltInFunctions: transpose(12) compiles successfully", "[BuiltInFunctions]")
{
    std::string file{"transpose(12)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("BuiltInFunctions: transpose(-5) with negative value compiles", "[BuiltInFunctions]")
{
    std::string file{"transpose(0 - 5)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("BuiltInFunctions: transpose with variable argument compiles", "[BuiltInFunctions]")
{
    std::string file{"semitones = 7\ntranspose(semitones)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("BuiltInFunctions: transpose(12) produces TRANSPOSE SequenceStep at Tick", "[BuiltInFunctions]")
{
    std::string file{"transpose(12)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE_FALSE(steps.empty());
    REQUIRE(steps[0].mType == SequenceStepType::TRANSPOSE);
    REQUIRE(steps[0].mFirst.GetValue(0) == 12);
}

TEST_CASE("BuiltInFunctions: transpose with no params fails", "[BuiltInFunctions]")
{
    std::string file{"transpose()\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("BuiltInFunctions: transpose with too many params fails", "[BuiltInFunctions]")
{
    std::string file{"transpose(12, 5)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

// --- print() ---

TEST_CASE("BuiltInFunctions: print(42) produces PRINT SequenceStep at Tick", "[BuiltInFunctions]")
{
    std::string file{"print(42)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE_FALSE(steps.empty());
    REQUIRE(steps[0].mType == SequenceStepType::PRINT);
    REQUIRE(steps[0].mFirst.GetValue(0) == 42);
}

TEST_CASE("BuiltInFunctions: print with variable argument produces correct output", "[BuiltInFunctions]")
{
    std::string file{"x = 99\nprint(x)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE_FALSE(steps.empty());
    REQUIRE(steps[0].mType == SequenceStepType::PRINT);
    REQUIRE(steps[0].mFirst.GetValue(0) == 99);
}

TEST_CASE("BuiltInFunctions: print($) prints globalCount", "[BuiltInFunctions]")
{
    std::string file{"print($)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 7));
    REQUIRE_FALSE(steps.empty());
    REQUIRE(steps[0].mType == SequenceStepType::PRINT);
    REQUIRE(steps[0].mFirst.GetValue(0) == 7);
}
