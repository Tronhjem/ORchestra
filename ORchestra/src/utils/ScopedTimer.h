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

#include <chrono>
#include <iostream>

namespace ORchestra
{
    class ScopedTimer
    {
    public:
        ScopedTimer(const std::string& name) :
            mName(name),
            mStartTime(std::chrono::high_resolution_clock::now())
        { }

        ~ScopedTimer()
        {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - mStartTime).count();
            std::cout << mName << " executed in " << duration << " µs" << std::endl;
        }

    private:
        std::string mName;
        std::chrono::high_resolution_clock::time_point mStartTime;
    };
} //namespace ORchestra
