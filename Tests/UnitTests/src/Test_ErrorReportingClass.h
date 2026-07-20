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

#include "catch.hpp"
#include "ErrorReporting.h"

using namespace ORchestra;

TEST_CASE("ErrorReporting: LogError with line records error with correct type", "[ErrorReporting]")
{
    ErrorReporting reporter;
    std::string msg{"Test error message"};
    reporter.LogError(10, msg);

    const auto& entries = reporter.GetErrors();
    REQUIRE_FALSE(entries.empty());
    REQUIRE(entries[0].mEntryType == EntryType::Error);
    REQUIRE(entries[0].mLine == 10);
    REQUIRE(entries[0].mMessage.find("Test error message") != std::string::npos);
}

TEST_CASE("ErrorReporting: LogWarning with line records warning with correct type", "[ErrorReporting]")
{
    ErrorReporting reporter;
    std::string msg{"Test warning message"};
    reporter.LogWarning(5, msg);

    const auto& entries = reporter.GetErrors();
    REQUIRE_FALSE(entries.empty());
    REQUIRE(entries[0].mEntryType == EntryType::Warning);
    REQUIRE(entries[0].mLine == 5);
    REQUIRE(entries[0].mMessage.find("Test warning message") != std::string::npos);
}

TEST_CASE("ErrorReporting: LogError without line records with line=0", "[ErrorReporting]")
{
    ErrorReporting reporter;
    reporter.LogError("Error without line");

    const auto& entries = reporter.GetErrors();
    REQUIRE_FALSE(entries.empty());
    REQUIRE(entries[0].mEntryType == EntryType::Error);
    REQUIRE(entries[0].mLine == 0);
}

TEST_CASE("ErrorReporting: LogWarning without line records with line=0", "[ErrorReporting]")
{
    ErrorReporting reporter;
    reporter.LogWarning("Warning without line");

    const auto& entries = reporter.GetErrors();
    REQUIRE_FALSE(entries.empty());
    REQUIRE(entries[0].mEntryType == EntryType::Warning);
    REQUIRE(entries[0].mLine == 0);
}

TEST_CASE("ErrorReporting: LogMessage records message", "[ErrorReporting]")
{
    ErrorReporting reporter;
    reporter.LogMessage("Info message");

    const auto& entries = reporter.GetErrors();
    REQUIRE_FALSE(entries.empty());
    REQUIRE(entries[0].mEntryType == EntryType::Messasge);
    REQUIRE(entries[0].mMessage.find("Info message") != std::string::npos);
}

TEST_CASE("ErrorReporting: Clear removes all entries", "[ErrorReporting]")
{
    ErrorReporting reporter;
    reporter.LogError("error 1");
    reporter.LogError("error 2");
    REQUIRE(reporter.GetErrors().size() == 2);

    reporter.Clear();
    REQUIRE(reporter.GetErrors().empty());
}

TEST_CASE("ErrorReporting: RequestClear clears on CheckAndClear", "[ErrorReporting]")
{
    ErrorReporting reporter;
    reporter.LogError("error");
    REQUIRE(reporter.GetErrors().size() == 1);

    reporter.RequestClear();
    reporter.CheckAndClear();
    REQUIRE(reporter.GetErrors().empty());
}

TEST_CASE("ErrorReporting: CheckAndClear without request does nothing", "[ErrorReporting]")
{
    ErrorReporting reporter;
    reporter.LogError("error");
    reporter.CheckAndClear();
    REQUIRE_FALSE(reporter.GetErrors().empty());
}

TEST_CASE("ErrorReporting: Listener is notified on LogError", "[ErrorReporting]")
{
    ErrorReporting reporter;
    bool notified = false;

    class TestListener : public ErrorReportingListener
    {
    public:
        bool* mNotified;
        TestListener(bool* n) : mNotified(n) {}
        void OnLogUpdated() override { *mNotified = true; }
    };

    TestListener listener(&notified);
    reporter.SetListener(&listener);
    reporter.LogError("test");
    REQUIRE(notified);
}

TEST_CASE("ErrorReporting: Listener notified on Clear", "[ErrorReporting]")
{
    ErrorReporting reporter;
    bool notified = false;

    class TestListener : public ErrorReportingListener
    {
    public:
        bool* mNotified;
        TestListener(bool* n) : mNotified(n) {}
        void OnLogUpdated() override { *mNotified = true; }
    };

    TestListener listener(&notified);
    reporter.SetListener(&listener);
    reporter.LogError("test");
    notified = false;
    reporter.Clear();
    REQUIRE(notified);
}

TEST_CASE("ErrorReporting: Max log entries trimmed at 100", "[ErrorReporting]")
{
    ErrorReporting reporter;
    for (int i = 0; i < 120; ++i)
    {
        reporter.LogError("error " + std::to_string(i));
    }

    const auto& entries = reporter.GetErrors();
    REQUIRE(entries.size() <= 100);
}
