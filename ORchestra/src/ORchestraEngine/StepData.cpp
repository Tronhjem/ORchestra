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

#include <cstring>

#include "StepData.h"

namespace ORchestra 
{
    StepData::StepData(const DataUnit* data, const int length)
        : mLength(length)
    {
#if defined(_DEBUG)
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
#if defined(_DEBUG)
        assert(index < mLength);
#endif

        return mData[index];
    }

    DataUnit StepData::GetEquivalentValueAtIndex(const int index, const int otherLength) const
    {
#if defined(_DEBUG)
        assert(index < MAX_SUB_DIVISION_LENGTH);
#endif

        if (mLength == 1)
            return mData[0];

        const int equivalentIndex = (index * mLength) / otherLength;

#if defined(_DEBUG)
        assert(equivalentIndex < mLength);
#endif

        return mData[equivalentIndex];
    }

    void StepData::SetData(const DataUnit* data, const int length)
    {
#if defined(_DEBUG)
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
