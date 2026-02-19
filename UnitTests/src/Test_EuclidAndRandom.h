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
TEST_CASE("Euclidean: Rejects 'euc()' with no parameters (compilation fails)", "[Euclidean]")
{
    std::string file{"a = euc() \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Euclidean: Rejects 'euc(2)' with only one parameter (compilation fails)", "[Euclidean]")
{
    std::string file{"a = euc(2) \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Euclidean: Generates 'euc(2,4)' sequence, first element is 1", "[Euclidean]")
{
    std::string file{"a = euc(2,4) \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Euclidean: Generates 'euc(2,4)' sequence, second element is 0", "[Euclidean]")
{
    std::string file{"a = euc(2,4) \n test a[1]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Euclidean: Generates 'euc(2,4)' sequence, third element is 1", "[Euclidean]")
{
    std::string file{"a = euc(2,4) \n test a[2]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Euclidean: Generates 'euc(2,4)' sequence, fourth element is 0", "[Euclidean]")
{
    std::string file{"a = euc(2,4) \n test a[3]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}
