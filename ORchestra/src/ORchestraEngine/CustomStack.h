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
#include <array>

namespace ORchestra
{
    template <typename T>
    class Stack
    {
    public:
        Stack() : mStackPointer(0) {}
        T Pop()
        {
#if _DEBUG
            assert(mStackPointer > 0);
#endif
            return mStack[--mStackPointer];
        }

        void Push(const T& i)
        {
#if _DEBUG
            assert(mStackPointer + 1 <= 64);
#endif
            mStack[mStackPointer++] = i;
        }

        void Clear()
        {
            mStackPointer = 0;
        }

        T Top()
        {
#if _DEBUG
            assert(mStackPointer != 0 && mStackPointer > 0);
#endif
            return mStack[mStackPointer - 1];
        }

        unsigned long mStackPointer;

    private:
        // TODO: Should we make this smaller or bigger??
        std::array<T, 64> mStack;
    };
} // namespace ORchestra
