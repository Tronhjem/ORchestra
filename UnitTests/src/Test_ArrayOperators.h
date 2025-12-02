#pragma once

#include "catch.hpp"
#include "VM.h"

using namespace ORchestra;
TEST_CASE("Array: Access first element 'a=[64,65,63], a[0]' evaluates to 64", "[Array]")
{
    std::string file{"a = [64,65,63] \n test a[0]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 64);
}

TEST_CASE("Array: Access element in expression 'a=[64,65,63], b=a[0]+2' evaluates to 66", "[Array]")
{
    std::string file{"a = [64,65,63] \n b = a[0] + 2 \n test b"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 64 + 2);
}

TEST_CASE("Array: Assign array to variable 'a=[64,65,63], b=a' gets first element (64)", "[Array]")
{
    // Global index when not running is 0
    std::string file{"a = [64,65,63] \n b = a \n test b"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 64);
}

TEST_CASE("Array: Set specific index 'a=[1,2,3], a[1]=5' changes second element to 5", "[Array]")
{
    std::string file{"a = [1,2,3] \n a[1] = 5 \n test a[1]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("Array: Set first index 'a=[10,20,30], a[0]=99' changes first element", "[Array]")
{
    std::string file{"a = [10,20,30] \n a[0] = 99 \n test a[0]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 99);
}

TEST_CASE("Array: Set last index 'a=[5,6,7], a[2]=100' changes third element", "[Array]")
{
    std::string file{"a = [5,6,7] \n a[2] = 100 \n test a[2]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 100);
}

TEST_CASE("Array: Set index with expression 'a=[1,2,3], a[1]=a[0]+10'", "[Array]")
{
    std::string file{"a = [1,2,3] \n a[1] = a[0] + 10 \n test a[1]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 11); // 1 + 10
}

TEST_CASE("Array: Set index with variable 'a=[10,20,30], b=50, a[1]=b'", "[Array]")
{
    std::string file{"a = [10,20,30] \n b = 50 \n a[1] = b \n test a[1]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 50);
}

TEST_CASE("Array: Set index with calculated index 'a=[1,2,3,4], idx=2, a[idx]=99'", "[Array]")
{
    std::string file{"a = [1,2,3,4] \n idx = 2 \n a[idx] = 99 \n test a[2]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 99);
}

TEST_CASE("Array: Multiple index assignments 'a=[1,2,3], a[0]=10, a[1]=20, a[2]=30'", "[Array]")
{
    std::string file{"a = [1,2,3] \n a[0] = 10 \n a[1] = 20 \n a[2] = 30 \n test a[0]"};
    VM vm;
    REQUIRE(vm.Prepare(file));
    
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10);
}

TEST_CASE("Array: Verify other elements unchanged after index assignment", "[Array]")
{
    std::string file{"a = [64,65,66] \n a[1] = 100 \n test a[0]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 64); // First element should be unchanged
}

TEST_CASE("Array: Set index with note value 'a=[60,62,64], a[1]=C4'", "[Array]")
{
    std::string file{"a = [60,62,64] \n a[1] = C4 \n test a[1]"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 60); // C4 = 60
}
