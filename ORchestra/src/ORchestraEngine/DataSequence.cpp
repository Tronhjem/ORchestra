#include "DataSequence.h"

namespace ORchestra 
{
    DataSequence::DataSequence(const std::vector<StepData>& data) : mData(data)
    {
    }

    StepData DataSequence::GetValue(const int index) const
    {
        const int indexWrapped = index % static_cast<int>(mData.size());
        return mData[static_cast<unsigned long>(indexWrapped)];
    }

    void DataSequence::SetValue(int index, StepData value)
    {
#if _DEBUG
        assert(index < mData.size());
#endif
        mData[static_cast<unsigned long>(index)] = value;
    }
} // namespace ORchestra
