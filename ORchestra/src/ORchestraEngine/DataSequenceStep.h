#pragma once

#include <cassert>
#include <cmath>

#include "Defines.h"

typedef unsigned char uChar;

/// Each Step in the DataSequences holds a number of unsigned chars that makes up
/// the sub division of a step. It's necessary to have all values as these DataSequenceSteps
/// with an array of data, as we need to always be able to combine a single value with a substep
/// of any length.
class DataSequenceStep
{
public:
    DataSequenceStep() : mLength(0) {};
    DataSequenceStep(const uChar* data, const int length);
    DataSequenceStep(const int i);
    uChar GetValue(const int index) const;
    uChar GetEquivalentValueAtIndex(const int index, const int otherLength) const;
    void SetData(const uChar* data, const int length);
    uChar GetLength() const { return mLength; };

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
    
    /// Takes another DataSequenceStep and returns a new DataSequenceStep
    /// with the length of the longest of the two.
    /// A lambda is passed that takes 2 ints, and this operation will be applied to each
    /// Sub Division step in the sequence.
    template<typename Operation>
    DataSequenceStep ApplySequenceWithOperation(const DataSequenceStep& otherSequence,
                                    Operation OperationLambda) const
    {
        static_assert(std::is_invocable_v<Operation, const int, const int>,
                             "Operation must be callable with two int parameters");
        
        const DataSequenceStep& longest = this->GetLength() > otherSequence.GetLength() ? *this : otherSequence;
        const DataSequenceStep& shortest = this->GetLength() <= otherSequence.GetLength() ? *this : otherSequence;
        
        const int newLength = longest.GetLength();
        DataSequenceStep newStep {};
        newStep.mLength = newLength;
        
        for(int i = 0; i < newLength; ++i)
        {
            const int newValue = OperationLambda(static_cast<int>(shortest.GetEquivalentValueAtIndex(i, newLength)),
                                                 static_cast<int>(longest.GetValue(i)));
            newStep.mData[i] = static_cast<uChar>(std::clamp(newValue, 0, 127));
        }
        
        return newStep;
    }
    
private:
    uChar mLength;
    uChar mData[MAX_SUB_DIVISION_LENGTH];
};
