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
TEST_CASE("Scanner: Recognizes 'ran' keyword as IDENTIFIER token", "[Scanner_Expressions]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "ran";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 1);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
}

TEST_CASE("Scanner: Recognizes 'euc' keyword as EUCLIDEAN token", "[Scanner_Expressions]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "euc";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 1);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::EUCLIDEAN);
}

TEST_CASE("Scanner: Tokenizes expression 'a = 42 + 10' into 5+ tokens", "[Scanner_Expressions]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a = 42 + 10";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 5);
}

TEST_CASE("Scanner: Tokenizes array syntax 'a = [1, 2, 3]' with brackets", "[Scanner_Expressions]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a = [1, 2, 3]";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            bool foundLeftBracket = false;
            bool foundRightBracket = false;
            
            for (const auto& token : tokens)
            {
                if (token.GetType() == ORchestraTokenType::LEFT_BRACKET)
                    foundLeftBracket = true;
                if (token.GetType() == ORchestraTokenType::RIGHT_BRACKET)
                    foundRightBracket = true;
            }
            
            REQUIRE(foundLeftBracket);
            REQUIRE(foundRightBracket);
}

TEST_CASE("Scanner: Tokenizes expression '(2 + 3) * 4' starting with LEFT_PAREN", "[Scanner_Expressions]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "(2 + 3) * 4";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 7);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::LEFT_PAREN);
}
