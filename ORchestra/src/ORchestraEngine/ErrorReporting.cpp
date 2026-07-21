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

#include <chrono>
#include <ctime>
#include <cstdio>
#include <thread>

#include "ErrorReporting.h"

#if defined(_DEBUG) && defined(ORCHESTRA_RACE_WIDEN)
    #define ORCHESTRA_ERROR_WIDEN_SLEEP() \
        std::this_thread::sleep_for(std::chrono::microseconds(300))
#else
    #define ORCHESTRA_ERROR_WIDEN_SLEEP() do { } while (false)
#endif

namespace ORchestra
{
    static std::string CurrentTimestamp()
    {
        const std::time_t t = std::time(nullptr);
        struct tm tm;
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        char buf[12];
        std::snprintf(buf, sizeof(buf), "[%02d:%02d:%02d] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
        return buf;
    }

    void ErrorReporting::CheckAndClear()
    {
        if (!mShouldClear.load(std::memory_order_relaxed))
            return;

        std::unique_lock<std::mutex> lock {mEntriesMutex, std::try_to_lock};
        if (!lock.owns_lock())
            return;

        mShouldClear.store(false, std::memory_order_relaxed);
        mLogEntries.clear();

        lock.unlock();
        NotifyListener();
    }

    void ErrorReporting::TrimOldEntries()
    {
        if (mLogEntries.size() > MAX_LOG_ENTRIES)
        {
            mLogEntries.erase(mLogEntries.begin());
        }
    }

    void ErrorReporting::NotifyListener()
    {
        if (mListener)
        {
            mListener->OnLogUpdated();
        }
    }

    inline void ErrorReporting::ForwardToSink(const LogEntry& entry)
    {
#if defined(_DEBUG)
        if (mSink)
            mSink(entry);
#else
        (void)entry;
#endif
    }

    void ErrorReporting::LogError(const int line, std::string& message)
    {
        {
            std::scoped_lock lock {mEntriesMutex};
            std::string stamp = CurrentTimestamp();
            message.insert(0, stamp);
            message += ", at line ";
            message += std::to_string(line);
            message += "\n";
            mLogEntries.emplace_back(LogEntry{ EntryType::Error, line, message });
            ForwardToSink(mLogEntries.back());
            TrimOldEntries();
        }
        NotifyListener();
    }

    void ErrorReporting::AppendPlainLocked(EntryType type, const std::string& message)
    {
        ORCHESTRA_ERROR_WIDEN_SLEEP();
        std::string stamped;
        stamped.reserve(12 + message.size() + 1);
        stamped = CurrentTimestamp();
        stamped += message;
        stamped += "\n";
        mLogEntries.emplace_back(LogEntry{ type, 0, stamped });
        ForwardToSink(mLogEntries.back());
        TrimOldEntries();
    }

    void ErrorReporting::LogError(const std::string& message)
    {
        {
            std::scoped_lock lock {mEntriesMutex};
            AppendPlainLocked(EntryType::Error, message);
        }
        NotifyListener();
    }

    void ErrorReporting::LogWarning(const int line, std::string& message)
    {
        {
            std::scoped_lock lock {mEntriesMutex};
            std::string stamp = CurrentTimestamp();
            message.insert(0, stamp);
            message += ", at line ";
            message += std::to_string(line);
            message += "\n";
            mLogEntries.emplace_back(LogEntry{ EntryType::Warning, line, message });
            ForwardToSink(mLogEntries.back());
            TrimOldEntries();
        }
        NotifyListener();
    }

    void ErrorReporting::LogWarning(const std::string& message)
    {
        {
            std::scoped_lock lock {mEntriesMutex};
            AppendPlainLocked(EntryType::Warning, message);
        }
        NotifyListener();
    }

    void ErrorReporting::LogMessage(const std::string& message)
    {
        {
            std::scoped_lock lock {mEntriesMutex};
            AppendPlainLocked(EntryType::Messasge, message);
        }
        NotifyListener();
    }

    bool ErrorReporting::TryLogMessage(const std::string& message)
    {
        std::unique_lock<std::mutex> lock {mEntriesMutex, std::try_to_lock};
        if (!lock.owns_lock())
            return false;

        AppendPlainLocked(EntryType::Messasge, message);

        lock.unlock();
        NotifyListener();
        return true;
    }

    void ErrorReporting::Clear()
    {
        {
            std::scoped_lock lock {mEntriesMutex};
            mLogEntries.clear();
        }
        NotifyListener();
    }

    void ErrorReporting::RequestClear()
    {
        mShouldClear.store(true);
    }
} // namespace ORchestra
