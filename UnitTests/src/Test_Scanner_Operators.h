#pragma once

using namespace juce;
#include "Scanner.h"
#include "ErrorReporting.h"
#include "Token.h"

class Test_Scanner_Operators : public UnitTest
{
public:
    Test_Scanner_Operators() : UnitTest("Test_Scanner_Operators") {}
    
    void runTest() override
    {
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
    }
};
