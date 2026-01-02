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


using namespace ORchestra;
TEST_CASE("Logical: AND operation with non-zero values '64 & 64' evaluates to 1 (true)", "[Logical]")
{
    std::string file {"a = 64 & 64 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Logical: AND operation with zero '64 & 0' evaluates to 0 (false)", "[Logical]")
{
    std::string file {"a = 64 & 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Logical: OR operation with non-zero values '64 | 64' evaluates to 1 (true)", "[Logical]")
{
    std::string file {"a = 64 | 64 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Logical: OR operation with one zero '64 | 0' evaluates to 1 (true)", "[Logical]")
{
    std::string file {"a = 64 | 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Logical: XOR operation with same values '64 ^ 64' evaluates to 0", "[Logical]")
{
    std::string file {"a = 64 ^ 64 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Logical: XOR operation with different values '1 ^ 0' evaluates to 1", "[Logical]")
{
    std::string file {"a = 1 ^ 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Logical: XOR operation with both zero '0 ^ 0' evaluates to 0", "[Logical]")
{
    std::string file {"a = 0 ^ 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}
