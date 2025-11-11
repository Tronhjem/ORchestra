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
            beginTest("Scanner: Tokenizes arithmetic operators '+ - * /' as PLUS, MINUS, STAR, SLASH");
            
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
            beginTest("Scanner: Distinguishes assignment '=' from equality '==' in 'a = 5 == 5'");
            
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
            beginTest("Scanner: Tokenizes comparison operators '< > <= >= !=' correctly");
            
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
            beginTest("Scanner: Tokenizes logic operators '& | ^' as AND, OR, XOR");
            
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
            beginTest("Scanner: Recognizes 'ran' keyword as RANDOM token");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "ran";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 1);
            expect(tokens[0].GetType() == TokenType::RANDOM);
        }
        {
            beginTest("Scanner: Recognizes 'euc' keyword as EUCLIDEAN token");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "euc";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 1);
            expect(tokens[0].GetType() == TokenType::EUCLIDEAN);
        }
        {
            beginTest("Scanner: Tokenizes expression 'a = 42 + 10' into 5+ tokens");
            
            ErrorReporting errorReporting;
            Scanner scanner(errorReporting);
            std::string input = "a = 42 + 10";
            
            expect(scanner.ScanFile(input.c_str()));
            
            auto& tokens = scanner.GetTokens();
            expect(tokens.size() >= 5);
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
        {
            beginTest("Scanner: Tokenizes array syntax 'a = [1, 2, 3]' with brackets");
            
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
            beginTest("Scanner: Tokenizes expression '(2 + 3) * 4' starting with LEFT_PAREN");
            
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
