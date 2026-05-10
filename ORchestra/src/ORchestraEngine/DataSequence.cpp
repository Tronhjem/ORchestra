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

#include "DataSequence.h"

namespace ORchestra 
{
    DataSequence::DataSequence(const std::vector<StepData>& data) : mData(data)
    {
    }

    DataSequence::DataSequence(std::vector<StepData>&& data) noexcept : mData(std::move(data))
    {
    }

    DataSequence::DataSequence(const StepData* begin, int count) : mData(begin, begin + count)
    {
    }

    DataSequence::DataSequence(StepData value) : mData{value}
    {
    }

    StepData DataSequence::GetValue(const int index) const
    {
        const int indexWrapped = index % static_cast<int>(mData.size());
        return mData[static_cast<unsigned long>(indexWrapped)];
    }

    void DataSequence::SetValue(int index, const StepData& value)
    {
        const int indexWrapped = index % static_cast<int>(mData.size());
        mData[static_cast<unsigned long>(indexWrapped)] = value;
    }
} // namespace ORchestra
