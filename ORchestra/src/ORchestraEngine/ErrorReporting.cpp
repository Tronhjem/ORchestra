#include <iostream>

#include "ErrorReporting.h"

void ErrorReporting::LogError(const int line, std::string& message)
{
    message = message + " at line " + std::to_string(line) + "\n";
    mLogEntries.emplace_back(LogEntry{EntryType::Error, line, message});
    std::cout << message << std::endl;
}

void ErrorReporting::LogError(const std::string& message)
{
    mLogEntries.emplace_back(LogEntry{EntryType::Error, 0, message + "\n"});
    std::cout << message << std::endl;
}

void ErrorReporting::LogWarning(const int line, std::string &message)
{
    message = message + " at line " + std::to_string(line) + "\n";
    mLogEntries.emplace_back(LogEntry{EntryType::Warning, line, message});
    std::cout << message << std::endl;
}

void ErrorReporting::LogWarning(const std::string& message)
{
    mLogEntries.emplace_back(LogEntry{EntryType::Warning, 0, message + "\n"});
    std::cout << message << std::endl;
}

void ErrorReporting::LogMessage(const std::string& message)
{
    mLogEntries.emplace_back(LogEntry{EntryType::Messasge, 0, message + "\n"});
    std::cout << message << std::endl;
}

void ErrorReporting::Clear()
{
    mLogEntries.clear();
}
