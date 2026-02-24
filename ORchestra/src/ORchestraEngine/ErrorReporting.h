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

#include <string>
#include <vector>
#include <atomic>

namespace ORchestra 
{
    constexpr int MAX_LOG_ENTRIES = 100;

    enum class EntryType { Messasge, Warning, Error };

    struct LogEntry
    {
        EntryType mEntryType;
        int mLine;
        int mStepCount;
        std::string mMessage;
    };

    /// Listener interface for ErrorReporting updates.
    /// Called when log entries are added or cleared.
    /// Note: OnLogUpdated() may be called from the audio thread.
    class ErrorReportingListener
    {
    public:
        virtual ~ErrorReportingListener() = default;
        virtual void OnLogUpdated() = 0;
    };

    //TODO: make this take line and position as well for reporting properly syntax erros
    class ErrorReporting
    {
    public:
        void LogError(const int line, std::string& message);
        void LogError(const std::string& message);
        void LogWarning(const int line, std::string& message);
        void LogWarning(const std::string& message);
        void LogMessage(const std::string& message);
        void LogMessage(const std::string& message, int stepCount);
        void Clear();
        void RequestClear();
        std::vector<LogEntry> GetErrors() { return mLogEntries; }
        
        void SetListener(ErrorReportingListener* listener) { mListener = listener; }

    private:
        void CheckAndClear();
        void TrimOldEntries();
        void NotifyListener();

        std::vector<LogEntry> mLogEntries;
        ErrorReportingListener* mListener = nullptr;
        std::atomic<bool> mShouldClear{false};
    };
} // namespace ORchestra
