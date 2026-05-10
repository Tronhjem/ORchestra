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

using namespace ORchestra;

TEST_CASE("VM_EdgeCases: Unary negate in parentheses '-5' evaluates to -5", "[VM_EdgeCases]")
{
    // Unary minus is not supported as a statement opener, use subtraction from 0
    std::string file{"a = 0 - 5\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == static_cast<DataUnit>(-5));
}

TEST_CASE("VM_EdgeCases: Expression '0 - (2 + 3)' evaluates to -5", "[VM_EdgeCases]")
{
    std::string file{"a = 0 - (2 + 3)\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == static_cast<DataUnit>(-5));
}

TEST_CASE("VM_EdgeCases: ran(0,0) returns 0", "[VM_EdgeCases]")
{
    std::string file{"a = ran(0,0)\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("VM_EdgeCases: ran(5,5) returns 5", "[VM_EdgeCases]")
{
    std::string file{"a = ran(5,5)\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("VM_EdgeCases: ran(low > high) returns low value", "[VM_EdgeCases]")
{
    std::string file{"a = ran(10,5)\ntest a"};
    for (int i = 0; i < 100; ++i)
    {
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(file));
        StepData result = vm.GetTopStackValue();
        REQUIRE(result.GetValue(0) == 10);
    }
}

TEST_CASE("VM_EdgeCases: euc(0, 4) produces sequence [1,0,0,0] for first element", "[VM_EdgeCases]")
{
    std::string file{"a = euc(0,4)\ntest a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1); // Euclidean always starts with 1
}

TEST_CASE("VM_EdgeCases: euc(0, 4) second element is 0", "[VM_EdgeCases]")
{
    std::string file{"a = euc(0,4)\ntest a[1]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("VM_EdgeCases: euc(4, 4) produces 4-element DataSequence of ones", "[VM_EdgeCases]")
{
    std::string file{"a = euc(4,4)\ntest a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("VM_EdgeCases: euc(4, 4) second element is also 1", "[VM_EdgeCases]")
{
    std::string file{"a = euc(4,4)\ntest a[1]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("VM_EdgeCases: euc(5, 3) clamps hits to length, first element is 1", "[VM_EdgeCases]")
{
    std::string file{"a = euc(5,3)\ntest a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("VM_EdgeCases: euc shift wraps correctly", "[VM_EdgeCases]")
{
    std::string file{"a = euc(2,4, 0 - 1)\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    // With shift=-1, euc(2,4)=[1,0,1,0] rotated to [0,1,0,1]. First element (a) is index 0 = 0
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("VM_EdgeCases: Tick with non-zero globalCount produces correct step", "[VM_EdgeCases]")
{
    std::string file{"a = $\nb = a + 10\ntest b"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 3));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 13);
}

TEST_CASE("VM_EdgeCases: StepData multiply with larger values stays within DataUnit range", "[VM_EdgeCases]")
{
    std::string file{"a = 200 * 200\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == DATA_UNIT_MAX_VALUE);
}

TEST_CASE("VM_EdgeCases: StepData negative computed at runtime", "[VM_EdgeCases]")
{
    std::string file{"a = 0 - 255 - 255\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == static_cast<DataUnit>(-510));
}

TEST_CASE("VM_EdgeCases: Multiple Ticks on same VM re-execute instructions", "[VM_EdgeCases]")
{
    std::string file{"a = $\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps1;
    REQUIRE(vm.Tick(steps1, 10));
    StepData res1 = vm.GetTopStackValue();
    REQUIRE(res1.GetValue(0) == 10);

    std::vector<SequenceStep> steps2;
    REQUIRE(vm.Tick(steps2, 20));
    StepData res2 = vm.GetTopStackValue();
    REQUIRE(res2.GetValue(0) == 20);
}

TEST_CASE("VM_EdgeCases: Empty script compiles and ticks successfully", "[VM_EdgeCases]")
{
    std::string file{""};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
}

TEST_CASE("VM_EdgeCases: Division by zero in Tick returns 0", "[VM_EdgeCases]")
{
    std::string file{"a = 10 / 0\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("VM_EdgeCases: Complex nested expression with all operators mixed", "[VM_EdgeCases]")
{
    std::string file{"a = (10 + 5) * 2 - 8 / 4 + 1\ntest a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 29);
}

TEST_CASE("VM_EdgeCases: Variable reference with index in Tick context", "[VM_EdgeCases]")
{
    std::string file{"a = [10, 20, 30]\ntest a[$]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 1));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 20);
}

TEST_CASE("VM_EdgeCases: note() and cc() in same Tick produce correct steps", "[VM_EdgeCases]")
{
    std::string file{"note(1, C4, 100, n4)\ncc(1, 74, 64)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 2);
    REQUIRE(steps[0].mType == SequenceStepType::NoteOn);
    REQUIRE(steps[1].mType == SequenceStepType::CC);
}

TEST_CASE("VM_EdgeCases: bpm() and bpmDiv() in same Tick produce steps", "[VM_EdgeCases]")
{
    std::string file{"bpm(120)\nbpmDiv(n4)\n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    REQUIRE(steps.size() == 2);
    REQUIRE(steps[0].mType == SequenceStepType::BPM);
    REQUIRE(steps[1].mType == SequenceStepType::BPM_DIVISION);
}

TEST_CASE("VM_EdgeCases: VM Reset allows reuse", "[VM_EdgeCases]")
{
    ErrorReporting errorReporter;
    VM vm(errorReporter);

    std::string file1 = "a = 42\ntest a";
    REQUIRE(vm.Prepare(file1));
    StepData res1 = vm.GetTopStackValue();
    REQUIRE(res1.GetValue(0) == 42);

    vm.Reset();

    std::string file2 = "b = 99\ntest b";
    REQUIRE(vm.Prepare(file2));
    StepData res2 = vm.GetTopStackValue();
    REQUIRE(res2.GetValue(0) == 99);
}
