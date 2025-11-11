#pragma once

using namespace juce;
#include "VM.h"

class Test_Compiler : public UnitTest
{
public:
    Test_Compiler() : UnitTest("Test_Compiler") {}
    
    void runTest() override
    {
        {
            beginTest("Compiler: Compiles nested array [[1,2],[3,4]], accessing a[0] returns [1,2]");
            
            std::string file = "a = [[1, 2], [3, 4]]\ntest a[0]";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetLength() == 2);
            expect(result.GetValue(0) == 1);
            expect(result.GetValue(1) == 2);
        }
        {
            beginTest("Compiler: Evaluates complex expression '(10 + 5) * 2 - 3' correctly (result=27)");
            
            std::string file = "a = (10 + 5) * 2 - 3\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == (10 + 5) * 2 - 3);
        }
        {
            beginTest("Compiler: Resolves variable reference in array 'b = [a, 10, 15]' where a=5");
            
            std::string file = "a = 5\nb = [a, 10, 15]\ntest b[0]";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 5);
        }
        {
            beginTest("Compiler: Handles multiple assignments 'a=10, b=20, c=a+b' (result=30)");
            
            std::string file = "a = 10\nb = 20\nc = a + b\ntest c";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 30);
        }
        {
            beginTest("Compiler: Evaluates division '100 / 5' correctly (result=20)");
            
            std::string file = "a = 100 / 5\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 20);
        }
        {
            beginTest("Compiler: Evaluates subtraction '17 - 12' correctly (result=5)");
            
            std::string file = "a = 17 - 12\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 5);
        }
        {
            beginTest("Compiler: Evaluates expressions in array '[2+2, 3*3, 10-5]' (first element=4)");
            
            std::string file = "a = [2+2, 3*3, 10-5]\ntest a[0]";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 4);
        }
        {
            beginTest("Compiler: Evaluates chained assignments 'a=10, b=a+5, c=b*2' (result=30)");
            
            std::string file = "a = 10\nb = a + 5\nc = b * 2\ntest c";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 30);
        }
        {
            beginTest("Compiler: Clamps negative result '0 - 10' to minimum (result=0)");
            
            std::string file = "a = 0 - 10\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 0); // Result is clamped to 0-127
        }
        {
            beginTest("Compiler: Clamps overflow '100 + 100' to maximum (result=127)");
            
            std::string file = "a = 100 + 100\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 127); // Clamped to max
        }
        {
            beginTest("Compiler: Accesses array element 'a=[10,20,30], b=a[1]' (result=20)");
            
            std::string file = "a = [10, 20, 30]\nb = a[1]\ntest b";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 20);
        }
        {
            beginTest("Compiler: Evaluates AND operation '1 & 1' correctly (result=1)");
            
            std::string file = "a = 1 & 1\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler: Evaluates XOR operation '1 ^ 1' correctly (result=0)");
            
            std::string file = "a = 1 ^ 1\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 0);
        }
        {
            beginTest("Compiler: Evaluates OR operation '0 | 1' correctly (result=1)");
            
            std::string file = "a = 0 | 1\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler: Evaluates greater-than comparison '10 > 5' correctly (result=1)");
            
            std::string file = "a = 10 > 5\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler: Evaluates less-than comparison '5 < 10' correctly (result=1)");
            
            std::string file = "a = 5 < 10\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler: Evaluates equality comparison '5 == 5' correctly (result=1)");
            
            std::string file = "a = 5 == 5\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler: Evaluates inequality comparison '5 != 10' correctly (result=1)");
            
            std::string file = "a = 5 != 10\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler: Evaluates complex expression '(5 + 3) * (10 - 2)' correctly (result=64)");
            
            std::string file = "a = (5 + 3) * (10 - 2)\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 64);
        }
    }
};
