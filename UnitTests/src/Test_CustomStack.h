#pragma once

using namespace juce;
#include "CustomStack.h"
#include "StepData.h"

class Test_CustomStack : public UnitTest
{
public:
    Test_CustomStack() : UnitTest("Test_CustomStack") {}
    
    void runTest() override
    {
        {
            beginTest("Stack Push and Pop with integers");
            
            Stack<int> stack;
            stack.Push(10);
            stack.Push(20);
            stack.Push(30);
            
            expect(stack.Pop() == 30);
            expect(stack.Pop() == 20);
            expect(stack.Pop() == 10);
        }
        {
            beginTest("Stack Push and Pop with StepData");
            
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
            
            expect(result3.GetValue(0) == 15);
            expect(result2.GetValue(0) == 10);
            expect(result1.GetValue(0) == 5);
        }
        {
            beginTest("Stack Clear");
            
            Stack<int> stack;
            stack.Push(10);
            stack.Push(20);
            stack.Push(30);
            
            stack.Clear();
            
            stack.Push(100);
            expect(stack.Pop() == 100);
        }
        {
            beginTest("Stack maintains order with multiple operations");
            
            Stack<int> stack;
            stack.Push(1);
            stack.Push(2);
            expect(stack.Pop() == 2);
            stack.Push(3);
            stack.Push(4);
            expect(stack.Pop() == 4);
            expect(stack.Pop() == 3);
            expect(stack.Pop() == 1);
        }
        {
            beginTest("Stack with single element");
            
            Stack<int> stack;
            stack.Push(42);
            expect(stack.Pop() == 42);
        }
        {
            beginTest("Stack Clear and reuse");
            
            Stack<int> stack;
            for (int i = 0; i < 10; ++i)
                stack.Push(i);
            
            stack.Clear();
            
            stack.Push(999);
            expect(stack.Pop() == 999);
        }
    }
};
