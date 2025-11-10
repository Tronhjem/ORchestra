#pragma once

using namespace juce;
#include "VM.h"

class Test_ErrorHandling  : public UnitTest
{
public:
    Test_ErrorHandling()  : UnitTest ("Test_ErrorHandling") {}
    
    void runTest() override
    {
        {
            beginTest ("Array: No end to array");
            
            std::string file {"a = [2, \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Array: two commas in a row");
            
            std::string file {"a = [2,,] \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Array: Number without comma seperation");
            
            std::string file {"a = [2 3,] \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: value and operator");
            
            std::string file {"a = 2 + \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: value and operator and parenteses");
            
            std::string file {"a = (2 + 3) + \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: parenteses again");
            
            std::string file {"a = ( + 3) + \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: parenteses again");
            
            std::string file {"a = () + 2 \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: parenteses again");
            
            std::string file {"a = (2 - 3) 2 \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: parenteses again");
            
            std::string file {"a = + (2 - 3) \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: matching parenteses");
            
            std::string file {"a = (2 - 3 \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: matching parenteses");
            
            std::string file {"a = 2 - 3) \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Expression: matching parenteses");
            
            std::string file {"a = (2 - 3) + 2) \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Random: two numbers no comma");
            
            std::string file {"a = ran{2 3} \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Random: two numbers no comma");
            
            std::string file {"a = ran{2,} \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Function: no matching parenteses");
            
            std::string file {"print(2 \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Function: too many params");
            
            std::string file {"print(2,2) \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Function: too little params");
            
            std::string file {"print() \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest ("Function: not valid function");
            
            std::string file {"invalid() \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
    }
};
