#pragma once

using namespace juce;
#include "VM.h"

class Test_Operators  : public UnitTest
{
public:
    Test_Operators()  : UnitTest ("Test_OpCodeResults") {}
    
    void runTest() override
    {
        {
            beginTest("VM: Assigns single value 'a = 74' and retrieves it correctly");
            
            std::string file {"a = 74 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74);
        }
        {
            beginTest("VM: Evaluates addition expression 'a = 74+1' correctly (result=75)");
            
            std::string file {"a = 74+1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74 + 1);
        }
        {
            beginTest("VM: Evaluates subtraction expression 'a = 74-1' correctly (result=73)");
            
            std::string file {"a = 74-1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74 - 1);
        }
        {
            beginTest("VM: Evaluates expression correctly (multiplication or division)");
            
            std::string file {"a = 2*2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 2 * 2);
        }
        {
            beginTest("VM: Evaluates expression correctly (multiplication or division)");
            
            std::string file {"a = 2/2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 2 / 2);
        }
        {
            beginTest("VM: Evaluates modulo operation '10 % 3' correctly (result=1)");
            
            std::string file {"a = 10 % 3 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 10 % 3);
        }
        {
            beginTest("VM: Evaluates modulo operation '7 % 2' correctly (result=1)");
            
            std::string file {"a = 7 % 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 7 % 2);
        }
        {
            beginTest("VM: Evaluates modulo operation '8 % 4' correctly (result=0)");
            
            std::string file {"a = 8 % 4 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 8 % 4);
        }
        {
            beginTest("VM: Handles modulo by zero safely '5 % 0' (result=0)");
            
            std::string file {"a = 5 % 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("VM: Respects operator precedence in '2 + 2 * 5' (result=12, not 20)");
            
            std::string file {"a = 2 + 2 * 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 12 /* (2 + 2) * 5 */);
        }
        {
            beginTest("VM: Evaluates simple parentheses expression '(2) + 2' correctly (result=4)");
            
            std::string file {"a = (2) + 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 4);
        }
        {
            beginTest("VM: Evaluates modulo in expression '10 % 3 + 2' correctly (result=3)");
            
            std::string file {"a = 10 % 3 + 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == (10 % 3) + 2);
        }
        {
            beginTest("VM: Evaluates parentheses with precedence '(2 + 2) * 2' correctly (result=8)");
            
            std::string file {"a = (2 + 2) * 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 8);
        }
        {
            beginTest("VM: Evaluates nested parentheses '((2 - 1) + 2) * 2' correctly (result=6)");
            
            std::string file {"a = ((2 - 1) + 2) * 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 6);
        }
        {
            beginTest("VM: Assigns variable to another 'a=5, b=a' correctly (result=5)");
            
            std::string file {"a = 5 \n b = a \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 5);
        }
        {
            beginTest("VM: Uses variable in expression 'a=5, b=a+2*5' correctly (result=15)");
            
            std::string file {"a = 5 \n b = a + 2 * 5 \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
           
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 5 + (2 * 5));
        }
        {
            beginTest("VM: Accesses array element correctly");
            
            std::string file {"a = [64,65,63] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64);
        }
        {
            beginTest("VM: Accesses array element correctly");
            
            std::string file {"a = [64,65,63] \n b = a[0] + 2 \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64 + 2);
        }
        {
            beginTest("VM: Assigns array to variable and retrieves first element correctly");
            
            // Global index when not running is 0
            std::string file {"a = [64,65,63] \n b = a \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64);
        }
        
        {
            beginTest("VM: Evaluates AND operation with non-zero values (result=1 for true)");
            
            // Global index when not running is 0
            std::string file {"a = 64 & 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates AND operation with zero (result=0 for false)");
            
            // Global index when not running is 0
            std::string file {"a = 64 & 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("VM: Evaluates OR operation with non-zero values (result=1 for true)");
            
            // Global index when not running is 0
            std::string file {"a = 64 | 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates OR operation with one zero (result=1 for true)");
            
            // Global index when not running is 0
            std::string file {"a = 64 | 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates XOR operation with same values (result=0)");
            
            // Global index when not running is 0
            std::string file {"a = 64 ^ 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("VM: Evaluates XOR operation with different values (result=1)");
            
            // Global index when not running is 0
            std::string file {"a = 1 ^ 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates XOR operation with both zero (result=0)");
            
            // Global index when not running is 0
            std::string file {"a = 0 ^ 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("VM: Evaluates greater-than comparison correctly (1 > 0 = true)");
            
            // Global index when not running is 0
            std::string file {"a = 1 > 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates greater-than comparison correctly (1 > 5 = false)");
            
            // Global index when not running is 0
            std::string file {"a = 1 > 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("VM: Evaluates greater-or-equal comparison correctly (equal values)");
            
            // Global index when not running is 0
            std::string file {"a = 1 >= 1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates greater-or-equal comparison correctly (equal values)");
            
            // Global index when not running is 0
            std::string file {"a = 1 >= 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates greater-or-equal comparison correctly (less than)");
            
            // Global index when not running is 0
            std::string file {"a = 1 >= 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        
        // Operator <
        {
            beginTest("VM: Evaluates less-than comparison correctly (1 < 0 = false)");
            
            // Global index when not running is 0
            std::string file {"a = 1 < 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("VM: Evaluates less-than comparison correctly (1 < 5 = true)");
            
            // Global index when not running is 0
            std::string file {"a = 1 < 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates less-or-equal comparison correctly (equal values)");
            
            // Global index when not running is 0
            std::string file {"a = 1 <= 1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates less-or-equal comparison correctly (less than)");
            
            // Global index when not running is 0
            std::string file {"a = 1 <= 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates less-or-equal comparison correctly (greater than)");
            
            // Global index when not running is 0
            std::string file {"a = 1 <= 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest("VM: Evaluates inequality comparison correctly (different values = true)");
            
            // Global index when not running is 0
            std::string file {"a = 1 != 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest("VM: Evaluates inequality comparison correctly (same values = false)");
            
            // Global index when not running is 0
            std::string file {"a = 1 != 1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
    }
};
