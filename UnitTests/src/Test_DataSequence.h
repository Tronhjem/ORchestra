#pragma once

using namespace juce;
#include "VM.h"
#include "DataSequenceStep.h"
#include "Defines.h"

class Test_DataSequence  : public UnitTest
{
public:
    Test_DataSequence()  : UnitTest ("Test_DataSequence") {}
    
    void runTest() override
    {
        {
            beginTest ("Check that ran works in data sequence");
            
            std::string file {"a = [ran(50,60), 5, 0] \n test a"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) >= 50);
            expect (result.GetValue(0) <= 60);
        }
        {
            beginTest ("Basic Get in Data Sequence");
            
            std::string file {"a = [1,0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
        }
        {
            beginTest ("Check Data Step Get");
            
            uChar data[3] {1,0,0};
            DataSequenceStep dataSeqStep{data, 3};
            expect (dataSeqStep.GetValue(0) == 1);
        }
        {
            beginTest ("Sub Step steps");
            std::string file {"a = [[1,4,5],0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
            expect (result.GetValue(1) == 4);
            expect (result.GetValue(2) == 5);
        }
        {
            beginTest ("Sub step get equivalent value from other sequence");
            std::string file {"a = [[60,64],0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            const int otherIndexLength = 4;
            expect (result.GetEquivalentValueAtIndex(0, otherIndexLength) == 60);
            expect (result.GetEquivalentValueAtIndex(1, otherIndexLength) == 60);
            expect (result.GetEquivalentValueAtIndex(2, otherIndexLength) == 64);
            expect (result.GetEquivalentValueAtIndex(3, otherIndexLength) == 64);
        }
        {
            beginTest ("Sub step get equivalent value from other sequence");
            std::string file {"a = [[60,64],0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            const int otherIndexLength = 3;
            expect (result.GetEquivalentValueAtIndex(0, otherIndexLength) == 60);
            expect (result.GetEquivalentValueAtIndex(1, otherIndexLength) == 60);
            expect (result.GetEquivalentValueAtIndex(2, otherIndexLength) == 64);
        }
        {
            beginTest ("Sub Step steps");
            std::string file {"a = [[1,1,0],0,0] \n test a[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 1);
            expect (result.GetValue(1) == 1);
            expect (result.GetValue(2) == 0);
        }
        {
            beginTest ("Sub step addition off different lengths");
            
            uChar dataOne[2] {1, 1};
            uChar dataTwo[4] {1, 0, 1, 0};
            
            DataSequenceStep dataStepOne {dataOne, 2};
            DataSequenceStep dataStepTwo {dataTwo, 4};
            
            DataSequenceStep added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            expect( added.GetLength() == 4);
            
            expect( added.GetValue(0) == 2);
            expect( added.GetValue(1) == 1);
            expect( added.GetValue(2) == 2);
            expect( added.GetValue(3) == 1);
        }
        {
            beginTest ("Sub step addition of different lenths");
            
            uChar dataOne[2] {1, 1};
            uChar dataTwo[3] {1, 0, 1};
            
            DataSequenceStep dataStepOne {dataOne, 2};
            DataSequenceStep dataStepTwo {dataTwo, 3};
            
            DataSequenceStep added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            expect( added.GetLength() == 3);
            
            expect( added.GetValue(0) == 2);
            expect( added.GetValue(1) == 1);
            expect( added.GetValue(2) == 2);
        }
        {
            beginTest ("Sub step addition clamp");
            
            uChar dataOne[2] {127, 127};
            uChar dataTwo[4] {127, 0, 127, 0};
            
            DataSequenceStep dataStepOne {dataOne, 2};
            DataSequenceStep dataStepTwo {dataTwo, 4};
            
            DataSequenceStep added = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Add);
            expect( added.GetLength() == 4);
            
            expect( added.GetValue(0) == 127);
            expect( added.GetValue(1) == 127);
            expect( added.GetValue(2) == 127);
            expect( added.GetValue(3) == 127);
        }
        {
            beginTest ("Sub step subtraction");
                
            const int length = 3;
            uChar dataOne[length] {10, 10, 10};
            uChar dataTwo[length] {1, 0, 1};
            
            DataSequenceStep dataStepOne {dataOne, length};
            DataSequenceStep dataStepTwo {dataTwo, length};
            
            DataSequenceStep result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Subtract);
            expect(result.GetLength() == length);
            
            for(int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == dataOne[i] - dataTwo[i]);
            }
        }
        {
            beginTest ("Sub step multiplication");
            
            const int length = 3;
            uChar dataOne[length] {10, 10, 10};
            uChar dataTwo[length] {1, 0, 1};
            
            DataSequenceStep dataStepOne {dataOne, length};
            DataSequenceStep dataStepTwo {dataTwo, length};
            
            DataSequenceStep result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Multiply);
            expect(result.GetLength() == length);
            
            for(int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == dataOne[i] * dataTwo[i]);
            }
        }
        {
            beginTest ("Sub step division");
            
            const int length = 3;
            uChar dataOne[length] {20, 10, 10};
            uChar dataTwo[length] {2, 1, 2};
            
            DataSequenceStep dataStepOne {dataOne, length};
            DataSequenceStep dataStepTwo {dataTwo, length};
            
            DataSequenceStep result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, Divide);
            expect(result.GetLength() == length);
            
            for(int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == dataOne[i] / dataTwo[i]);
            }
        }
        {
            beginTest ("Sub step AND");
            
            const int length = 3;
            uChar dataOne[length] {10, 10, 10};
            uChar dataTwo[length] {1, 0, 1};
            
            DataSequenceStep dataStepOne {dataOne, length};
            DataSequenceStep dataStepTwo {dataTwo, length};
            
            DataSequenceStep result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryAND);
            expect(result.GetLength() == length);
            
            for(int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == ((dataOne[i] > 0) & (dataTwo[i] > 0)));
            }
        }
        {
            beginTest ("Sub step XOR");
            
            const int length = 3;
            uChar dataOne[length] {10, 10, 10};
            uChar dataTwo[length] {1, 0, 1};
            
            DataSequenceStep dataStepOne {dataOne, length};
            DataSequenceStep dataStepTwo {dataTwo, length};
            
            DataSequenceStep result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryXOR);
            expect(result.GetLength() == length);
            
            for(int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == ((dataOne[i] > 0) ^ (dataTwo[i] > 0)));
            }
        }
        {
            beginTest ("Sub step OR");
            
            const int length = 3;
            uChar dataOne[length] {10, 10, 10};
            uChar dataTwo[length] {1, 0, 1};
            
            DataSequenceStep dataStepOne {dataOne, length};
            DataSequenceStep dataStepTwo {dataTwo, length};
            
            DataSequenceStep result = dataStepOne.ApplySequenceWithOperation(dataStepTwo, BinaryOR);
            expect(result.GetLength() == length);
            
            for(int i = 0; i < length; ++i)
            {
                expect(result.GetValue(i) == ((dataOne[i] > 0) | (dataTwo[i] > 0)));
            }
        }
        {
            beginTest ("Sub Division Add with single digit");
            
            std::string file {"a = [[60,65,70], 0, 0] \n b = a + 10 \n test b[0]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 60 + 10);
            expect (result.GetValue(1) == 65 + 10);
            expect (result.GetValue(2) == 70 + 10);
        }
        {
            beginTest ("Sub Division Add with single digit");
            
            std::string file {"a = [[60,65,70], 5, 0] \n b = a[1] + 10 \n test b[1]"};
            VM vm;
            expect(vm.Prepare(&file[0]));
            
            StepData result = vm.GetTopStackValue();
            expect (result.GetValue(0) == 5 + 10);
        }
        
    }
};
