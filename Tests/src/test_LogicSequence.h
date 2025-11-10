#pragma once
using namespace juce;
#include "LogicSequence.h"

constexpr int test_logicSequenceLength = 8;
class Test_LogicSequence  : public UnitTest
{
public:
    Test_LogicSequence()  : UnitTest ("Test_LogicSequence") {}
 
    void runTest() override
    {
//        // Init
//        uChar seq[test_logicSequenceLength] = {1,0,1,0,1,0,1,0};
//        LogicSequence sequence {&seq[0], test_logicSequenceLength};
//
//        beginTest ("Length Getter Getter");
//        expect (sequence.GetLength() == test_logicSequenceLength);
//
//        beginTest ("Get Sequence entry");
//        for (int i = 0; i < test_logicSequenceLength; ++i)
//        {
//            expect (sequence[i] == seq[i]);
//        }
//
//        beginTest ("Get Sequence Wrap Around");
//        expect (sequence[test_logicSequenceLength + 3] == seq[(test_logicSequenceLength + 3) % test_logicSequenceLength]);
//
//        for (int i = 0; i < test_logicSequenceLength * 3; ++i)
//        {
//            expect (sequence[i] == seq[i % test_logicSequenceLength]);
//        }
    }
};
