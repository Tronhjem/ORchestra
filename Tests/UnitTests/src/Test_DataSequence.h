/*
 * Copyright (C) 2026 Christian Tronhjem
 *
 * This file is part of ORchestra.
 *
 * ORchestra is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORchestra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ORchestra. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "catch.hpp"

#include "VM.h"
#include "ErrorReporting.h"
#include "StepData.h"
#include "Defines.h"

using namespace ORchestra;
TEST_CASE("DataSequence: Evaluates ran(50,60) in array, result in range [50,60]", "[DataSequence]")
{
    std::string file{"a = [ran(30,60), 5, 0] \n test a"};

    for (int i = 0; i < 40; ++i)
    {
        ErrorReporting errorReporter;
    VM vm(errorReporter);
        REQUIRE(vm.Prepare(file));

        StepData result = vm.GetTopStackValue();
        REQUIRE(result.GetValue(0) >= 30);
        REQUIRE(result.GetValue(0) <= 60);
    }
}

TEST_CASE("DataSequence: Accesses array element 'a=[1,0,0], a[0]' correctly (result=1)", "[DataSequence]")
{
    std::string file{"a = [1,0,0] \n test a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

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
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
    REQUIRE(result.GetValue(1) == 4);
    REQUIRE(result.GetValue(2) == 5);
}

TEST_CASE("DataSequence: GetEquivalentValueAtIndex maps values to different sequence lengths (length=4)", "[DataSequence]")
{
    std::string file{"a = [[60,64],0,0] \n test a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

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
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    const int otherIndexLength = 3;
    REQUIRE(result.GetEquivalentValueAtIndex(0, otherIndexLength) == 60);
    REQUIRE(result.GetEquivalentValueAtIndex(1, otherIndexLength) == 60);
    REQUIRE(result.GetEquivalentValueAtIndex(2, otherIndexLength) == 64);
}

TEST_CASE("DataSequence: Accesses nested array element [1,1,0], returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[1,1,0],0,0] \n test a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 3);
    REQUIRE(result.GetValue(0) == 1);
    REQUIRE(result.GetValue(1) == 1);
    REQUIRE(result.GetValue(2) == 0);
}

TEST_CASE("DataSequence: Accesses nested array element [1,1,0] added with other, returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[1,1,0],0,0] \n b = [[1,1,0], 0 , 0] \n test a[0] + b[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 3);
    REQUIRE(result.GetValue(0) == 2);
    REQUIRE(result.GetValue(1) == 2);
    REQUIRE(result.GetValue(2) == 0);
}

TEST_CASE("DataSequence: Accesses nested array element [1,2,0] subtractd with other, returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[1,2,0],0,0] \n b = [[1,1,0], 0 , 0] \n test a[0] - b[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 3);
    REQUIRE(result.GetValue(0) == 0);
    REQUIRE(result.GetValue(1) == 1);
    REQUIRE(result.GetValue(2) == 0);
}

TEST_CASE("DataSequence: Accesses nested array element [2,4,0] divided with other, returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[2,4,0],0,0] \n b = [[2,2,0], 0 , 0] \n test a[0] / b[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 3);
    REQUIRE(result.GetValue(0) == 1);
    REQUIRE(result.GetValue(1) == 2);
    REQUIRE(result.GetValue(2) == 0);
}

TEST_CASE("DataSequence: Accesses nested array element [2,4,0] mutiplied with other, returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[2,4,0],0,0] \n b = [[2,2,0], 0 , 0] \n test a[0] * b[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 3);
    REQUIRE(result.GetValue(0) == 4);
    REQUIRE(result.GetValue(1) == 8);
    REQUIRE(result.GetValue(2) == 0);
}

TEST_CASE("DataSequence: Accesses nested array element [1,1,3] subtractd with other of different lengths, returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[1,2,3],0,0] \n b = [[1,1], 0 , 0] \n test a[0] - b[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 3);

    REQUIRE(result.GetValue(0) == 0);
    REQUIRE(result.GetValue(1) == 1);
    REQUIRE(result.GetValue(2) == 2);
}

TEST_CASE("DataSequence: Accesses nested array element [3,1] subtractd with other of different lengths, returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[1,2,3],0,0] \n b = [[3,1], 0 , 0] \n test b[0] - a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 3);

    REQUIRE(result.GetValue(0) == 2);
    REQUIRE(result.GetValue(1) == 1);
    REQUIRE(result.GetValue(2) == static_cast<DataUnit>(-2)); // 1 - 3 = -2, no longer clamped to 0
}

TEST_CASE("DataSequence: Accesses nested array element [3,5] subtractd with other of different lengths, returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[1,2,3,4],0,0] \n b = [[3,5], 0 , 0] \n test b[0] - a[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 4);

    REQUIRE(result.GetValue(0) == 2);
    REQUIRE(result.GetValue(1) == 1);
    REQUIRE(result.GetValue(2) == 2);
    REQUIRE(result.GetValue(3) == 1);
}

TEST_CASE("DataSequence: Accesses nested array element [2,4,0] divided with other of different lengths, returns values correctly", "[DataSequence]")
{
    std::string file{"a = [[2,4,0],0,0] \n b = [[2,2], 0 , 0] \n test a[0] / b[0]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetLength() == 3);

    REQUIRE(result.GetValue(0) == 1);
    REQUIRE(result.GetValue(1) == 2);
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

TEST_CASE("DataSequence: Adds sequences with values exceeding 255 (DataUnit is int16_t)", "[DataSequence]")
{
    DataUnit dataOne[2]{130, 130};
    DataUnit dataTwo[4]{130, 0, 130, 0};

    StepData dataStepOne{dataOne, 2};
    StepData dataStepTwo{dataTwo, 4};

    StepData added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
    REQUIRE(added.GetLength() == 4);

    REQUIRE(added.GetValue(0) == 260);
    REQUIRE(added.GetValue(1) == 130);
    REQUIRE(added.GetValue(2) == 260);
    REQUIRE(added.GetValue(3) == 130);
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
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 60 + 10);
    REQUIRE(result.GetValue(1) == 65 + 10);
    REQUIRE(result.GetValue(2) == 70 + 10);
}

TEST_CASE("DataSequence: Adds scalar to nested array sub-division correctly (a[1]+10)", "[DataSequence]")
{
    std::string file{"a = [[60,65,70], 5, 0] \n b = a[1] + 10 \n test b[1]"};
    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(file));

    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5 + 10);
}
