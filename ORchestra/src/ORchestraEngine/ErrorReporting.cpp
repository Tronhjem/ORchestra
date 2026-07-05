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

#include <ctime>
#include <cstdio>

#include "ErrorReporting.h"

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
        if (mShouldClear.exchange(false))
        {
            mLogEntries.clear();
            NotifyListener();
        }
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
#if defined (_DEBUG)
        if (mSink)
            mSink(entry);
#endif
    }

    void ErrorReporting::LogError(const int line, std::string& message)
    {
        std::string stamp = CurrentTimestamp();
        message.insert(0, stamp);
        message += ", at line ";
        message += std::to_string(line);
        message += "\n";
        mLogEntries.emplace_back(LogEntry{ EntryType::Error, line, message });
        ForwardToSink(mLogEntries.back());
        TrimOldEntries();
        NotifyListener();
    }

    void ErrorReporting::LogError(const std::string& message)
    {
        std::string stamped;
        stamped.reserve(12 + message.size() + 1);
        stamped = CurrentTimestamp();
        stamped += message;
        stamped += "\n";
        mLogEntries.emplace_back(LogEntry{ EntryType::Error, 0, stamped });
        ForwardToSink(mLogEntries.back());
        TrimOldEntries();
        NotifyListener();
    }

    void ErrorReporting::LogWarning(const int line, std::string& message)
    {
        std::string stamp = CurrentTimestamp();
        message.insert(0, stamp);
        message += ", at line ";
        message += std::to_string(line);
        message += "\n";
        mLogEntries.emplace_back(LogEntry{ EntryType::Warning, line, message });
        ForwardToSink(mLogEntries.back());
        TrimOldEntries();
        NotifyListener();
    }

    void ErrorReporting::LogWarning(const std::string& message)
    {
        std::string stamped;
        stamped.reserve(12 + message.size() + 1);
        stamped = CurrentTimestamp();
        stamped += message;
        stamped += "\n";
        mLogEntries.emplace_back(LogEntry{ EntryType::Warning, 0, stamped });
        ForwardToSink(mLogEntries.back());
        TrimOldEntries();
        NotifyListener();
    }

    void ErrorReporting::LogMessage(const std::string& message)
    {
        std::string stamped;
        stamped.reserve(12 + message.size() + 1);
        stamped = CurrentTimestamp();
        stamped += message;
        stamped += "\n";
        mLogEntries.emplace_back(LogEntry{ EntryType::Messasge, 0, stamped });
        ForwardToSink(mLogEntries.back());
        TrimOldEntries();
        NotifyListener();
    }

    void ErrorReporting::Clear()
    {
        mLogEntries.clear();
        NotifyListener();
    }

    void ErrorReporting::RequestClear()
    {
        mShouldClear.store(true);
    }
} // namespace ORchestra
