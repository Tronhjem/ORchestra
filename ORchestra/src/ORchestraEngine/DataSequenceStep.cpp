#include <algorithm>

#include "DataSequenceStep.h"
#include "Types.h"

DataSequenceStep::DataSequenceStep(const uChar* data, const int length)
    : mLength(length)
{
#if _DEBUG
    assert(length <= MAX_SUB_DIVISION_LENGTH);
#endif
    
    SetData(data, length);
}

DataSequenceStep::DataSequenceStep(const int i)
{
    mData[0] = i;
    mLength = 1;
}

uChar DataSequenceStep::GetValue(const int index) const
{
#if _DEBUG
    assert(index < mLength);
#endif
    
    return mData[index];
}

uChar DataSequenceStep::GetEquivalentValueAtIndex(const int index, const int otherLength) const
{
#if _DEBUG
    assert(index < MAX_SUB_DIVISION_LENGTH);
#endif
    
    if (mLength == 1)
        return mData[0];
    
    const int equivalentIndex = floor((static_cast<float>(index) / static_cast<float>(otherLength)) * mLength);
    
#if _DEBUG
    assert(equivalentIndex < mLength);
#endif
    
    return mData[equivalentIndex];
}

void DataSequenceStep::SetData(const uChar* data, const int length)
{
    memcpy(mData, data, length);
    mLength = length;
}

// Keeping these for now in case we need them again.
// But really should use the Apply Operation Directly instead of operation overload.
//
//DataSequenceStep DataSequenceStep::operator+ (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a + b; });
//}
//
//DataSequenceStep DataSequenceStep::operator- (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a - b; });
//}
//
//DataSequenceStep DataSequenceStep::operator* (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a * b; });
//}
//
//DataSequenceStep DataSequenceStep::operator/ (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a / b; });
//}
//
//DataSequenceStep DataSequenceStep::operator& (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return (a > 0) & (b > 0); });
//}
//
//DataSequenceStep DataSequenceStep::operator^ (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return (a > 0) ^ (b > 0); });
//}
//
//DataSequenceStep DataSequenceStep::operator| (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return (a > 0) | (b > 0); });
//}
//
//DataSequenceStep DataSequenceStep::operator< (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a < b; });
//}
//
//DataSequenceStep DataSequenceStep::operator<= (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a <= b; });
//}
//
//DataSequenceStep DataSequenceStep::operator> (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a > b; });
//}
//
//DataSequenceStep DataSequenceStep::operator>= (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a >= b; });
//}
//
//DataSequenceStep DataSequenceStep::operator== (const DataSequenceStep& other) const
//{
//    return ApplyOperation(other, [](int a, int b ) { return a == b; });
//}
