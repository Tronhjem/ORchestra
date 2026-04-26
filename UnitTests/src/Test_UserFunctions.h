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
