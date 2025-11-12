#pragma once

using namespace juce;
#include "VM.h"

class Test_ArrayOperators  : public UnitTest
{
public:
    Test_ArrayOperators()  : UnitTest ("Test_ArrayOperators") {}
    
    void runTest() override
    {
        {
            beginTest("Array: Access first element 'a=[64,65,63], a[0]' evaluates to 64");
            
            std::string file {"a = [64,65,63] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64);
        }
        {
            beginTest("Array: Access element in expression 'a=[64,65,63], b=a[0]+2' evaluates to 66");
            
            std::string file {"a = [64,65,63] \n b = a[0] + 2 \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64 + 2);
        }
        {
            beginTest("Array: Assign array to variable 'a=[64,65,63], b=a' gets first element (64)");
            
            // Global index when not running is 0
            std::string file {"a = [64,65,63] \n b = a \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64);
        }
    }
};
