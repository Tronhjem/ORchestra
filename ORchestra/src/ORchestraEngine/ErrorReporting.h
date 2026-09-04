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

#include <mutex>
#include <string>
#include <vector>
#include <atomic>

#if defined(_DEBUG) || defined(ORCHESTRA_ENABLE_LOGGING)
    #include <functional>
#endif

namespace ORchestra 
{
    constexpr int MAX_LOG_ENTRIES = 100;

    enum class EntryType { Messasge, Warning, Error };

    struct LogEntry
    {
        EntryType mEntryType;
        int mLine;
        std::string mMessage;
    };

#if defined(_DEBUG) || defined(ORCHESTRA_ENABLE_LOGGING)
    using LogSinkFn = std::function<void(const LogEntry&)>;
#endif

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
        // Non-blocking variant for the audio thread: on lock contention the
        // message is dropped and false is returned (no entry, no notify).
        bool TryLogMessage(const std::string& message);
        void Clear();
        void RequestClear();
        // Returns a copy: mLogEntries is mutated from the worker and audio
        // threads, so callers must never iterate it by reference.
        std::vector<LogEntry> GetErrors() const
        {
            std::scoped_lock lock {mEntriesMutex};
            return mLogEntries;
        }
        
        void SetListener(ErrorReportingListener* listener) { mListener = listener; }
#if defined(_TEST)
        // Lets tests hold the entries mutex to exercise the TryLogMessage drop path.
        std::unique_lock<std::mutex> AcquireLockForTest() { return std::unique_lock<std::mutex>{mEntriesMutex}; }
#endif
#if defined(_DEBUG) || defined(ORCHESTRA_ENABLE_LOGGING)
        void SetSink(LogSinkFn sink) { mSink = std::move(sink); }
#endif
        void CheckAndClear();

    private:
        void TrimOldEntries();
        void NotifyListener();
        void ForwardToSink(const LogEntry& entry);
        // Appends a timestamped plain message. Caller must hold mEntriesMutex.
        void AppendPlainLocked(EntryType type, const std::string& message);

        std::vector<LogEntry> mLogEntries;
        ErrorReportingListener* mListener = nullptr;
        mutable std::mutex mEntriesMutex;
#if defined(_DEBUG) || defined(ORCHESTRA_ENABLE_LOGGING)
        LogSinkFn mSink;
#endif
        std::atomic<bool> mShouldClear{false};
    };
} // namespace ORchestra
