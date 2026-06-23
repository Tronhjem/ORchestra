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

namespace ORchestra
{
    // Maps a note-duration division index to its BPM division factor.
    // 1 = whole (0.25), 2 = half (0.5), 3 = quarter (1.0), 4 = eighth (2.0),
    // 5 = 16th (4.0), 6 = 32nd (8.0), 7 = 64th (16.0).
    // Out-of-range values clamp to [1,7] so audio and UI never diverge.
    inline float DurationToBpmDivision(int divValue)
    {
        const int i = std::clamp(divValue, 1, 7);
        switch (i)
        {
            case 1:  return 0.25f;
            case 2:  return 0.5f;
            case 3:  return 1.0f;
            case 4:  return 2.0f;
            case 5:  return 4.0f;
            case 6:  return 8.0f;
            case 7:  return 16.0f;
            default: return 1.0f;
        }
    }
} // namespace ORchestra
