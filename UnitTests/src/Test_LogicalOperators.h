#pragma once

using namespace juce;
#include "VM.h"

class Test_LogicalOperators  : public UnitTest
{
public:
    Test_LogicalOperators()  : UnitTest ("Test_LogicalOperators") {}
    
    void runTest() override
    {
        {
            beginTest("Logical: AND operation with non-zero values '64 & 64' evaluates to 1 (true)");
            
            std::string file {"a = 64 & 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("Logical: AND operation with zero '64 & 0' evaluates to 0 (false)");
            
            std::string file {"a = 64 & 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("Logical: OR operation with non-zero values '64 | 64' evaluates to 1 (true)");
            
            std::string file {"a = 64 | 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("Logical: OR operation with one zero '64 | 0' evaluates to 1 (true)");
            
            std::string file {"a = 64 | 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("Logical: XOR operation with same values '64 ^ 64' evaluates to 0");
            
            std::string file {"a = 64 ^ 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("Logical: XOR operation with different values '1 ^ 0' evaluates to 1");
            
            std::string file {"a = 1 ^ 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("Logical: XOR operation with both zero '0 ^ 0' evaluates to 0");
            
            std::string file {"a = 0 ^ 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
    }
};
