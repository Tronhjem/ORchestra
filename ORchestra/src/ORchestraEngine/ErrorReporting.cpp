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
