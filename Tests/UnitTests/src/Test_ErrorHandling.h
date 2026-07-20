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
TEST_CASE("Error: Detects unterminated array 'a = [2,' (compilation fails)", "[Error]")
{
    std::string file{"a = [2, \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects double comma in array 'a = [2,,]' (compilation fails)", "[Error]")
{
    std::string file{"a = [2,,] \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing comma in array 'a = [2 3,]' (compilation fails)", "[Error]")
{
    std::string file{"a = [2 3,] \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects incomplete expression 'a = 2 +' (compilation fails)", "[Error]")
{
    std::string file{"a = 2 + \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects incomplete expression 'a = (2 + 3) +' (compilation fails)", "[Error]")
{
    std::string file{"a = (2 + 3) + \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects operator without value 'a = ( + 3) +' (compilation fails)", "[Error]")
{
    std::string file{"a = ( + 3) + \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects empty parentheses 'a = () + 2' (compilation fails)", "[Error]")
{
    std::string file{"a = () + 2 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing operator between values 'a = (2 - 3) 2' (compilation fails)", "[Error]")
{
    std::string file{"a = (2 - 3) 2 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects operator at start 'a = + (2 - 3)' (compilation fails)", "[Error]")
{
    std::string file{"a = + (2 - 3) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects unmatched opening parenthesis 'a = (2 - 3' (compilation fails)", "[Error]")
{
    std::string file{"a = (2 - 3 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects unmatched closing parenthesis 'a = 2 - 3)' (compilation fails)", "[Error]")
{
    std::string file{"a = 2 - 3) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects extra closing parenthesis 'a = (2 - 3) + 2)' (compilation fails)", "[Error]")
{
    std::string file{"a = (2 - 3) + 2) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing comma in ran 'a = ran{2 3}' (compilation fails)", "[Error]")
{
    std::string file{"a = ran{2 3} \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects trailing comma in ran 'a = ran{2,}' (compilation fails)", "[Error]")
{
    std::string file{"a = ran{2,} \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects unmatched parenthesis in function 'print(2' (compilation fails)", "[Error]")
{
    std::string file{"print(2 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects too many parameters in 'print(2,2)' (compilation fails)", "[Error]")
{
    std::string file{"print(2,2) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing parameters in 'print()' (compilation fails)", "[Error]")
{
    std::string file{"print() \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects invalid function name 'invalid()' (compilation fails)", "[Error]")
{
    std::string file{"invalid() \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects unsupported character '@' (compilation fails)", "[Error]")
{
    std::string file{"a = @ \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);

    const auto& errors = errorReporter.GetErrors();
    REQUIRE_FALSE(errors.empty());
    REQUIRE(errors[0].mMessage.find("Unexpected Character '@'") != std::string::npos);
}

TEST_CASE("Error: Detects trailing comma in array 'a = [1,]' (compilation fails)", "[Error]")
{
    std::string file{"a = [1,] \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);

    const auto& errors = errorReporter.GetErrors();
    REQUIRE_FALSE(errors.empty());
    REQUIRE(errors[0].mMessage.find("Unexpected Character ','") != std::string::npos);
}

TEST_CASE("Error: Detects consecutive commas in function call 'note(1,,2,3,4)' (compilation fails)", "[Error]")
{
    std::string file{"note(1,,2,3,4) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);

    const auto& errors = errorReporter.GetErrors();
    REQUIRE_FALSE(errors.empty());
    REQUIRE(errors[0].mMessage.find("Unexpected Character ','") != std::string::npos);
}

TEST_CASE("Error: Detects trailing comma in note 'note(1, C4, 100, n4,)' (compilation fails)", "[Error]")
{
    std::string file{"note(1, C4, 100, n4,) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects too many params in note 'note(1, C4, 100, n4, 1, 2)' (compilation fails)", "[Error]")
{
    std::string file{"note(1, C4, 100, n4, 1, 2) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing comma in note 'note(1 C4 100 n4)' (compilation fails)", "[Error]")
{
    std::string file{"note(1 C4 100 n4) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects trailing comma in cc 'cc(1, 74, 64,)' (compilation fails)", "[Error]")
{
    std::string file{"cc(1, 74, 64,) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects too many params in cc 'cc(1, 74, 64, 2, 3)' (compilation fails)", "[Error]")
{
    std::string file{"cc(1, 74, 64, 2, 3) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing comma in cc 'cc(1 74 64)' (compilation fails)", "[Error]")
{
    std::string file{"cc(1 74 64) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing right paren in note 'note(1, C4, 100, n4' (compilation fails)", "[Error]")
{
    std::string file{"note(1, C4, 100, n4 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects missing right paren in cc 'cc(1, 74, 64' (compilation fails)", "[Error]")
{
    std::string file{"cc(1, 74, 64 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects array max length exceeded 'a=[1,...,33]' (compilation fails)", "[Error]")
{
    std::string file{"a = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33] \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Nested array too deep '[[[1]]]' (compilation fails)", "[Error]")
{
    std::string file{"a = [[[1]]] \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Missing comma after euc 'a = euc(2)' (compilation fails)", "[Error]")
{
    std::string file{"a = euc(2) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Missing right paren in euc 'a = euc(2, 4' (compilation fails)", "[Error]")
{
    std::string file{"a = euc(2, 4 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Trailing comma in euc 'a = euc(2,4,)' (compilation fails)", "[Error]")
{
    std::string file{"a = euc(2,4,) \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects two operators in sequence 'a = 2 + * 3' (compilation fails)", "[Error]")
{
    std::string file{"a = 2 + * 3 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects two comparison operators in sequence 'a = 2 > < 3' (compilation fails)", "[Error]")
{
    std::string file{"a = 2 > < 3 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects lone operator at line start '= 5' (compilation fails)", "[Error]")
{
    std::string file{"= 5 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects invalid note letter 'H4' (compilation fails)", "[Error]")
{
    std::string file{"a = H4 \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects incomplete note 'C#' without octave (compilation fails)", "[Error]")
{
    std::string file{"a = C# \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects incomplete note 'Bb' without octave (compilation fails)", "[Error]")
{
    std::string file{"a = Bb \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects undefined variable in expression 'a = b' (compilation fails)", "[Error]")
{
    // Using undefined variable in expression that resolves to compile-time eval
    std::string file{"a = b + 2 \n test a"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    // b is treated as variable that hasn't been defined - should fail
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects stray closing bracket ']' at line start (compilation fails)", "[Error]")
{
    std::string file{"] \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects stray closing paren ')' at line start (compilation fails)", "[Error]")
{
    std::string file{") \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects stray closing brace '}' at line start (compilation fails)", "[Error]")
{
    std::string file{"} \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects comma at start of line (compilation fails)", "[Error]")
{
    std::string file{", \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Detects dot at start of line (compilation fails)", "[Error]")
{
    std::string file{". \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Empty parentheses in array 'a = [()]' (compilation fails)", "[Error]")
{
    std::string file{"a = [()] \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Multiple errors in one line reported correctly", "[Error]")
{
    std::string file{"@ # \n"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);

    const auto& errors = errorReporter.GetErrors();
    REQUIRE_FALSE(errors.empty());
}

TEST_CASE("Error: Variable name collision with built-in 'print' fails", "[Error]")
{
    std::string file{"print = 5 \n test print"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    // print is a built-in function, cannot be used as variable
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("Error: Using 'ran' as a variable name shadows built-in function (allowed)", "[Error]")
{
    std::string file{"ran = 5 \n test ran"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    // ran is now a regular IDENTIFIER like bpm/beat; variable assignment succeeds
    REQUIRE(vm.Prepare(file));
}

TEST_CASE("Error: Variable name collision with built-in 'euc' fails", "[Error]")
{
    std::string file{"euc = 5 \n test euc"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file) == false);
}
