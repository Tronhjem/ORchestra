#pragma once

#include "catch.hpp"
#include "VM.h"

using namespace ORchestra;
TEST_CASE("Comparison: Greater-than '1 > 0' evaluates to 1 (true)", "[Comparison]")
{
    std::string file{"a = 1 > 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Comparison: Greater-than '1 > 5' evaluates to 0 (false)", "[Comparison]")
{
    std::string file{"a = 1 > 5 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Comparison: Greater-or-equal '1 >= 1' with equal values evaluates to 1 (true)", "[Comparison]")
{
    std::string file{"a = 1 >= 1 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Comparison: Greater-or-equal '1 >= 0' with greater value evaluates to 1 (true)", "[Comparison]")
{
    std::string file{"a = 1 >= 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Comparison: Greater-or-equal '1 >= 5' with less value evaluates to 0 (false)", "[Comparison]")
{
    std::string file{"a = 1 >= 5 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Comparison: Less-than '1 < 0' evaluates to 0 (false)", "[Comparison]")
{
    std::string file{"a = 1 < 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Comparison: Less-than '1 < 5' evaluates to 1 (true)", "[Comparison]")
{
    std::string file{"a = 1 < 5 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Comparison: Less-or-equal '1 <= 1' with equal values evaluates to 1 (true)", "[Comparison]")
{
    std::string file{"a = 1 <= 1 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Comparison: Less-or-equal '1 <= 5' with less value evaluates to 1 (true)", "[Comparison]")
{
    std::string file{"a = 1 <= 5 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Comparison: Less-or-equal '1 <= 0' with greater value evaluates to 0 (false)", "[Comparison]")
{
    std::string file{"a = 1 <= 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Comparison: Inequality '1 != 0' with different values evaluates to 1 (true)", "[Comparison]")
{
    std::string file{"a = 1 != 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("Comparison: Inequality '1 != 1' with same values evaluates to 0 (false)", "[Comparison]")
{
    std::string file{"a = 1 != 1 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}
