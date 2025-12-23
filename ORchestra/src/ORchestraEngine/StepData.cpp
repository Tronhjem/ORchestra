#include <cstring>
#include <cmath>

#include "StepData.h"

namespace ORchestra 
{
    StepData::StepData(const DataUnit *data, const int length)
        : mLength(length)
    {
#if _DEBUG
        assert(length <= MAX_SUB_DIVISION_LENGTH);
        assert(length <= MAX_SUB_DIVISION_LENGTH);
#endif

        SetData(data, mLength);
    }

    StepData::StepData(const int i)
    {
        mData[0] = static_cast<DataUnit>(i);
        mLength = 1;
    }

    DataUnit StepData::GetValue(const int index) const
    {
#if _DEBUG
        assert(index < mLength);
#endif

        return mData[index];
    }

    DataUnit StepData::GetEquivalentValueAtIndex(const int index, const int otherLength) const
    {
#if _DEBUG
        assert(index < MAX_SUB_DIVISION_LENGTH);
#endif

        if (mLength == 1)
            return mData[0];

        const int equivalentIndex = static_cast<int>(floor((static_cast<float>(index) 
                                    / static_cast<float>(otherLength)) 
                                    * static_cast<float>(mLength)));

#if _DEBUG
        assert(equivalentIndex < mLength);
#endif

        return mData[equivalentIndex];
    }

    void StepData::SetData(const DataUnit *data, const int length)
    {
#if _DEBUG
        assert(length <= MAX_SUB_DIVISION_LENGTH);
#endif

        memcpy(mData, data, static_cast<unsigned long>(length) * sizeof(DataUnit));
        mLength = length;
    }

    // Keeping these for now in case we need them again.
    // But really should use the Apply Operation Directly instead of operation overload.
    //
    // DataSequenceStep DataSequenceStep::operator+ (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a + b; });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator- (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a - b; });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator* (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a * b; });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator/ (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a / b; });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator& (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return (a > 0) & (b > 0); });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator^ (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return (a > 0) ^ (b > 0); });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator| (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return (a > 0) | (b > 0); });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator< (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a < b; });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator<= (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a <= b; });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator> (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a > b; });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator>= (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a >= b; });
    //}
    //
    // DataSequenceStep DataSequenceStep::operator== (const DataSequenceStep& other) const
    //{
    //    return ApplyOperation(other, [](int a, int b ) { return a == b; });
    //}
} // namespace ORchestra
