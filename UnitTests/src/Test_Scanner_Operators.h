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
TEST_CASE("Scanner: Tokenizes arithmetic operators '+ - * /' as PLUS, MINUS, STAR, SLASH", "[Scanner_Operators]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "+ - * /";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 4);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::PLUS);
            REQUIRE(tokens[1].GetType() == ORchestraTokenType::MINUS);
            REQUIRE(tokens[2].GetType() == ORchestraTokenType::STAR);
            REQUIRE(tokens[3].GetType() == ORchestraTokenType::SLASH);
}

TEST_CASE("Scanner: Distinguishes assignment '=' from equality '==' in 'a = 5 == 5'", "[Scanner_Operators]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a = 5 == 5";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            bool foundAssign = false;
            bool foundEqual = false;
            
            for (const auto& token : tokens)
            {
                if (token.GetType() == ORchestraTokenType::EQUAL)
                    foundAssign = true;
                if (token.GetType() == ORchestraTokenType::EQUAL_EQUAL)
                    foundEqual = true;
            }
            
            REQUIRE(foundAssign);
            REQUIRE(foundEqual);
}

TEST_CASE("Scanner: Tokenizes comparison operators '< > <= >= !=' correctly", "[Scanner_Operators]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "< > <= >= !=";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 5);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::LESS);
            REQUIRE(tokens[1].GetType() == ORchestraTokenType::GREATER);
            REQUIRE(tokens[2].GetType() == ORchestraTokenType::LESS_EQUAL);
            REQUIRE(tokens[3].GetType() == ORchestraTokenType::GREATER_EQUAL);
            REQUIRE(tokens[4].GetType() == ORchestraTokenType::BANG_EQUAL);
}

TEST_CASE("Scanner: Tokenizes logic operators '& | ^' as AND, OR, XOR", "[Scanner_Operators]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "& | ^";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 3);
            REQUIRE(tokens[0].GetType() == ORchestraTokenType::AND);
            REQUIRE(tokens[1].GetType() == ORchestraTokenType::OR);
            REQUIRE(tokens[2].GetType() == ORchestraTokenType::XOR);
}
