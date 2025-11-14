#pragma once

#include "../catch.hpp"
#include "VM.h"


using namespace ORchestra;
TEST_CASE("Error: Detects unterminated array 'a = [2,' (compilation fails)", "[Error]")
{
    std::string file {"a = [2, \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects double comma in array 'a = [2,,]' (compilation fails)", "[Error]")
{
    std::string file {"a = [2,,] \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects missing comma in array 'a = [2 3,]' (compilation fails)", "[Error]")
{
    std::string file {"a = [2 3,] \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects incomplete expression 'a = 2 +' (compilation fails)", "[Error]")
{
    std::string file {"a = 2 + \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects incomplete expression 'a = (2 + 3) +' (compilation fails)", "[Error]")
{
    std::string file {"a = (2 + 3) + \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects operator without value 'a = ( + 3) +' (compilation fails)", "[Error]")
{
    std::string file {"a = ( + 3) + \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects empty parentheses 'a = () + 2' (compilation fails)", "[Error]")
{
    std::string file {"a = () + 2 \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects missing operator between values 'a = (2 - 3) 2' (compilation fails)", "[Error]")
{
    std::string file {"a = (2 - 3) 2 \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects operator at start 'a = + (2 - 3)' (compilation fails)", "[Error]")
{
    std::string file {"a = + (2 - 3) \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects unmatched opening parenthesis 'a = (2 - 3' (compilation fails)", "[Error]")
{
    std::string file {"a = (2 - 3 \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects unmatched closing parenthesis 'a = 2 - 3)' (compilation fails)", "[Error]")
{
    std::string file {"a = 2 - 3) \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects extra closing parenthesis 'a = (2 - 3) + 2)' (compilation fails)", "[Error]")
{
    std::string file {"a = (2 - 3) + 2) \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects missing comma in ran 'a = ran{2 3}' (compilation fails)", "[Error]")
{
    std::string file {"a = ran{2 3} \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects trailing comma in ran 'a = ran{2,}' (compilation fails)", "[Error]")
{
    std::string file {"a = ran{2,} \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects unmatched parenthesis in function 'print(2' (compilation fails)", "[Error]")
{
    std::string file {"print(2 \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects too many parameters in 'print(2,2)' (compilation fails)", "[Error]")
{
    std::string file {"print(2,2) \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects missing parameters in 'print()' (compilation fails)", "[Error]")
{
    std::string file {"print() \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Error: Detects invalid function name 'invalid()' (compilation fails)", "[Error]")
{
    std::string file {"invalid() \n"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}
