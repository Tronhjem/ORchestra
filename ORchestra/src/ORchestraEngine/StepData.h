#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>

#include "Defines.h"
#include "Types.h"

namespace ORchestra {


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
            Operation OperationLambda) const
        {
            static_assert(std::is_invocable_v<Operation, const int, const int>,
                "Operation must be callable with two int parameters");

            const StepData& longest = this->GetLength() > otherSequence.GetLength() ? *this : otherSequence;
            const StepData& shortest = this->GetLength() <= otherSequence.GetLength() ? *this : otherSequence;

            const int newLength = longest.GetLength();
            StepData newStep{};
            newStep.mLength = newLength;

            for (int i = 0; i < newLength; ++i)
            {
                const int newValue = OperationLambda(static_cast<int>(shortest.GetEquivalentValueAtIndex(i, newLength)),
                    static_cast<int>(longest.GetValue(i)));
                newStep.mData[i] = static_cast<DataUnit>(std::clamp(newValue, 0, 127));
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