#pragma once
#include "../catch.hpp"
#include "VM.h"
#include "StepData.h"
#include "Defines.h"

using namespace ORchestra;
TEST_CASE("DataSequence: Evaluates ran(50,60) in array, result in range [50,60]", "[DataSequence]")
{

            std::string file{"a = [ran(50,60), 5, 0] \n test a"};
            VM vm;
            REQUIRE(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            REQUIRE(result.GetValue(0) >= 50);
            REQUIRE(result.GetValue(0) <= 60);
}

TEST_CASE("DataSequence: Accesses array element 'a=[1,0,0], a[0]' correctly (result=1)", "[DataSequence]")
{

            std::string file{"a = [1,0,0] \n test a[0]"};
            VM vm;
            REQUIRE(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("DataSequence: StepData.GetValue(0) returns first element correctly", "[DataSequence]")
{

            DataUnit data[3]{1, 0, 0};
            StepData dataSeqStep{data, 3};
            REQUIRE(dataSeqStep.GetValue(0) == 1);
}

TEST_CASE("DataSequence: Accesses nested array element [1,4,5], returns values correctly", "[DataSequence]")
{
            std::string file{"a = [[1,4,5],0,0] \n test a[0]"};
            VM vm;
            REQUIRE(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            REQUIRE(result.GetValue(0) == 1);
            REQUIRE(result.GetValue(1) == 4);
            REQUIRE(result.GetValue(2) == 5);
}

TEST_CASE("DataSequence: GetEquivalentValueAtIndex maps values to different sequence lengths (length=4)", "[DataSequence]")
{
            std::string file{"a = [[60,64],0,0] \n test a[0]"};
            VM vm;
            REQUIRE(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            const int otherIndexLength = 4;
            REQUIRE(result.GetEquivalentValueAtIndex(0, otherIndexLength) == 60);
            REQUIRE(result.GetEquivalentValueAtIndex(1, otherIndexLength) == 60);
            REQUIRE(result.GetEquivalentValueAtIndex(2, otherIndexLength) == 64);
            REQUIRE(result.GetEquivalentValueAtIndex(3, otherIndexLength) == 64);
}

TEST_CASE("DataSequence: GetEquivalentValueAtIndex maps values to different sequence lengths (length=3)", "[DataSequence]")
{
            std::string file{"a = [[60,64],0,0] \n test a[0]"};
            VM vm;
            REQUIRE(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            const int otherIndexLength = 3;
            REQUIRE(result.GetEquivalentValueAtIndex(0, otherIndexLength) == 60);
            REQUIRE(result.GetEquivalentValueAtIndex(1, otherIndexLength) == 60);
            REQUIRE(result.GetEquivalentValueAtIndex(2, otherIndexLength) == 64);
}

TEST_CASE("DataSequence: Accesses nested array element [1,1,0], returns values correctly", "[DataSequence]")
{
            std::string file{"a = [[1,1,0],0,0] \n test a[0]"};
            VM vm;
            REQUIRE(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            REQUIRE(result.GetValue(0) == 1);
            REQUIRE(result.GetValue(1) == 1);
            REQUIRE(result.GetValue(2) == 0);
}

TEST_CASE("DataSequence: Adds sequences of length 2 and 4, result has length 4", "[DataSequence]")
{

            DataUnit dataOne[2]{1, 1};
            DataUnit dataTwo[4]{1, 0, 1, 0};

            StepData dataStepOne{dataOne, 2};
            StepData dataStepTwo{dataTwo, 4};

            StepData added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            REQUIRE(added.GetLength() == 4);

            REQUIRE(added.GetValue(0) == 2);
            REQUIRE(added.GetValue(1) == 1);
            REQUIRE(added.GetValue(2) == 2);
            REQUIRE(added.GetValue(3) == 1);
}

TEST_CASE("DataSequence: Adds sequences of length 2 and 3, result has length 3", "[DataSequence]")
{

            DataUnit dataOne[2]{1, 1};
            DataUnit dataTwo[3]{1, 0, 1};

            StepData dataStepOne{dataOne, 2};
            StepData dataStepTwo{dataTwo, 3};

            StepData added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            REQUIRE(added.GetLength() == 3);

            REQUIRE(added.GetValue(0) == 2);
            REQUIRE(added.GetValue(1) == 1);
            REQUIRE(added.GetValue(2) == 2);
}

TEST_CASE("DataSequence: Clamps addition overflow to max 127", "[DataSequence]")
{

            DataUnit dataOne[2]{127, 127};
            DataUnit dataTwo[4]{127, 0, 127, 0};

            StepData dataStepOne{dataOne, 2};
            StepData dataStepTwo{dataTwo, 4};

            StepData added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            REQUIRE(added.GetLength() == 4);

            REQUIRE(added.GetValue(0) == 127);
            REQUIRE(added.GetValue(1) == 127);
            REQUIRE(added.GetValue(2) == 127);
            REQUIRE(added.GetValue(3) == 127);
}

TEST_CASE("DataSequence: Subtracts sequences element-wise correctly", "[DataSequence]")
{

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Subtract);
            REQUIRE(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                REQUIRE(result.GetValue(i) == dataOne[i] - dataTwo[i]);
            }
}

TEST_CASE("DataSequence: Multiplies sequences element-wise correctly", "[DataSequence]")
{

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Multiply);
            REQUIRE(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                REQUIRE(result.GetValue(i) == dataOne[i] * dataTwo[i]);
            }
}

TEST_CASE("DataSequence: Divides sequences element-wise correctly", "[DataSequence]")
{

            const int length = 3;
            DataUnit dataOne[length]{20, 10, 10};
            DataUnit dataTwo[length]{2, 1, 2};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Divide);
            REQUIRE(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                REQUIRE(result.GetValue(i) == dataOne[i] / dataTwo[i]);
            }
}

TEST_CASE("DataSequence: Applies AND operation to sequences element-wise", "[DataSequence]")
{

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryAND);
            REQUIRE(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                REQUIRE(result.GetValue(i) == ((dataOne[i] > 0) & (dataTwo[i] > 0)));
            }
}

TEST_CASE("DataSequence: Applies XOR operation to sequences element-wise", "[DataSequence]")
{

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryXOR);
            REQUIRE(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                REQUIRE(result.GetValue(i) == ((dataOne[i] > 0) ^ (dataTwo[i] > 0)));
            }
}

TEST_CASE("DataSequence: Applies OR operation to sequences element-wise", "[DataSequence]")
{

            const int length = 3;
            DataUnit dataOne[length]{10, 10, 10};
            DataUnit dataTwo[length]{1, 0, 1};

            StepData dataStepOne{dataOne, length};
            StepData dataStepTwo{dataTwo, length};

            StepData result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryOR);
            REQUIRE(result.GetLength() == length);

            for (int i = 0; i < length; ++i)
            {
                REQUIRE(result.GetValue(i) == ((dataOne[i] > 0) | (dataTwo[i] > 0)));
            }
}

TEST_CASE("DataSequence: Adds scalar to nested array sub-division correctly (a+10)", "[DataSequence]")
{

            std::string file{"a = [[60,65,70], 0, 0] \n b = a + 10 \n test b[0]"};
            VM vm;
            REQUIRE(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            REQUIRE(result.GetValue(0) == 60 + 10);
            REQUIRE(result.GetValue(1) == 65 + 10);
            REQUIRE(result.GetValue(2) == 70 + 10);
}

TEST_CASE("DataSequence: Adds scalar to nested array sub-division correctly (a[1]+10)", "[DataSequence]")
{

            std::string file{"a = [[60,65,70], 5, 0] \n b = a[1] + 10 \n test b[1]"};
            VM vm;
            REQUIRE(vm.Prepare(&file[0]));

            StepData result = vm.GetTopStackValue();
            REQUIRE(result.GetValue(0) == 5 + 10);
}
