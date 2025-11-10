#pragma once
#include "DataSequence.h"

void GenerateEuclideanSequence(StepData* sequence, const int hits, const int length)
{
    int count = length;
    for(int i = 0; i < length; ++i)
    {
        if (count >= length)
        {
            sequence[i] = static_cast<StepData>(1);
            count = count - length;
        }
        else
        {
            sequence[i] = static_cast<StepData>(0);
        }
        count += hits;
    }
}
