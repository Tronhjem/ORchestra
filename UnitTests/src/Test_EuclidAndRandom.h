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
            beginTest ("Test Euclid sequence wrong defined");
            
            std::string file {"a = euc() \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Test Euclid sequence wrong defined ");
            
            std::string file {"a = euc(2) \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Test Euclid sequence");
            
            std::string file {"a = euc(2,4) \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test Euclid sequence");
            
            std::string file {"a = euc(2,4) \n test a[1]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest ("Test Euclid sequence");
            
            std::string file {"a = euc(2,4) \n test a[2]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test Euclid sequence");
            
            std::string file {"a = euc(2,4) \n test a[3]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
    }
};
