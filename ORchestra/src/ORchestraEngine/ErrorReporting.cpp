#include <iostream>

#include "ErrorReporting.h"

namespace ORchestra 
{
    void ErrorReporting::LogError(const int line, std::string& message)
    {
        message = message + ", at line " + std::to_string(line) + "\n";
        mLogEntries.emplace_back(LogEntry{ EntryType::Error, line, std::move(message) });
        std::cout << message << std::endl;
    }

    void ErrorReporting::LogError(const std::string& message)
    {
        mLogEntries.emplace_back(LogEntry{ EntryType::Error, 0, std::move(message) });
        std::cout << message << std::endl;
    }

    void ErrorReporting::LogWarning(const int line, std::string& message)
    {
        message = message + ", at line " + std::to_string(line) + "\n";
        mLogEntries.emplace_back(LogEntry{ EntryType::Warning, line, std::move(message) });
        std::cout << message << std::endl;
    }

    void ErrorReporting::LogWarning(const std::string& message)
    {
        mLogEntries.emplace_back(LogEntry{ EntryType::Warning, 0, std::move(message) });
        std::cout << message << std::endl;
    }

    void ErrorReporting::LogMessage(const std::string& message)
    {
        mLogEntries.emplace_back(LogEntry{ EntryType::Messasge, 0, std::move(message) });
        std::cout << message << std::endl;
    }

    void ErrorReporting::Clear()
    {
        mLogEntries.clear();
    }
} // namespace ORchestra
