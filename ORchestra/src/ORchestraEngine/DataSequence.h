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

#include <vector>

#include "StepData.h"

namespace ORchestra
{
    /// DataSequence holds a number of StepData.
    /// These are the ones we access with the global index to send data.
    class DataSequence
    {
    public:
        DataSequence() {}
        explicit DataSequence(const std::vector<StepData>& data);
        explicit DataSequence(std::vector<StepData>&& data) noexcept;
        explicit DataSequence(const StepData* begin, int count);
        explicit DataSequence(StepData value);
        StepData GetValue(const int index) const;
        void SetValue(const int index, const StepData& value);

    private:
        std::vector<StepData> mData;
    };

} // namespace ORchestra
