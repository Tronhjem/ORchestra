#include "DataSequence.h"

DataSequence::DataSequence(const std::vector<StepData>& data) : mData(data)
{
}

StepData DataSequence::GetValue(const int index) const
{
    const int indexWrapped = index % mData.size();
    return mData[indexWrapped];
}

void DataSequence::SetValue(int index, StepData value)
{
    mData[index] = value;
}
