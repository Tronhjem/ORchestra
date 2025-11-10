//OLD, JUST KEEPING FOR ARCHIVE

//#include <assert.h>
//
//#include "LogicSequence.h"
//
//LogicSequence::LogicSequence(const uChar* start, int length)
//{
//    SetSequence(start, length);
//}
//
//inline void LogicSequence::SetSequence(const uChar* start, const int length)
//{
//#if DEBUG
//    assert(length <= MAX_LOGIC_SEQUENCE_LENGTH);
//#endif
//
//    mTrigger = 0;
//    for (int i = 0; i < length; ++i)
//    {
//        mTrigger |= start[i] << i;
//    }
//
//    mLength = length;
//}
//
//const uChar LogicSequence::operator [](const int i) const
//{
//    LogicContainer index = static_cast<LogicContainer>(i % mLength);
//    uChar value = ((1 << index) & mTrigger) >> index;
//    return value;
//}
