#pragma once

using namespace juce;
#include "VM.h"

class Test_EuclidAndRandom  : public UnitTest
{
public:
    Test_EuclidAndRandom()  : UnitTest ("Test_OpCodeResults") {}
    
    void runTest() override
    {
        {
            beginTest("Euclidean: Rejects 'euc()' with no parameters (compilation fails)");
            
            std::string file {"a = euc() \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Euclidean: Rejects 'euc(2)' with only one parameter (compilation fails)");
            
            std::string file {"a = euc(2) \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Euclidean: Generates 'euc(2,4)' sequence, first element is 1");
            
            std::string file {"a = euc(2,4) \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("Euclidean: Generates 'euc(2,4)' sequence, second element is 0");
            
            std::string file {"a = euc(2,4) \n test a[1]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("Euclidean: Generates 'euc(2,4)' sequence, third element is 1");
            
            std::string file {"a = euc(2,4) \n test a[2]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("Euclidean: Generates 'euc(2,4)' sequence, fourth element is 0");
            
            std::string file {"a = euc(2,4) \n test a[3]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
    }
};
