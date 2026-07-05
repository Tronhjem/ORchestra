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


#if defined(_DEBUG)
    #include "ORchestraAssert.h"
    #include <mutex>
    #include <atomic>
#endif

namespace ORchestra
{
    class AssertMutex
    {
    public:
        AssertMutex() = default;
        ~AssertMutex() = default;

        AssertMutex(const AssertMutex&) = delete;
        AssertMutex& operator=(const AssertMutex&) = delete;

        void lock()
        {
#if defined(_DEBUG)
            ORCHESTRA_ASSERT_SIMPLE(!mLocked.load(std::memory_order_relaxed));
            mMutex.lock();
            mLocked.store(true, std::memory_order_relaxed);
#endif
        }

        void unlock()
        {
#if defined(_DEBUG)
            mLocked.store(false, std::memory_order_relaxed);
            mMutex.unlock();
#endif
        }

        bool tryLock()
        {
#if defined(_DEBUG)
            if (mMutex.try_lock())
            {
                mLocked.store(true, std::memory_order_relaxed);
                return true;
            }
            ORCHESTRA_ASSERT_SIMPLE(false);
            return false;
#else
            return true;
#endif
        }

    private:
#if defined(_DEBUG)
        std::mutex mMutex;
        std::atomic<bool> mLocked { false };
#endif
    };

    // RAII scoped lock for AssertMutex. No-op in release builds.
    class AssertMutexScopedLock
    {
    public:
        explicit AssertMutexScopedLock(AssertMutex& mutex) :
            mMutex(mutex)
        {
            mMutex.lock();
        }

        ~AssertMutexScopedLock()
        {
            mMutex.unlock();
        }

        AssertMutexScopedLock(const AssertMutexScopedLock&) = delete;
        AssertMutexScopedLock& operator=(const AssertMutexScopedLock&) = delete;

    private:
        AssertMutex& mMutex;
    };
} //namespace ORchestra
