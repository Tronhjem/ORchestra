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
            beginTest("NoteNumbers: Converts 'C#0' to MIDI note 13 correctly");

            std::string file{"a = C#0 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            DataUnit value = result.GetValue(0);
            expect(value == 13 /*C#0 == 13 */);
        }
        {
            beginTest("NoteNumbers: Converts 'Db3' to MIDI note 49 correctly");

            std::string file{"a = Db3 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            DataUnit value = result.GetValue(0);
            expect(value == 49 /*Db3 == 49 */);
        }
        {
            beginTest("NoteNumbers: Parses note in array '[G#5, G#5]' as MIDI 80 correctly");

            std::string file{"a = [G#5, G#5] \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            DataUnit value = result.GetValue(0);
            expect(value == 80 /*G#5 == 80*/);
        }
        {
            beginTest("NoteNumbers: Evaluates 'C0 + 1' in array correctly (result=13)");

            std::string file{"a = [C0 + 1, 0] \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            DataUnit value = result.GetValue(0);
            expect(value == 13);
        }
        {
            beginTest("NoteNumbers: Rejects invalid note 'Dw21312' (compilation fails)");

            std::string file{"a = Dw21312 \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
        {
            beginTest("NoteNumbers: Rejects incomplete note 'C' without octave (compilation fails)");

            std::string file{"a = C \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]) == false);
        }
    }
};
