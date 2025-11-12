#pragma once

#include "../catch.hpp"
#include "Scanner.h"
#include "ErrorReporting.h"
#include "Token.h"

TEST_CASE("Scanner: Tokenizes arithmetic operators '+ - * /' as PLUS, MINUS, STAR, SLASH", "[Scanner_Operators]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "+ - * /";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 4);
            REQUIRE(tokens[0].GetType() == TokenType::PLUS);
            REQUIRE(tokens[1].GetType() == TokenType::MINUS);
            REQUIRE(tokens[2].GetType() == TokenType::STAR);
            REQUIRE(tokens[3].GetType() == TokenType::SLASH);
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
                if (token.GetType() == TokenType::EQUAL)
                    foundAssign = true;
                if (token.GetType() == TokenType::EQUAL_EQUAL)
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
            REQUIRE(tokens[0].GetType() == TokenType::LESS);
            REQUIRE(tokens[1].GetType() == TokenType::GREATER);
            REQUIRE(tokens[2].GetType() == TokenType::LESS_EQUAL);
            REQUIRE(tokens[3].GetType() == TokenType::GREATER_EQUAL);
            REQUIRE(tokens[4].GetType() == TokenType::BANG_EQUAL);
        }

TEST_CASE("Scanner: Tokenizes logic operators '& | ^' as AND, OR, XOR", "[Scanner_Operators]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "& | ^";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 3);
            REQUIRE(tokens[0].GetType() == TokenType::AND);
            REQUIRE(tokens[1].GetType() == TokenType::OR);
            REQUIRE(tokens[2].GetType() == TokenType::XOR);

