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
TEST_CASE("Error: Detects unterminated array 'a = [2,' (compilation fails)", "[Error]")
{
    std::string file{"a = [2, \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects double comma in array 'a = [2,,]' (compilation fails)", "[Error]")
{
    std::string file{"a = [2,,] \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing comma in array 'a = [2 3,]' (compilation fails)", "[Error]")
{
    std::string file{"a = [2 3,] \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects incomplete expression 'a = 2 +' (compilation fails)", "[Error]")
{
    std::string file{"a = 2 + \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects incomplete expression 'a = (2 + 3) +' (compilation fails)", "[Error]")
{
    std::string file{"a = (2 + 3) + \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects operator without value 'a = ( + 3) +' (compilation fails)", "[Error]")
{
    std::string file{"a = ( + 3) + \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects empty parentheses 'a = () + 2' (compilation fails)", "[Error]")
{
    std::string file{"a = () + 2 \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing operator between values 'a = (2 - 3) 2' (compilation fails)", "[Error]")
{
    std::string file{"a = (2 - 3) 2 \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects operator at start 'a = + (2 - 3)' (compilation fails)", "[Error]")
{
    std::string file{"a = + (2 - 3) \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects unmatched opening parenthesis 'a = (2 - 3' (compilation fails)", "[Error]")
{
    std::string file{"a = (2 - 3 \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects unmatched closing parenthesis 'a = 2 - 3)' (compilation fails)", "[Error]")
{
    std::string file{"a = 2 - 3) \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects extra closing parenthesis 'a = (2 - 3) + 2)' (compilation fails)", "[Error]")
{
    std::string file{"a = (2 - 3) + 2) \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing comma in ran 'a = ran{2 3}' (compilation fails)", "[Error]")
{
    std::string file{"a = ran{2 3} \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects trailing comma in ran 'a = ran{2,}' (compilation fails)", "[Error]")
{
    std::string file{"a = ran{2,} \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects unmatched parenthesis in function 'print(2' (compilation fails)", "[Error]")
{
    std::string file{"print(2 \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects too many parameters in 'print(2,2)' (compilation fails)", "[Error]")
{
    std::string file{"print(2,2) \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing parameters in 'print()' (compilation fails)", "[Error]")
{
    std::string file{"print() \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects invalid function name 'invalid()' (compilation fails)", "[Error]")
{
    std::string file{"invalid() \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects unsupported character '@' (compilation fails)", "[Error]")
{
    std::string file{"a = @ \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);

    const auto& errors = vm.GetErrors();
    REQUIRE_FALSE(errors.empty());
    REQUIRE(errors[0].mMessage.find("Unsupported character '@'") != std::string::npos);
}

TEST_CASE("Error: Detects trailing comma in array 'a = [1,]' (compilation fails)", "[Error]")
{
    std::string file{"a = [1,] \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing function argument 'note(1,,2,3,4)' (compilation fails)", "[Error]")
{
    std::string file{"note(1,,2,3,4) \n"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}
