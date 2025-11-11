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
            beginTest("Error: Detects unterminated array 'a = [2,' (compilation fails)");
            
            std::string file {"a = [2, \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects double comma in array 'a = [2,,]' (compilation fails)");
            
            std::string file {"a = [2,,] \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects missing comma in array 'a = [2 3,]' (compilation fails)");
            
            std::string file {"a = [2 3,] \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects incomplete expression 'a = 2 +' (compilation fails)");
            
            std::string file {"a = 2 + \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects incomplete expression 'a = (2 + 3) +' (compilation fails)");
            
            std::string file {"a = (2 + 3) + \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects operator without value 'a = ( + 3) +' (compilation fails)");
            
            std::string file {"a = ( + 3) + \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects empty parentheses 'a = () + 2' (compilation fails)");
            
            std::string file {"a = () + 2 \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects missing operator between values 'a = (2 - 3) 2' (compilation fails)");
            
            std::string file {"a = (2 - 3) 2 \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects operator at start 'a = + (2 - 3)' (compilation fails)");
            
            std::string file {"a = + (2 - 3) \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects unmatched opening parenthesis 'a = (2 - 3' (compilation fails)");
            
            std::string file {"a = (2 - 3 \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects unmatched closing parenthesis 'a = 2 - 3)' (compilation fails)");
            
            std::string file {"a = 2 - 3) \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects extra closing parenthesis 'a = (2 - 3) + 2)' (compilation fails)");
            
            std::string file {"a = (2 - 3) + 2) \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects missing comma in ran 'a = ran{2 3}' (compilation fails)");
            
            std::string file {"a = ran{2 3} \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects trailing comma in ran 'a = ran{2,}' (compilation fails)");
            
            std::string file {"a = ran{2,} \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects unmatched parenthesis in function 'print(2' (compilation fails)");
            
            std::string file {"print(2 \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects too many parameters in 'print(2,2)' (compilation fails)");
            
            std::string file {"print(2,2) \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects missing parameters in 'print()' (compilation fails)");
            
            std::string file {"print() \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Error: Detects invalid function name 'invalid()' (compilation fails)");
            
            std::string file {"invalid() \n"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
    }
};
