#pragma once

using namespace juce;
#include "VM.h"

class Test_ArithmeticOperators  : public UnitTest
{
public:
    Test_ArithmeticOperators()  : UnitTest ("Test_ArithmeticOperators") {}
    
    void runTest() override
    {
        {
            beginTest("Arithmetic: Assigns single value 'a = 74' and retrieves it correctly");
            
            std::string file {"a = 74 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74);
        }
        {
            beginTest("Arithmetic: Addition 'a = 74+1' correctly evaluates to 75");
            
            std::string file {"a = 74+1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74 + 1);
        }
        {
            beginTest("Arithmetic: Subtraction 'a = 74-1' correctly evaluates to 73");
            
            std::string file {"a = 74-1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74 - 1);
        }
        {
            beginTest("Arithmetic: Multiplication 'a = 2*2' correctly evaluates to 4");
            
            std::string file {"a = 2*2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 2 * 2);
        }
        {
            beginTest("Arithmetic: Division 'a = 2/2' correctly evaluates to 1");
            
            std::string file {"a = 2/2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 2 / 2);
        }
        {
            beginTest("Arithmetic: Modulo '10 % 3' correctly evaluates to 1");
            
            std::string file {"a = 10 % 3 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 10 % 3);
        }
        {
            beginTest("Arithmetic: Modulo '7 % 2' correctly evaluates to 1");
            
            std::string file {"a = 7 % 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 7 % 2);
        }
        {
            beginTest("Arithmetic: Modulo '8 % 4' correctly evaluates to 0");
            
            std::string file {"a = 8 % 4 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 8 % 4);
        }
        {
            beginTest("Arithmetic: Modulo by zero '5 % 0' returns 0 safely");
            
            std::string file {"a = 5 % 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("Arithmetic: Operator precedence '2 + 2 * 5' evaluates to 12 (not 20)");
            
            std::string file {"a = 2 + 2 * 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 12 /* (2 + 2) * 5 */);
        }
        {
            beginTest("Arithmetic: Operator precedence with modulo '10 + 7 % 3' evaluates to 11");
            
            std::string file {"a = 10 + 7 % 3 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 10 + (7 % 3));
        }
        {
            beginTest("Arithmetic: Simple parentheses '(2) + 2' evaluates to 4");
            
            std::string file {"a = (2) + 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 4);
        }
        {
            beginTest("Arithmetic: Modulo in expression '10 % 3 + 2' evaluates to 3");
            
            std::string file {"a = 10 % 3 + 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == (10 % 3) + 2);
        }
        {
            beginTest("Arithmetic: Parentheses override precedence '(2 + 2) * 2' evaluates to 8");
            
            std::string file {"a = (2 + 2) * 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 8);
        }
        {
            beginTest("Arithmetic: Nested parentheses '((2 - 1) + 2) * 2' evaluates to 6");
            
            std::string file {"a = ((2 - 1) + 2) * 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 6);
        }
        {
            beginTest("Arithmetic: Variable assignment 'a=5, b=a' evaluates to 5");
            
            std::string file {"a = 5 \n b = a \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 5);
        }
        {
            beginTest("Arithmetic: Variable in expression 'a=5, b=a+2*5' evaluates to 15");
            
            std::string file {"a = 5 \n b = a + 2 * 5 \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
           
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 5 + (2 * 5));
        }
    }
};
