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

TEST_CASE("NoteDivisionLiterals: n1 compiles to value 1 (whole note)", "[NoteDivisionLiterals]")
{
    std::string file{"a = n1 \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("NoteDivisionLiterals: n2 compiles to value 2 (half note)", "[NoteDivisionLiterals]")
{
    std::string file{"a = n2 \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 2);
}

TEST_CASE("NoteDivisionLiterals: n4 compiles to value 3 (quarter note)", "[NoteDivisionLiterals]")
{
    std::string file{"a = n4 \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 3);
}

TEST_CASE("NoteDivisionLiterals: n8 compiles to value 4 (eighth note)", "[NoteDivisionLiterals]")
{
    std::string file{"a = n8 \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 4);
}

TEST_CASE("NoteDivisionLiterals: n16 compiles to value 5 (sixteenth note)", "[NoteDivisionLiterals]")
{
    std::string file{"a = n16 \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("NoteDivisionLiterals: n32 compiles to value 6 (32nd note)", "[NoteDivisionLiterals]")
{
    std::string file{"a = n32 \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 6);
}

TEST_CASE("NoteDivisionLiterals: n64 compiles to value 7 (64th note)", "[NoteDivisionLiterals]")
{
    std::string file{"a = n64 \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 7);
}

TEST_CASE("NoteDivisionLiterals: n8 in array '[n8, n4]' first element is 4", "[NoteDivisionLiterals]")
{
    std::string file{"a = [n8, n4] \n test a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 4); // n8 -> 4
}

TEST_CASE("NoteDivisionLiterals: n4 in array '[n8, n4]' second element is 3", "[NoteDivisionLiterals]")
{
    std::string file{"a = [n8, n4] \n test a[1]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 3); // n4 -> 3
}
