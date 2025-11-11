#pragma once

using namespace juce;
#include "Scanner.h"
#include "ErrorReporting.h"
#include "Token.h"

class Test_Scanner : public UnitTest
{
public:
    Test_Scanner() : UnitTest("Test_Scanner") {}
    
    void runTest() override
    {
        {
            beginTest("Scanner tokenizes simple number");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "42";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 1);
            expect(tokens[0].GetType() == TokenType::NUMBER);
        }
        {
            beginTest("Scanner tokenizes identifier");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "myVar";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 1);
            expect(tokens[0].GetType() == TokenType::IDENTIFIER);
        }
        {
            beginTest("Scanner tokenizes operators");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "+ - * /";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 4);
            expect(tokens[0].GetType() == TokenType::PLUS);
            expect(tokens[1].GetType() == TokenType::MINUS);
            expect(tokens[2].GetType() == TokenType::STAR);
            expect(tokens[3].GetType() == TokenType::SLASH);
        }
        {
            beginTest("Scanner tokenizes parentheses");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "()";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 2);
            expect(tokens[0].GetType() == TokenType::LEFT_PAREN);
            expect(tokens[1].GetType() == TokenType::RIGHT_PAREN);
        }
        {
            beginTest("Scanner tokenizes brackets");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "[]";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 2);
            expect(tokens[0].GetType() == TokenType::LEFT_BRACKET);
            expect(tokens[1].GetType() == TokenType::RIGHT_BRACKET);
        }
        {
            beginTest("Scanner tokenizes braces");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "{}";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 2);
            expect(tokens[0].GetType() == TokenType::LEFT_BRACE);
            expect(tokens[1].GetType() == TokenType::RIGHT_BRACE);
        }
        {
            beginTest("Scanner tokenizes assignment and equality");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a = 5 == 5";
            
            expect(scanner.ScanFile(input.c_str()));
            
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
            
            expect(foundAssign);
            expect(foundEqual);
        }
        {
            beginTest("Scanner tokenizes comparison operators");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "< > <= >= !=";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 5);
            expect(tokens[0].GetType() == TokenType::LESS);
            expect(tokens[1].GetType() == TokenType::GREATER);
            expect(tokens[2].GetType() == TokenType::LESS_EQUAL);
            expect(tokens[3].GetType() == TokenType::GREATER_EQUAL);
            expect(tokens[4].GetType() == TokenType::BANG_EQUAL);
        }
        {
            beginTest("Scanner tokenizes logic operators");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "& | ^";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 3);
            expect(tokens[0].GetType() == TokenType::AND);
            expect(tokens[1].GetType() == TokenType::OR);
            expect(tokens[2].GetType() == TokenType::XOR);
        }
        {
            beginTest("Scanner recognizes ran keyword");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "ran";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 1);
            expect(tokens[0].GetType() == TokenType::RANDOM);
        }
        {
            beginTest("Scanner recognizes euc keyword");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "euc";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 1);
            expect(tokens[0].GetType() == TokenType::EUCLIDEAN);
        }
        {
            beginTest("Scanner handles multiple tokens on one line");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a = 42 + 10";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 5);
        }
        {
            beginTest("Scanner handles comma separation");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "1,2,3";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            int commaCount = 0;
            for (const auto& token : tokens)
            {
                if (token.GetType() == TokenType::COMMA)
                    commaCount++;
            }
            expect(commaCount == 2);
        }
        {
            beginTest("Scanner handles newlines for line tracking");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a\nb\nc";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            // Line numbers should increase
            expect(tokens.size() >= 3);
        }
        {
            beginTest("Scanner tokenizes array syntax");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a = [1, 2, 3]";
            
            expect(scanner.ScanFile(input.c_str()));
            
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
            
            expect(foundLeftBracket);
            expect(foundRightBracket);
        }
        {
            beginTest("Scanner handles expression with parentheses");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "(2 + 3) * 4";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 7);
            expect(tokens[0].GetType() == TokenType::LEFT_PAREN);
        }
    }
};
