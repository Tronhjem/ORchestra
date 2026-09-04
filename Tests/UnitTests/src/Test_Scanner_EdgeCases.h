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

#include "Scanner.h"
#include "ErrorReporting.h"
#include "ORchestraToken.h"

using namespace ORchestra;

TEST_CASE("Scanner: Empty input produces only END token", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::END);
}

TEST_CASE("Scanner: Comments are ignored and not tokenized", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "// this is a comment\n42";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    // Token order: EOL (\n after comment), NUMBER (42), END
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::EOL);
    REQUIRE(tokens[1].GetType() == ORchestraTokenType::NUMBER);
    REQUIRE(tokens[2].GetType() == ORchestraTokenType::END);
}

TEST_CASE("Scanner: Comment at end of file with no newline is still ignored", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "42 // trailing comment";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NUMBER);
}

TEST_CASE("Scanner: Comment spanning entire line before code", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "// var declaration\na = 5";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens.size() >= 4);
    // After comment and newline: first token is EOL, then IDENTIFIER 'a' 
    REQUIRE(tokens[1].GetType() == ORchestraTokenType::IDENTIFIER);
}

TEST_CASE("Scanner: Double slash not at start of line is still a comment", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "a = 5 // inline\nb = 10";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens.size() >= 5);
}

TEST_CASE("Scanner: Tabs and carriage returns are skipped as whitespace", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "\t42\r\n\t100";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens.size() >= 3);
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NUMBER);
}

TEST_CASE("Scanner: Multiple spaces between tokens", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "a    =    42";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
    REQUIRE(tokens[1].GetType() == ORchestraTokenType::EQUAL);
    REQUIRE(tokens[2].GetType() == ORchestraTokenType::NUMBER);
}

TEST_CASE("Scanner: Floats like '3.14' are tokenized as NUMBER", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "3.14";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NUMBER);
}

TEST_CASE("Scanner: Float '0.5' is tokenized as NUMBER", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "0.5";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NUMBER);
}

TEST_CASE("Scanner: Identifier with '#' character e.g. 'C#4' is parsed as NOTE_IDENTIFIER", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "C#4";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NOTE_IDENTIFIER);
}

TEST_CASE("Scanner: Identifier with '_' underscore is valid", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "my_var";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
}

TEST_CASE("Scanner: Identifier starting with underscore is valid", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "_private";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
}

TEST_CASE("Scanner: Identifier with digits after underscore 'var_123'", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "var_123";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
}

TEST_CASE("Scanner: Error on unexpected character '?' produces PARSE_ERROR", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "?";

    REQUIRE(scanner.ScanFile(input.c_str()) == false);

    const auto& errors = errorReporting.GetErrors();
    REQUIRE_FALSE(errors.empty());
    REQUIRE(errors[0].mMessage.find("Unexpected Character '?'") != std::string::npos);
}

TEST_CASE("Scanner: Error on unexpected character '`' produces PARSE_ERROR", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "`";

    REQUIRE(scanner.ScanFile(input.c_str()) == false);

    const auto& errors = errorReporting.GetErrors();
    REQUIRE_FALSE(errors.empty());
    REQUIRE(errors[0].mMessage.find("Unexpected Character '`'") != std::string::npos);
}

TEST_CASE("Scanner: Error on unexpected character '~' produces PARSE_ERROR", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "~";

    REQUIRE(scanner.ScanFile(input.c_str()) == false);

    const auto& errors = errorReporting.GetErrors();
    REQUIRE_FALSE(errors.empty());
    REQUIRE(errors[0].mMessage.find("Unexpected Character '~'") != std::string::npos);
}

TEST_CASE("Scanner: Very long identifier is tokenized correctly", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
    REQUIRE(tokens[0].mLength == 62);
}

TEST_CASE("Scanner: Very long number is tokenized correctly", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "12345678901234567890";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NUMBER);
    REQUIRE(tokens[0].mLength == 20);
}

TEST_CASE("Scanner: Reset clears tokens and allows re-scan", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input1 = "42";
    std::string input2 = "99";

    REQUIRE(scanner.ScanFile(input1.c_str()));
    REQUIRE(scanner.GetTokens()[0].GetType() == ORchestraTokenType::NUMBER);

    scanner.Reset();
    REQUIRE(scanner.ScanFile(input2.c_str()));
    REQUIRE(scanner.GetTokens()[0].GetType() == ORchestraTokenType::NUMBER);
}

TEST_CASE("Scanner: Note identifier 'Eb3' recognized through scanner", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "Eb3";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NOTE_IDENTIFIER);
}

TEST_CASE("Scanner: Note identifier 'F#5' recognized through scanner", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "F#5";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NOTE_IDENTIFIER);
}

TEST_CASE("Scanner: Note identifier 'Bb2' recognized through scanner", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "Bb2";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NOTE_IDENTIFIER);
}

TEST_CASE("Scanner: Prints keyword recognized", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "print";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::PRINT);
}

TEST_CASE("Scanner: 'fn' keyword recognized", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "fn";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::FN);
}

TEST_CASE("Scanner: 'end' keyword recognized", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "end";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::END_FN);
}


TEST_CASE("Scanner: 'return' keyword recognized", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "return";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::RETURN);
}

TEST_CASE("Scanner: 'note' keyword recognized", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "note";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::NOTE);
}

TEST_CASE("Scanner: 'cc' keyword recognized", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "cc";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::CC);
}

TEST_CASE("Scanner: All note division literals tokenized as BEAT_IDENTIFIER", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);

    const std::string divisions[] = {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
    for (const auto& div : divisions)
    {
        Scanner s(errorReporting);
        REQUIRE(s.ScanFile(div.c_str()));
        REQUIRE(s.GetTokens()[0].GetType() == ORchestraTokenType::BEAT_IDENTIFIER);
    }
}

TEST_CASE("Scanner: Mixed case 'MyVar' is tokenized as IDENTIFIER", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "MyVar";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
}

TEST_CASE("Scanner: Number with leading dot '.5' is not a token (starts with DOT, then number)", "[Scanner_EdgeCases]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = ".5";

    REQUIRE(scanner.ScanFile(input.c_str()));

    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::DOT);
    REQUIRE(tokens[1].GetType() == ORchestraTokenType::NUMBER);
}
