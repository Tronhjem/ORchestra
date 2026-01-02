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

#include "StepData.h"

namespace ORchestra 
{
    static void GenerateEuclideanSequence(StepData* sequence, const int hits, const int length)
    {
        int count = length;
        for (int i = 0; i < length; ++i)
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


} // namespace ORchestra
