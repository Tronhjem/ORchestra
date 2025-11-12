#pragma once

using namespace juce;
#include "Scanner.h"
#include "ErrorReporting.h"
#include "Token.h"

class Test_Scanner_Expressions : public UnitTest
{
public:
    Test_Scanner_Expressions() : UnitTest("Test_Scanner_Expressions") {}
    
    void runTest() override
    {
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
