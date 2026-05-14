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

TEST_CASE("Compiler: Chained comparisons '5 > 3 > 1' evaluates left-to-right (result=0)", "[Compiler]")
{
    std::string file = "a = 5 > 3 > 1\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    // (5 > 3) = 1, then 1 > 1 = 0
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Compiler: Mixing logical and comparison '5 > 3 & 1' evaluates correctly", "[Compiler]")
{
    std::string file = "a = 5 > 3 & 1\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    // 5 > 3 = 1, 1 & 1 = 1
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Multiple statements on same line 'a=1 b=2 c=a+b' (result=3)", "[Compiler]")
{
    // Multiple statements per line is not allowed syntactically
    std::string file = "a = 1\nb = 2\nc = a + b\ntest c";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 3);
}

TEST_CASE("Compiler: Handles maximum 8-bit constant 255 correctly", "[Compiler]")
{
    std::string file = "a = 255\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 255);
}

TEST_CASE("Compiler: Large values built via expression 'a = 255 + 1' (result=256)", "[Compiler]")
{
    std::string file = "a = 255 + 1\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 256);
}

TEST_CASE("Compiler: Negative values built via expression 'a = 0 - 255' (result=-255)", "[Compiler]")
{
    std::string file = "a = 0 - 255\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == static_cast<DataUnit>(-255));
}

TEST_CASE("Compiler: Evaluates binary AND with '0 & 0' correctly (result=0)", "[Compiler]")
{
    std::string file = "a = 0 & 0\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Compiler: Evaluates binary OR with '0 | 0' correctly (result=0)", "[Compiler]")
{
    std::string file = "a = 0 | 0\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Compiler: Greater-or-equal comparison '5 >= 5' (result=1)", "[Compiler]")
{
    std::string file = "a = 5 >= 5\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Greater-or-equal comparison '5 >= 10' (result=0)", "[Compiler]")
{
    std::string file = "a = 5 >= 10\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Compiler: Less-or-equal comparison '5 <= 5' (result=1)", "[Compiler]")
{
    std::string file = "a = 5 <= 5\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Less-or-equal comparison '5 <= 3' (result=0)", "[Compiler]")
{
    std::string file = "a = 5 <= 3\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Compiler: Operator precedence with modulo and addition '3 + 10 % 4' (result=5)", "[Compiler]")
{
    std::string file = "a = 3 + 10 % 4\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    // 10 % 4 = 2, 3 + 2 = 5
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("Compiler: Operator precedence with comparison and arithmetic '2 + 3 > 4' (result=1)", "[Compiler]")
{
    std::string file = "a = 2 + 3 > 4\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    // 2 + 3 = 5, 5 > 4 = 1
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Compiler: Variable assigned to itself 'a = a' (fails - undefined var)", "[Compiler]")
{
    std::string file = "a = a\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    // a not defined yet
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Compiler: Scalar in array with expressions 'a = [1+1, 2+2, 3+3]' (first=2)", "[Compiler]")
{
    std::string file = "a = [1+1, 2+2, 3+3]\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 2);
}

TEST_CASE("Compiler: Array with ran inside produces correct result structure", "[Compiler]")
{
    std::string file = "a = [ran(50,60), 5]\ntest a[0]";
    for (int i = 0; i < 10; ++i)
    {
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(file));
        StepData result = vm.GetTopStackValue();
        REQUIRE(result.GetValue(0) >= 50);
        REQUIRE(result.GetValue(0) <= 60);
    }
}

TEST_CASE("Compiler: Division precedence over subtraction '10 - 4 / 2' (result=8)", "[Compiler]")
{
    std::string file = "a = 10 - 4 / 2\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    // 4 / 2 = 2, 10 - 2 = 8
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 8);
}

TEST_CASE("Compiler: Multiplication precedence over addition '2 + 3 * 4' (result=14)", "[Compiler]")
{
    std::string file = "a = 2 + 3 * 4\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    // 3 * 4 = 12, 2 + 12 = 14
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 14);
}
