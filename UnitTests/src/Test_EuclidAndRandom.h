#pragma once

#include "catch.hpp"

#include "VM.h"


using namespace ORchestra;
TEST_CASE("Euclidean: Rejects 'euc()' with no parameters (compilation fails)", "[Euclidean]")
{
    std::string file {"a = euc() \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Euclidean: Rejects 'euc(2)' with only one parameter (compilation fails)", "[Euclidean]")
{
    std::string file {"a = euc(2) \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("Euclidean: Generates 'euc(2,4)' sequence, first element is 1", "[Euclidean]")
{
    std::string file {"a = euc(2,4) \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Euclidean: Generates 'euc(2,4)' sequence, second element is 0", "[Euclidean]")
{
    std::string file {"a = euc(2,4) \n test a[1]"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Euclidean: Generates 'euc(2,4)' sequence, third element is 1", "[Euclidean]")
{
    std::string file {"a = euc(2,4) \n test a[2]"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Euclidean: Generates 'euc(2,4)' sequence, fourth element is 0", "[Euclidean]")
{
    std::string file {"a = euc(2,4) \n test a[3]"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}
