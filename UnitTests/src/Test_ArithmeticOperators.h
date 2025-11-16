#pragma once

#include "catch.hpp"
#include "VM.h"

using namespace ORchestra;
TEST_CASE("Arithmetic: Assigns single value 'a = 74' and retrieves it correctly", "[Arithmetic]")
{
    const std::string file{"a = 74 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 74);
}

TEST_CASE("Arithmetic: Addition 'a = 74+1' correctly evaluates to 75", "[Arithmetic]")
{
    std::string file{"a = 74+1 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 74 + 1);
}

TEST_CASE("Arithmetic: Subtraction 'a = 74-1' correctly evaluates to 73", "[Arithmetic]")
{
    std::string file{"a = 74-1 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 74 - 1);
}

TEST_CASE("Arithmetic: Multiplication 'a = 2*2' correctly evaluates to 4", "[Arithmetic]")
{
    std::string file{"a = 2*2 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 2 * 2);
}

TEST_CASE("Arithmetic: Division 'a = 2/2' correctly evaluates to 1", "[Arithmetic]")
{
    std::string file{"a = 2/2 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 2 / 2);
}

TEST_CASE("Arithmetic: Modulo '10 % 3' correctly evaluates to 1", "[Arithmetic]")
{
    std::string file{"a = 10 % 3 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10 % 3);
}

TEST_CASE("Arithmetic: Modulo '7 % 2' correctly evaluates to 1", "[Arithmetic]")
{
    std::string file{"a = 7 % 2 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 7 % 2);
}

TEST_CASE("Arithmetic: Modulo '8 % 4' correctly evaluates to 0", "[Arithmetic]")
{
    std::string file{"a = 8 % 4 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 8 % 4);
}

TEST_CASE("Arithmetic: Modulo by zero '5 % 0' returns 0 safely", "[Arithmetic]")
{
    std::string file{"a = 5 % 0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}

TEST_CASE("Arithmetic: Operator precedence '2 + 2 * 5' evaluates to 12 (not 20)", "[Arithmetic]")
{
    std::string file{"a = 2 + 2 * 5 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 12 /* (2 + 2) * 5 */);
}

TEST_CASE("Arithmetic: Operator precedence with modulo '10 + 7 % 3' evaluates to 11", "[Arithmetic]")
{
    std::string file{"a = 10 + 7 % 3 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 10 + (7 % 3));
}

TEST_CASE("Arithmetic: Simple parentheses '(2) + 2' evaluates to 4", "[Arithmetic]")
{
    std::string file{"a = (2) + 2 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 4);
}

TEST_CASE("Arithmetic: Modulo in expression '10 % 3 + 2' evaluates to 3", "[Arithmetic]")
{
    std::string file{"a = 10 % 3 + 2 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == (10 % 3) + 2);
}

TEST_CASE("Arithmetic: Parentheses override precedence '(2 + 2) * 2' evaluates to 8", "[Arithmetic]")
{
    std::string file{"a = (2 + 2) * 2 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 8);
}

TEST_CASE("Arithmetic: Nested parentheses '((2 - 1) + 2) * 2' evaluates to 6", "[Arithmetic]")
{
    std::string file{"a = ((2 - 1) + 2) * 2 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 6);
}

TEST_CASE("Arithmetic: Variable assignment 'a=5, b=a' evaluates to 5", "[Arithmetic]")
{
    std::string file{"a = 5 \n b = a \n test b"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("Arithmetic: Variable in expression 'a=5, b=a+2*5' evaluates to 15", "[Arithmetic]")
{
    std::string file{"a = 5 \n b = a + 2 * 5 \n test b"};
    VM vm;
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5 + (2 * 5));
}
