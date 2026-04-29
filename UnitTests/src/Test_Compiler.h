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
TEST_CASE("Compiler: Compiles nested array [[1,2],[3,4]], accessing a[0] returns [1,2]", "[Compiler]")
{

    std::string file = "a = [[1, 2], [3, 4]]\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 2);
    REQUIRE(result.GetValue(0) == 1);
    REQUIRE(result.GetValue(1) == 2);
}

TEST_CASE("Compiler: Evaluates complex expression '(10 + 5) * 2 - 3' correctly (result=27)", "[Compiler]")
{

    std::string file = "a = (10 + 5) * 2 - 3\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == (10 + 5) * 2 - 3);
}

TEST_CASE("Compiler: Resolves variable reference in array 'b = [a, 10, 15]' where a=5", "[Compiler]")
{

    std::string file = "a = 5\nb = [a, 10, 15]\ntest b[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("Compiler: Handles multiple assignments 'a=10, b=20, c=a+b' (result=30)", "[Compiler]")
{

    std::string file = "a = 10\nb = 20\nc = a + b\ntest c";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 30);
}

TEST_CASE("Compiler: Evaluates division '100 / 5' correctly (result=20)", "[Compiler]")
{

    std::string file = "a = 100 / 5\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 20);
}

TEST_CASE("Compiler: Evaluates subtraction '17 - 12' correctly (result=5)", "[Compiler]")
{

    std::string file = "a = 17 - 12\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("Compiler: Evaluates expressions in array '[2+2, 3*3, 10-5]' (first element=4)", "[Compiler]")
{

    std::string file = "a = [2+2, 3*3, 10-5]\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 4);
}

TEST_CASE("Compiler: Evaluates chained assignments 'a=10, b=a+5, c=b*2' (result=30)", "[Compiler]")
{

    std::string file = "a = 10\nb = a + 5\nc = b * 2\ntest c";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 30);
}

TEST_CASE("Compiler: Subtracts below zero '0 - 10' yields -10 (DataUnit is int16_t)", "[Compiler]")
{

    std::string file = "a = 0 - 10\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == static_cast<DataUnit>(-10));
}

TEST_CASE("Compiler: Addition '150 + 150' yields 300 (DataUnit is int16_t)", "[Compiler]")
{

    std::string file = "a = 150 + 150\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 300);
}

TEST_CASE("Compiler: Accesses array element 'a=[10,20,30], b=a[1]' (result=20)", "[Compiler]")
{

    std::string file = "a = [10, 20, 30]\nb = a[1]\ntest b";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 20);
}

TEST_CASE("Compiler: Evaluates AND operation '1 & 1' correctly (result=1)", "[Compiler]")
{

    std::string file = "a = 1 & 1\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Evaluates XOR operation '1 ^ 1' correctly (result=0)", "[Compiler]")
{

    std::string file = "a = 1 ^ 1\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Compiler: Evaluates OR operation '0 | 1' correctly (result=1)", "[Compiler]")
{

    std::string file = "a = 0 | 1\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Evaluates greater-than comparison '10 > 5' correctly (result=1)", "[Compiler]")
{

    std::string file = "a = 10 > 5\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Evaluates less-than comparison '5 < 10' correctly (result=1)", "[Compiler]")
{

    std::string file = "a = 5 < 10\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Evaluates equality comparison '5 == 5' correctly (result=1)", "[Compiler]")
{

    std::string file = "a = 5 == 5\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Evaluates inequality comparison '5 != 10' correctly (result=1)", "[Compiler]")
{

    std::string file = "a = 5 != 10\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Evaluates complex expression '(5 + 3) * (10 - 2)' correctly (result=64)", "[Compiler]")
{

    std::string file = "a = (5 + 3) * (10 - 2)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 64);
}
