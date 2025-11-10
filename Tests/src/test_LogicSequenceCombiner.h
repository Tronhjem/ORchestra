#pragma once
using namespace juce;
#include "LogicSequenceCombiner.h"

constexpr int test_logicSequencecombinerLength = 8;
class Test_LogicSequenceCombiner  : public UnitTest
{
public:
    Test_LogicSequenceCombiner()  : UnitTest ("Test_LogicSequenceCombiner") {}
    
    void runTest() override
    {
//        uChar seq1[test_logicSequenceLength] = {1,0,1,0,1,0,1,0};
//        LogicSequence sequence1 {&seq1[0], test_logicSequencecombinerLength};
//
//        uChar seq2[test_logicSequenceLength] = {1,0,1,0,1,0,1,0};
//        LogicSequence sequence2 {&seq2[0], test_logicSequencecombinerLength};
//
//        uChar seq3[test_logicSequenceLength] = {1,0,1,0,1,0,1,0};
//        LogicSequence sequence3 {&seq2[0], test_logicSequencecombinerLength};
//
//        beginTest ("Single Logic Sequence");
//        {
//            LogicSequenceCombiner combiner {sequence1};
//
//            for (int i = 0; i < test_logicSequenceLength; ++i)
//            {
//                expect (combiner[i] == seq1[i]);
//                expect (combiner[i] == sequence1[i]);
//            }
//        }
//
//        beginTest ("Testing AND Operation");
//        {
//            LogicSequenceCombiner combiner {sequence1};
//            combiner.AddLogic(sequence2, Operation::AND);
//
//            int index = 3;
//            expect (combiner[index] == (seq1[index] & seq2[index]));
//
//            for (int i = 0; i < test_logicSequenceLength; ++i)
//            {
//                expect (combiner[i] == (seq1[i] & seq2[i]));
//                expect (combiner[i] == (sequence1[i] & sequence2[i]));
//            }
//
//        }
//
//        beginTest ("Testing OR Operation");
//        {
//            LogicSequenceCombiner combiner {sequence1};
//
//            combiner.AddLogic(sequence2, Operation::OR);
//            for (int i = 0; i < test_logicSequenceLength; ++i)
//            {
//                expect (combiner[i] == (seq1[i] | seq2[i]));
//                expect (combiner[i] == (sequence1[i] | sequence2[i]));
//            }
//        }
//
//        beginTest ("Testing XOR Operation");
//        {
//            LogicSequenceCombiner combiner {sequence1};
//
//            combiner.AddLogic(sequence2, Operation::XOR);
//            for (int i = 0; i < test_logicSequenceLength; ++i)
//            {
//                expect (combiner[i] == (seq1[i] ^ seq2[i]));
//                expect (combiner[i] == (sequence1[i] ^ sequence2[i]));
//            }
//        }
//
//        beginTest ("Testing AND and OR Operation");
//        {
//            LogicSequenceCombiner combiner {sequence1};
//            combiner.AddLogic(sequence2, Operation::AND);
//            combiner.AddLogic(sequence3, Operation::OR);
//
//            for (int i = 0; i < test_logicSequenceLength; ++i)
//            {
//                expect (combiner[i] == ((seq1[i] & seq2[i]) | seq3[i]));
//                expect (combiner[i] == ((sequence1[i] & sequence2[i]) | sequence3[i]));
//            }
//        }
    }
};
