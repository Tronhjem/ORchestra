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
#include <functional>

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

    // Type of the external sink callback. Set by the plugin wrapper to forward log
    // entries to juce::Logger (or any other sink). Called under the same thread as
    // the originating LogError/LogWarning/LogMessage call, so the sink must be
    // thread-safe. May be nullptr to disable forwarding.
    using LogSinkFn = std::function<void(const LogEntry&)>;

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
        void Clear();
        void RequestClear();
        const std::vector<LogEntry>& GetErrors() const { return mLogEntries; }
        
        void SetListener(ErrorReportingListener* listener) { mListener = listener; }
        void SetSink(LogSinkFn sink) { mSink = std::move(sink); }
        void CheckAndClear();

    private:
        void TrimOldEntries();
        void NotifyListener();
        void ForwardToSink(const LogEntry& entry);

        std::vector<LogEntry> mLogEntries;
        ErrorReportingListener* mListener = nullptr;
        LogSinkFn mSink;
        std::atomic<bool> mShouldClear{false};
    };
} // namespace ORchestra
