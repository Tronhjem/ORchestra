//OLD, JUST KEEPING FOR ARCHIVE

//#include <cassert>
//#include "LogicSequenceCombiner.h"
//
//LogicSequenceCombiner::LogicSequenceCombiner(LogicSequence logicSeq)
//{
//    mLogicSequences.emplace_back(logicSeq);
//}
//
//void LogicSequenceCombiner::AddLogic(LogicSequence logicSeq, Operation operation)
//{
//    mLogicSequences.emplace_back(logicSeq);
//    mOperations.emplace_back(operation);
//}
//
//const uChar LogicSequenceCombiner::operator [](const int index) const
//{
//#if _DEBUG
//    assert(mLogicSequences.size() - 1 == mOperations.size());
//#endif
//
//    const int length = static_cast<int>(mLogicSequences.size());
//
//    uChar result = mLogicSequences[0][index];
//    for (int i = 1; i < length; ++i)
//    {
//        result = Evaluate(result, mLogicSequences[i][index], mOperations[i - 1]);
//    }
//
//    return result;
//}
//
//uChar LogicSequenceCombiner::Evaluate(const uChar a, const uChar b, const Operation operation) const
//{
//    switch (operation)
//    {
//        case Operation::AND:
//            return a & b;
//
//        case Operation::OR:
//            return a | b;
//
//        case Operation::XOR:
//            return a ^ b;
//
//        default:
//            return 0;
//    }
//}
