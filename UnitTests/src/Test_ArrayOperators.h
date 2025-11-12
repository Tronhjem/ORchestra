#pragma once

#include "../catch.hpp"
#include "VM.h"

TEST_CASE("Array: Access first element 'a=[64,65,63], a[0]' evaluates to 64", "[Array]")
{
    std::string file {"a = [64,65,63] \n test a[0]"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 64);
}

TEST_CASE("Array: Access element in expression 'a=[64,65,63], b=a[0]+2' evaluates to 66", "[Array]")
{
    std::string file {"a = [64,65,63] \n b = a[0] + 2 \n test b"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 64 + 2);
}

TEST_CASE("Array: Assign array to variable 'a=[64,65,63], b=a' gets first element (64)", "[Array]")
{
    // Global index when not running is 0
    std::string file {"a = [64,65,63] \n b = a \n test b"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 64);
}
