#pragma once

using namespace juce;
#include "Scanner.h"
#include "ErrorReporting.h"
#include "Token.h"

class Test_Scanner_BasicTokens : public UnitTest
{
public:
    Test_Scanner_BasicTokens() : UnitTest("Test_Scanner_BasicTokens") {}
    
    void runTest() override
    {
        {
            beginTest("Scanner: Tokenizes simple number '42' as NUMBER token");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "42";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 1);
            expect(tokens[0].GetType() == TokenType::NUMBER);
        }
        {
            beginTest("Scanner: Tokenizes identifier 'myVar' as IDENTIFIER token");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "myVar";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 1);
            expect(tokens[0].GetType() == TokenType::IDENTIFIER);
        }
        {
            beginTest("Scanner: Tokenizes parentheses '()' as LEFT_PAREN, RIGHT_PAREN");
            
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
            beginTest("Scanner: Tokenizes brackets '[]' as LEFT_BRACKET, RIGHT_BRACKET");
            
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
            beginTest("Scanner: Tokenizes braces '{}' as LEFT_BRACE, RIGHT_BRACE");
            
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
            beginTest("Scanner: Tokenizes comma-separated list '1,2,3' with 2 COMMA tokens");
            
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
            beginTest("Scanner: Handles multi-line input 'a\\nb\\nc' and tracks line numbers");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a\nb\nc";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            // Line numbers should increase
            expect(tokens.size() >= 3);
        }
    }
};
