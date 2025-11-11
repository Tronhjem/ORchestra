#pragma once

using namespace juce;
#include "VM.h"
#include "StepData.h"
#include "Defines.h"

class Test_DataSequence : public UnitTest
{
public:
    Test_DataSequence() : UnitTest("Test_DataSequence") {}

    void runTest() override
    {
        {
            beginTest("DataSequence: Evaluates ran(50,60) in array, result in range [50,60]");

            std::string file{"a = [ran(50,60), 5, 0] \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) >= 50);
            expect(result.GetValue(0) <= 60);
        }
        {
            beginTest("DataSequence: Accesses array element 'a=[1,0,0], a[0]' correctly (result=1)");

            std::string file{"a = [1,0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
        }
        {
            beginTest("DataSequence: StepData.GetValue(0) returns first element correctly");

            DataUnit data[3]{1, 0, 0};
            StepData dataSeqStep{data, 3};
            expect(dataSeqStep.GetValue(0) == 1);
        }
        {
            beginTest("DataSequence: Accesses nested array element, returns values correctly");
            std::string file{"a = [[1,4,5],0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
            expect(result.GetValue(1) == 4);
            expect(result.GetValue(2) == 5);
        }
        {
            beginTest("DataSequence: GetEquivalentValueAtIndex maps values to different sequence lengths");
            std::string file{"a = [[60,64],0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            const int otherIndexLength = 4;
            expect(result.GetEquivalentValueAtIndex(0, otherIndexLength) == 60);
            expect(result.GetEquivalentValueAtIndex(1, otherIndexLength) == 60);
            expect(result.GetEquivalentValueAtIndex(2, otherIndexLength) == 64);
            expect(result.GetEquivalentValueAtIndex(3, otherIndexLength) == 64);
        }
        {
            beginTest("DataSequence: GetEquivalentValueAtIndex maps values to different sequence lengths");
            std::string file{"a = [[60,64],0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            const int otherIndexLength = 3;
            expect(result.GetEquivalentValueAtIndex(0, otherIndexLength) == 60);
            expect(result.GetEquivalentValueAtIndex(1, otherIndexLength) == 60);
            expect(result.GetEquivalentValueAtIndex(2, otherIndexLength) == 64);
        }
        {
            beginTest("DataSequence: Accesses nested array element, returns values correctly");
            std::string file{"a = [[1,1,0],0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 1);
            expect(result.GetValue(1) == 1);
            expect(result.GetValue(2) == 0);
        }
        {
            beginTest("DataSequence: Adds sequences of length 2 and 4, result has length 4");

            DataUnit dataOne[2]{1, 1};
            DataUnit dataTwo[4]{1, 0, 1, 0};

            StepData dataStepOne{dataOne, 2};
            StepData dataStepTwo{dataTwo, 4};

            StepData added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            expect(added.GetLength() == 4);

            expect(added.GetValue(0) == 2);
            expect(added.GetValue(1) == 1);
            expect(added.GetValue(2) == 2);
            expect(added.GetValue(3) == 1);
        }
        {
            beginTest("DataSequence: Adds sequences of length 2 and 3, result has length 3");

            DataUnit dataOne[2]{1, 1};
            DataUnit dataTwo[3]{1, 0, 1};

            StepData dataStepOne{dataOne, 2};
            StepData dataStepTwo{dataTwo, 3};

            StepData added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            expect(added.GetLength() == 3);

            expect(added.GetValue(0) == 2);
            expect(added.GetValue(1) == 1);
            expect(added.GetValue(2) == 2);
        }
        {
            beginTest("DataSequence: Clamps addition overflow to max 127");

            DataUnit dataOne[2]{127, 127};
            DataUnit dataTwo[4]{127, 0, 127, 0};

            StepData dataStepOne{dataOne, 2};
            StepData dataStepTwo{dataTwo, 4};

            StepData added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            expect(added.GetLength() == 4);

            expect(added.GetValue(0) == 127);
            expect(added.GetValue(1) == 127);
            expect(added.GetValue(2) == 127);
            expect(added.GetValue(3) == 127);
        }
        {
            beginTest("DataSequence: Subtracts sequences element-wise correctly");

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Subtract);
            expect(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == dataOne[i] - dataTwo[i]);
            }
        }
        {
            beginTest("DataSequence: Multiplies sequences element-wise correctly");

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Multiply);
            expect(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == dataOne[i] * dataTwo[i]);
            }
        }
        {
            beginTest("DataSequence: Divides sequences element-wise correctly");

            const int length = 3;
            DataUnit dataOne[length]{20, 10, 10};
            DataUnit dataTwo[length]{2, 1, 2};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Divide);
            expect(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == dataOne[i] / dataTwo[i]);
            }
        }
        {
            beginTest("DataSequence: Applies AND operation to sequences element-wise");

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryAND);
            expect(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == ((dataOne[i] > 0) & (dataTwo[i] > 0)));
            }
        }
        {
            beginTest("DataSequence: Applies XOR operation to sequences element-wise");

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryXOR);
            expect(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == ((dataOne[i] > 0) ^ (dataTwo[i] > 0)));
            }
        }
        {
            beginTest("DataSequence: Applies OR operation to sequences element-wise");

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryOR);
            expect(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == ((dataOne[i] > 0) | (dataTwo[i] > 0)));
            }
        }
        {
            beginTest("DataSequence: Adds scalar to nested array sub-division correctly");

            std::string file{"a = [[60,65,70], 0, 0] \n b = a + 10 \n test b[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 60 + 10);
            expect(result.GetValue(1) == 65 + 10);
            expect(result.GetValue(2) == 70 + 10);
        }
        {
            beginTest("DataSequence: Adds scalar to nested array sub-division correctly");

            std::string file{"a = [[60,65,70], 5, 0] \n b = a[1] + 10 \n test b[1]"};
            VM vm;
            expect(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            expect(result.GetValue(0) == 5 + 10);
        }
    }
};
