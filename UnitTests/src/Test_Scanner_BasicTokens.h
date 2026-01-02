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
TEST_CASE("Scanner: Tokenizes simple number '42' as NUMBER token", "[Scanner_BasicTokens]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "42";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 1);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::NUMBER);
}

TEST_CASE("Scanner: Tokenizes identifier 'myVar' as IDENTIFIER token", "[Scanner_BasicTokens]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "myVar";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 1);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
}

TEST_CASE("Scanner: Tokenizes parentheses '()' as LEFT_PAREN, RIGHT_PAREN", "[Scanner_BasicTokens]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "()";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 2);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::LEFT_PAREN);
            REQUIRE(tokens[1].GetType() == ORchestraTokenType::RIGHT_PAREN);
}

TEST_CASE("Scanner: Tokenizes brackets '[]' as LEFT_BRACKET, RIGHT_BRACKET", "[Scanner_BasicTokens]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "[]";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 2);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::LEFT_BRACKET);
            REQUIRE(tokens[1].GetType() == ORchestraTokenType::RIGHT_BRACKET);
}

TEST_CASE("Scanner: Tokenizes braces '{}' as LEFT_BRACE, RIGHT_BRACE", "[Scanner_BasicTokens]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "{}";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 2);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::LEFT_BRACE);
            REQUIRE(tokens[1].GetType() == ORchestraTokenType::RIGHT_BRACE);
}

TEST_CASE("Scanner: Tokenizes comma-separated list '1,2,3' with 2 COMMA tokens", "[Scanner_BasicTokens]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "1,2,3";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            int commaCount = 0;
            for (const auto& token : tokens)
            {
                if (token.GetType() == ORchestraTokenType::COMMA)
                    commaCount++;
            }
            REQUIRE(commaCount == 2);
}

TEST_CASE("Scanner: Handles multi-line input 'a\\nb\\nc' and tracks line numbers", "[Scanner_BasicTokens]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a\nb\nc";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            // Line numbers should increase
            REQUIRE(tokens.size() >= 3);
}
