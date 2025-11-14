#pragma once

#include "../catch.hpp"
#include "CustomStack.h"
#include "StepData.h"


using namespace ORchestra;
TEST_CASE("CustomStack: LIFO order maintained for integer values (Push 10,20,30 -> Pop returns 30,20,10)", "[CustomStack]")
{
    Stack<int> stack;
    stack.Push(10);
    stack.Push(20);
    stack.Push(30);
    
    REQUIRE(stack.Pop() == 30);
    REQUIRE(stack.Pop() == 20);
    REQUIRE(stack.Pop() == 10);
}

TEST_CASE("CustomStack: LIFO order maintained for StepData values (Push 5,10,15 -> Pop returns 15,10,5)", "[CustomStack]")
{
    Stack<StepData> stack;
    StepData data1(5);
    StepData data2(10);
    StepData data3(15);
    
    stack.Push(data1);
    stack.Push(data2);
    stack.Push(data3);
    
    StepData result3 = stack.Pop();
    StepData result2 = stack.Pop();
    StepData result1 = stack.Pop();
    
    REQUIRE(result3.GetValue(0) == 15);
    REQUIRE(result2.GetValue(0) == 10);
    REQUIRE(result1.GetValue(0) == 5);
}

TEST_CASE("CustomStack: Clear() empties stack and allows reuse", "[CustomStack]")
{
    Stack<int> stack;
    stack.Push(10);
    stack.Push(20);
    stack.Push(30);
    
    stack.Clear();
    
    stack.Push(100);
    REQUIRE(stack.Pop() == 100);
}

TEST_CASE("CustomStack: LIFO order preserved with interleaved Push/Pop operations", "[CustomStack]")
{
    Stack<int> stack;
    stack.Push(1);
    stack.Push(2);
    REQUIRE(stack.Pop() == 2);
    stack.Push(3);
    stack.Push(4);
    REQUIRE(stack.Pop() == 4);
    REQUIRE(stack.Pop() == 3);
    REQUIRE(stack.Pop() == 1);
}

TEST_CASE("CustomStack: Single element Push and Pop works correctly", "[CustomStack]")
{
    Stack<int> stack;
    stack.Push(42);
    REQUIRE(stack.Pop() == 42);
}

TEST_CASE("CustomStack: Clear() resets stack after multiple pushes", "[CustomStack]")
{
    Stack<int> stack;
    for (int i = 0; i < 10; ++i)
        stack.Push(i);
    
    stack.Clear();
    
    stack.Push(999);
    REQUIRE(stack.Pop() == 999);
}
