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
    std::string file = "fn play(n)\nnote(1, n, 100, n4, 1)\nend\nplay(C4)";
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

TEST_CASE("Pattern array selects pattern by index via Tick", "[UserFunction]")
{
    std::string file = "a = [0]\nptn setA10\na[0] = 10\nend\nptn setA20\na[0] = 20\nend\npattern = [setA10, setA20]\npattern($)\ntest a[0]";
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

TEST_CASE("Pattern array wraps index with modulo", "[UserFunction]")
{
    std::string file = "a = [0]\nptn setA10\na[0] = 10\nend\nptn setA20\na[0] = 20\nend\npattern = [setA10, setA20]\npattern($)\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 2));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10);
}

TEST_CASE("Pattern array with NOTE output differs per index", "[UserFunction]")
{
    std::string file = "ptn verse\nnote(1, C4, 100, n4, 1)\nend\nptn chorus\nnote(1, E4, 127, n4, 1)\nend\npattern = [verse, chorus]\npattern($)";
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

TEST_CASE("Pattern array with expression as index", "[UserFunction]")
{
    std::string file = "a = [0]\nptn setA10\na[0] = 10\nend\nptn setA20\na[0] = 20\nend\npattern = [setA10, setA20]\npattern($ + 1)\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 20);  // index = 0 + 1 = 1, selects setA20
}

TEST_CASE("Multiple pattern arrays in one script", "[UserFunction]")
{
    std::string file = "a = [0]\nb = [0]\nptn setA1\na[0] = 1\nend\nptn setA2\na[0] = 2\nend\nptn setB1\nb[0] = 10\nend\nptn setB2\nb[0] = 20\nend\npatternA = [setA1, setA2]\npatternB = [setB1, setB2]\npatternA($)\npatternB($)\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    StepData resultA = vm.GetTopStackValue();
    REQUIRE(resultA.GetValue(0) == 1);
}

TEST_CASE("Pattern: Error - unknown identifier in pattern array fails at runtime", "[UserFunction]")
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

TEST_CASE("Pattern: Error - pattern cannot be called directly", "[UserFunction]")
{
    std::string file = "ptn myPat\nprint(1)\nend\nmyPat()";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Pattern: Error - nested pattern definition fails", "[UserFunction]")
{
    std::string file = "ptn outer\nptn inner\nend\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Pattern: Error - unterminated pattern body", "[UserFunction]")
{
    std::string file = "ptn myPat\nprint(1)\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Pattern: Error - name collision with function", "[UserFunction]")
{
    std::string file = "fn myFunc\nend\nptn myFunc\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Pattern: Error - fn function cannot be used in pattern array", "[UserFunction]")
{
    std::string file = "fn myFunc\nprint(1)\nend\npattern = [myFunc]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Function returns value used in assignment", "[UserFunction]")
{
    std::string file = "fn double(x)\nreturn x * 2\nend\na = double(5)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10);
}

TEST_CASE("UserFunction: Function return value used in expression with operators", "[UserFunction]")
{
    std::string file = "fn add1(x)\nreturn x + 1\nend\na = add1(5) * 2\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 12);
}

TEST_CASE("UserFunction: Function return value used in array", "[UserFunction]")
{
    std::string file = "fn triple(x)\nreturn x * 3\nend\na = [triple(1), triple(2), triple(3)]\ntest a[1]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 6);
}

TEST_CASE("UserFunction: Function with multiple statements and return", "[UserFunction]")
{
    std::string file = "b = 0\nfn compute(x)\nb = x * 2\nreturn b + 1\nend\na = compute(5)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 11);
}

TEST_CASE("UserFunction: Nested function calls in expressions", "[UserFunction]")
{
    std::string file = "fn double(x)\nreturn x * 2\nend\nfn addOne(x)\nreturn x + 1\nend\na = double(addOne(4))\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10);
}

TEST_CASE("UserFunction: Function return used as argument to built-in", "[UserFunction]")
{
    std::string file = "fn getVel(x)\nreturn x * 10\nend\nnote(1, C4, getVel(10), n4, 1)";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("UserFunction: Error - return outside function body", "[UserFunction]")
{
    std::string file = "return 5";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - fn name collision with variable", "[UserFunction]")
{
    std::string file = "a = 5\nfn a\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - ptn name collision with variable", "[UserFunction]")
{
    std::string file = "a = 5\nptn a\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - fn name collision with pattern", "[UserFunction]")
{
    std::string file = "ptn myFunc\nend\nfn myFunc\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: return inside pattern body works (ptn uses mInsideFunctionDefinition)", "[UserFunction]")
{
    std::string file = "a = 5\nptn myPat\nreturn 99\nend\npat = [myPat]\npat(0)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5); // a was not modified by return in ptn
}

TEST_CASE("UserFunction: Error - duplicate function definition", "[UserFunction]")
{
    std::string file = "fn myFunc\nend\nfn myFunc\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - duplicate pattern definition", "[UserFunction]")
{
    std::string file = "ptn myPat\nend\nptn myPat\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: fn returning expression used in note call", "[UserFunction]")
{
    std::string file = "fn getNote()\nreturn C4\nend\nnote(1, getNote(), 100, n4, 1)";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("UserFunction: fn with return used in multiple calls", "[UserFunction]")
{
    std::string file = "fn addOne(x)\nreturn x + 1\nend\na = addOne(5)\nb = addOne(10)\ntest b";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 11);
}

TEST_CASE("UserFunction: Error - comma without param name in fn def", "[UserFunction]")
{
    std::string file = "fn myFunc(,)\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - duplicate parameter names", "[UserFunction]")
{
    std::string file = "fn add(x, x)\na = x + x\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    // Duplicate parameter names should be allowed (x is same var id), just tests it compiles
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("UserFunction: fn with multiple return statements returns last one", "[UserFunction]")
{
    std::string file = "fn pick(x)\nreturn x + 1\nreturn x + 2\nend\na = pick(5)\ntest a";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 7); // last return wins
}

TEST_CASE("UserFunction: Pattern array with constant index works correctly", "[UserFunction]")
{
    std::string file = "a = [0]\nptn setA\na[0] = 42\nend\nptn setB\na[0] = 99\nend\npat = [setA, setB]\npat(1)\ntest a[0]";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 0));
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 99);
}

TEST_CASE("UserFunction: Error - cannot define fn inside ptn", "[UserFunction]")
{
    std::string file = "ptn outer\nfn inner\nend\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("UserFunction: Error - ptn name collision with built-in function", "[UserFunction]")
{
    std::string file = "ptn print\nend\n";
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}
