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

TEST_CASE("UserFunction: Define and call a function that sets a variable", "[UserFunction]")
{
    std::string file = "a = [1, 2, 3]\nfn setA\na[0] = 99\nend\nsetA()\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 99);
}

TEST_CASE("UserFunction: Empty function body compiles and is a no-op", "[UserFunction]")
{
    std::string file = "a = 5\nfn noop\nend\nnoop()\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("UserFunction: Function can call built-in functions with parameters", "[UserFunction]")
{
    std::string file = "fn myPrint\nprint(42)\nend\nmyPrint()";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("UserFunction: Function can call previously defined user function", "[UserFunction]")
{
    std::string file = "a = [0]\nfn inner\na[0] = 50\nend\nfn outer\ninner()\nend\nouter()\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 50);
}

TEST_CASE("UserFunction: Multiple calls inline correctly", "[UserFunction]")
{
    std::string file = "a = [0, 0]\nfn setFirst\na[0] = 10\nend\nsetFirst()\na[1] = 20\nsetFirst()\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10);
}

TEST_CASE("UserFunction: Error - call before definition fails", "[UserFunction]")
{
    std::string file = "myFunc()\nfn myFunc\nprint(1)\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - nested function definition fails", "[UserFunction]")
{
    std::string file = "fn outer\nfn inner\nend\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - name collision with built-in function", "[UserFunction]")
{
    std::string file = "fn print\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - unterminated function body", "[UserFunction]")
{
    std::string file = "fn myFunc\nprint(1)\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - unexpected end keyword outside function", "[UserFunction]")
{
    std::string file = "a = 5\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Function with 1 parameter", "[UserFunction]")
{
    std::string file = "fn double(x)\na = x + x\nend\ndouble(5)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10);
}

TEST_CASE("UserFunction: Function with 2 parameters", "[UserFunction]")
{
    std::string file = "fn add(x, y)\na = x + y\nend\nadd(10, 20)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 30);
}

TEST_CASE("UserFunction: Function with params called multiple times with different args", "[UserFunction]")
{
    std::string file = "fn setVal(v)\na = v\nend\nsetVal(10)\nsetVal(42)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 42);
}

TEST_CASE("UserFunction: Function with params using expression as argument", "[UserFunction]")
{
    std::string file = "fn setVal(v)\na = v\nend\nsetVal(2 + 3)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("UserFunction: Function with params using note identifier as argument", "[UserFunction]")
{
    std::string file = "fn playNote(n)\na = n\nend\nplayNote(C4)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 60);
}

TEST_CASE("UserFunction: Parameterized function calling built-in", "[UserFunction]")
{
    std::string file = "fn play(n)\nnote(1, n, 100, 1)\nend\nplay(C4)";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("UserFunction: Error - wrong number of arguments (too few)", "[UserFunction]")
{
    std::string file = "fn add(x, y)\na = x + y\nend\nadd(10)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - wrong number of arguments (too many)", "[UserFunction]")
{
    std::string file = "fn single(x)\na = x\nend\nsingle(10, 20)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - missing closing paren in definition", "[UserFunction]")
{
    std::string file = "fn bad(x\na = x\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Parameterized function called from another function body", "[UserFunction]")
{
    std::string file = "fn inner(x)\na = x\nend\nfn outer(y)\ninner(y)\nend\nouter(77)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 77);
}

TEST_CASE("UserFunction: Function array selects function by index via Tick", "[UserFunction]")
{
    std::string file = "a = [0]\nfn setA10\na[0] = 10\nend\nfn setA20\na[0] = 20\nend\npattern = [setA10, setA20]\npattern($)\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps0;
    REQUIRE(vm.Tick(steps0, 0));
    StepData result0 = vm.GetTopStackValue();
    REQUIRE(result0.GetValue(0) == 10);

    std::vector<SequenceStep> steps1;
    REQUIRE(vm.Tick(steps1, 1));
    StepData result1 = vm.GetTopStackValue();
    REQUIRE(result1.GetValue(0) == 20);
}

TEST_CASE("UserFunction: Function array wraps index with modulo", "[UserFunction]")
{
    std::string file = "a = [0]\nfn setA10\na[0] = 10\nend\nfn setA20\na[0] = 20\nend\npattern = [setA10, setA20]\npattern($)\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 2));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10);
}

TEST_CASE("UserFunction: Function array with NOTE output differs per index", "[UserFunction]")
{
    std::string file = "fn verse\nnote(1, C4, 100, 1)\nend\nfn chorus\nnote(1, E4, 127, 1)\nend\npattern = [verse, chorus]\npattern($)";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps0;
    REQUIRE(vm.Tick(steps0, 0));
    REQUIRE(steps0.size() >= 1);
    REQUIRE(steps0[0].mFirst.GetValue(0) == 60);  // C4
    REQUIRE(steps0[0].mSecond.GetValue(0) == 100);

    std::vector<SequenceStep> steps1;
    REQUIRE(vm.Tick(steps1, 1));
    REQUIRE(steps1.size() >= 1);
    REQUIRE(steps1[0].mFirst.GetValue(0) == 64);  // E4
    REQUIRE(steps1[0].mSecond.GetValue(0) == 127);
}

TEST_CASE("UserFunction: Function array with expression as index", "[UserFunction]")
{
    std::string file = "a = [0]\nfn setA10\na[0] = 10\nend\nfn setA20\na[0] = 20\nend\npattern = [setA10, setA20]\npattern($ + 1)\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 20);  // index = 0 + 1 = 1, selects setA20
}

TEST_CASE("UserFunction: Multiple function arrays in one script", "[UserFunction]")
{
    std::string file = "a = [0]\nb = [0]\nfn setA1\na[0] = 1\nend\nfn setA2\na[0] = 2\nend\nfn setB1\nb[0] = 10\nend\nfn setB2\nb[0] = 20\nend\npatternA = [setA1, setA2]\npatternB = [setB1, setB2]\npatternA($)\npatternB($)\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    StepData resultA = vm.GetTopStackValue();
    REQUIRE(resultA.GetValue(0) == 1);
}

TEST_CASE("UserFunction: Error - unknown function name in function array", "[UserFunction]")
{
    std::string file = "pattern = [undefinedFunc]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Non-function identifier in array falls through to normal array", "[UserFunction]")
{
    std::string file = "a = 5\nb = [a, a]\ntest b[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}
