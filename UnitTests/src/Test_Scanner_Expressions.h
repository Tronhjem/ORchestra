#pragma once
#include "../catch.hpp"
#include "Scanner.h"
#include "ErrorReporting.h"
#include "Token.h"
TEST_CASE("Scanner: Recognizes 'ran' keyword as RANDOM token", "[Scanner_Expressions]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "ran";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 1);
            REQUIRE(tokens[0].GetType() == TokenType::RANDOM);
}

TEST_CASE("Scanner: Recognizes 'euc' keyword as EUCLIDEAN token", "[Scanner_Expressions]")
{
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "euc";
            
            REQUIRE(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            REQUIRE(tokens.size() >= 1);
            REQUIRE(tokens[0].GetType() == TokenType::EUCLIDEAN);
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
                if (token.GetType() == TokenType::LEFT_BRACKET)
                    foundLeftBracket = true;
                if (token.GetType() == TokenType::RIGHT_BRACKET)
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
            REQUIRE(tokens[0].GetType() == TokenType::LEFT_PAREN);
}
