#pragma once

using namespace juce;
#include "VM.h"

class Test_NoteNumbers : public UnitTest
{
public:
    Test_NoteNumbers() : UnitTest("Test_NoteNumbers") {}

    void runTest() override
    {
        {
            beginTest("Compiles with Single Note sharp number");

            std::string file{"a = C#0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            DataUnit value = result.GetValue(0);
            std::cout << (int)value << std::endl;
            expect(value == (DataUnit)13 /*C#0 == 13 */);
        }
        {
            beginTest("Compiles with Single Note flat number");

            std::string file{"a = Db3 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            DataUnit value = result.GetValue(0);
            expect(value == 49 /*Db3 == 49 */);
        }
        {
            beginTest("Compiles as part of array");

            std::string file{"a = [G#5, G#5] \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            DataUnit value = result.GetValue(0);
            expect(value == 80 /*G#5 == 80*/);
        }
        {
            beginTest("Compiles as part of array and expression");

            std::string file{"a = [C0 + 1, 0] \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            DataUnit value = result.GetValue(0);
            expect(value == 13);
        }
        {
            beginTest("Doesn't compile wrong Note Information");

            std::string file{"a = Dw21312 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("Require Longer note");

            std::string file{"a = C \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
    }
};
