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
            beginTest("Compiler handles nested arrays");
            
            std::string file = "a = [[1, 2], [3, 4]]\ntest a[0]";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetLength() == 2);
            expect(result.GetValue(0) == 1);
            expect(result.GetValue(1) == 2);
        }
        {
            beginTest("Compiler handles complex expression");
            
            std::string file = "a = (10 + 5) * 2 - 3\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == (10 + 5) * 2 - 3);
        }
        {
            beginTest("Compiler handles variable references in arrays");
            
            std::string file = "a = 5\nb = [a, 10, 15]\ntest b[0]";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 5);
        }
        {
            beginTest("Compiler handles multiple variable assignments");
            
            std::string file = "a = 10\nb = 20\nc = a + b\ntest c";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 30);
        }
        {
            beginTest("Compiler handles division by constant");
            
            std::string file = "a = 100 / 5\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 20);
        }
        {
            beginTest("Compiler handles modulo-like operations with subtraction");
            
            std::string file = "a = 17 - 12\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 5);
        }
        {
            beginTest("Compiler handles array with expressions");
            
            std::string file = "a = [2+2, 3*3, 10-5]\ntest a[0]";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 4);
        }
        {
            beginTest("Compiler handles chained operations");
            
            std::string file = "a = 10\nb = a + 5\nc = b * 2\ntest c";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 30);
        }
        {
            beginTest("Compiler handles negative numbers");
            
            std::string file = "a = 0 - 10\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 0); // Result is clamped to 0-127
        }
        {
            beginTest("Compiler handles value clamping at max");
            
            std::string file = "a = 100 + 100\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 127); // Clamped to max
        }
        {
            beginTest("Compiler handles array indexing with variable");
            
            std::string file = "a = [10, 20, 30]\nb = a[1]\ntest b";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 20);
        }
        {
            beginTest("Compiler handles complex boolean logic");
            
            std::string file = "a = 1 & 1\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler handles XOR logic");
            
            std::string file = "a = 1 ^ 1\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 0);
        }
        {
            beginTest("Compiler handles OR logic");
            
            std::string file = "a = 0 | 1\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler handles comparison greater than");
            
            std::string file = "a = 10 > 5\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler handles comparison less than");
            
            std::string file = "a = 5 < 10\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler handles equality comparison");
            
            std::string file = "a = 5 == 5\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler handles inequality comparison");
            
            std::string file = "a = 5 != 10\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("Compiler handles mixed operators with parentheses");
            
            std::string file = "a = (5 + 3) * (10 - 2)\ntest a";
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 64);
        }
    }
};
