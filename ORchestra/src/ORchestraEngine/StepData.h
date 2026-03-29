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

#include <algorithm>
#include <cassert>

#include "Defines.h"

namespace ORchestra 
{
    /// Each Step in the DataSequences holds a number of unsigned chars that makes up
    /// the sub division of a step. It's necessary to have all values as these DataSequenceSteps
    /// with an array of data, as we need to always be able to combine a single value with a substep
    /// of any length.
    class StepData
    {
    public:
        StepData() : mLength(static_cast<DataUnit>(0)) {}
        explicit StepData(const DataUnit* data, const int length);
        explicit StepData(const int i);
        DataUnit GetValue(const int index) const;
        DataUnit GetEquivalentValueAtIndex(const int index, const int otherLength) const;
        void SetData(const DataUnit* data, const int length);
        int GetLength() const { return mLength; }

        /// Takes another StepData and returns a new DataSequenceStep
        /// with the length of the longest of the two.
        /// A lambda is passed that takes 2 ints, and this operation will be applied to each
        /// Sub Division step in the sequence.
        template <typename Operation>
        StepData ApplySequenceWithOperation(const StepData& otherSequence,
            const Operation operationLambda) const
        {
            static_assert(std::is_invocable_v<Operation, const int, const int>,
                "Operation must be callable with two int parameters");

            const int newLength = std::max(this->GetLength(), otherSequence.GetLength()); 

            StepData newStep{};
            newStep.mLength = newLength;

            for (int i = 0; i < newLength; ++i)
            {
                const int newValue = operationLambda(
                        static_cast<int>(this->GetEquivalentValueAtIndex(i, newLength)),
                        static_cast<int>(otherSequence.GetEquivalentValueAtIndex(i, newLength)));
                newStep.mData[i] = static_cast<DataUnit>(std::clamp(newValue, DATA_UNIT_MIN_VALUE, DATA_UNIT_MAX_VALUE));
            }

            return newStep;
        }

    private:
        int mLength;
        DataUnit mData[MAX_SUB_DIVISION_LENGTH];

        //===========================================================================
        // Keeping these for now in case we need them again.
        // But really should use the Apply Operation Directly instead of operation overload.
        //===========================================================================
        //    DataSequenceStep operator+ (const DataSequenceStep& other) const;
        //    DataSequenceStep operator- (const DataSequenceStep& other) const;
        //    DataSequenceStep operator* (const DataSequenceStep& other) const;
        //    DataSequenceStep operator/ (const DataSequenceStep& other) const;
        //
        //    DataSequenceStep operator& (const DataSequenceStep& other) const;
        //    DataSequenceStep operator^ (const DataSequenceStep& other) const;
        //    DataSequenceStep operator| (const DataSequenceStep& other) const;
        //
        //    DataSequenceStep operator< (const DataSequenceStep& other) const;
        //    DataSequenceStep operator<= (const DataSequenceStep& other) const;
        //    DataSequenceStep operator> (const DataSequenceStep& other) const;
        //    DataSequenceStep operator>= (const DataSequenceStep& other) const;
        //    DataSequenceStep operator== (const DataSequenceStep& other) const;
        // ===========================================================================
    };


} // namespace ORchestra
