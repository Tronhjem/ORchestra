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
            beginTest ("Test Setting single var value");
            
            std::string file {"a = 74 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74);
        }
        {
            beginTest ("Test Setting single expression addition value");
            
            std::string file {"a = 74+1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74 + 1);
        }
        {
            beginTest ("Test Setting single expression subtraction value");
            
            std::string file {"a = 74-1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 74 - 1);
        }
        {
            beginTest ("Test Setting single expression multiplication value");
            
            std::string file {"a = 2*2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 2 * 2);
        }
        {
            beginTest ("Test Setting single expression multiplication value");
            
            std::string file {"a = 2/2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 2 / 2);
        }
        {
            beginTest ("Test Setting single expression math precedence");
            
            std::string file {"a = 2 + 2 * 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 12 /* (2 + 2) * 5 */);
        }
        {
            beginTest ("Test setting parenteses");
            
            std::string file {"a = (2) + 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 4);
        }
        {
            beginTest ("Test setting parenteses with more operators");
            
            std::string file {"a = (2 + 2) * 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 8);
        }
        {
            beginTest ("Test setting nesterd parenteses");
            
            std::string file {"a = ((2 - 1) + 2) * 2 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 6);
        }
        {
            beginTest ("Test Setting identifier from other identifier");
            
            std::string file {"a = 5 \n b = a \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 5);
        }
        {
            beginTest ("Test Setting identifier from other plus math identifier");
            
            std::string file {"a = 5 \n b = a + 2 * 5 \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
           
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 5 + (2 * 5));
        }
        {
            beginTest ("Test Setting getting array index from identifier");
            
            std::string file {"a = [64,65,63] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64);
        }
        {
            beginTest ("Test Setting getting array index from identifier");
            
            std::string file {"a = [64,65,63] \n b = a[0] + 2 \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64 + 2);
        }
        {
            beginTest ("Test Setting setting identifier with getting global index");
            
            // Global index when not running is 0
            std::string file {"a = [64,65,63] \n b = a \n test b"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 64);
        }
        
        {
            beginTest ("Test Logical AND operator with true");
            
            // Global index when not running is 0
            std::string file {"a = 64 & 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test Logical AND operator with zero");
            
            // Global index when not running is 0
            std::string file {"a = 64 & 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest ("Test Logical OR operator with true");
            
            // Global index when not running is 0
            std::string file {"a = 64 | 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test Logical OR operator with zero");
            
            // Global index when not running is 0
            std::string file {"a = 64 | 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test Logical XOR operator with 64 and 64");
            
            // Global index when not running is 0
            std::string file {"a = 64 ^ 64 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest ("Test Logical XOR operator with 1 and 0");
            
            // Global index when not running is 0
            std::string file {"a = 1 ^ 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test Logical XOR operator with 0 and 0");
            
            // Global index when not running is 0
            std::string file {"a = 0 ^ 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest ("Test > operator with 1 and 0");
            
            // Global index when not running is 0
            std::string file {"a = 1 > 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test > operator with 1 and 5");
            
            // Global index when not running is 0
            std::string file {"a = 1 > 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest ("Test >= operator with 1 and 1");
            
            // Global index when not running is 0
            std::string file {"a = 1 >= 1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test >= operator with 1 and 1");
            
            // Global index when not running is 0
            std::string file {"a = 1 >= 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test >= operator with 1 and 5");
            
            // Global index when not running is 0
            std::string file {"a = 1 >= 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        
        // Operator <
        {
            beginTest ("Test < operator with 1 and 0");
            
            // Global index when not running is 0
            std::string file {"a = 1 < 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest ("Test M operator with 1 and 5");
            
            // Global index when not running is 0
            std::string file {"a = 1 < 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test <= operator with 1 and 1");
            
            // Global index when not running is 0
            std::string file {"a = 1 <= 1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test <= operator with 1 and 5");
            
            // Global index when not running is 0
            std::string file {"a = 1 <= 5 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test <= operator with 1 and 0");
            
            // Global index when not running is 0
            std::string file {"a = 1 <= 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
        {
            beginTest ("Test != operator with 1 and 0");
            
            // Global index when not running is 0
            std::string file {"a = 1 != 0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Test != operator with 1 and 1");
            
            // Global index when not running is 0
            std::string file {"a = 1 != 1 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 0);
        }
    }
};
