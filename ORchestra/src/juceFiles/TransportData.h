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

#include <cstdint>

namespace ORchestra
{
    struct TransportData
    {
        double bpm = 0.0;          // filled from host/DAW via FillPositionData
        double bpmFromScript = 0.0; // set by script; 0 = not set, engine falls back to bpm
        double sampleRate = 0;
        float bpmDivision = 1.0f;
        int64_t timeInSamples = 0;
        bool isPlaying = false;
        int transposeOffset = 0;
    };
} // namespace ORchestra
