#pragma once

using namespace juce;
#include "Token.h"

class Test_Token : public UnitTest
{
public:
    Test_Token() : UnitTest("Test_Token") {}
    
    void runTest() override
    {
        {
            beginTest("Token: Constructs IDENTIFIER token with correct type, length, line number and pointer");
            
            const char* testString = "test";
            Token token(TokenType::IDENTIFIER, testString, 4, 1);
            
            expect(token.GetType() == TokenType::IDENTIFIER);
            expect(token.mLength == 4);
            expect(token.mLine == 1);
            expect(token.mStart == testString);
        }
        {
            beginTest("Token: Constructs NUMBER token with correct type, length=3, line=5");
            
            const char* numberString = "123";
            Token token(TokenType::NUMBER, numberString, 3, 5);
            
            expect(token.GetType() == TokenType::NUMBER);
            expect(token.mLength == 3);
            expect(token.mLine == 5);
        }
        {
            beginTest("Token: Constructs PLUS operator token with correct type, length=1, line=10");
            
            const char* opString = "+";
            Token token(TokenType::PLUS, opString, 1, 10);
            
            expect(token.GetType() == TokenType::PLUS);
            expect(token.mLength == 1);
            expect(token.mLine == 10);
        }
        {
            beginTest("Token: Constructs LEFT_PAREN token with correct type");
            
            const char* parenString = "(";
            Token leftParen(TokenType::LEFT_PAREN, parenString, 1, 2);
            
            expect(leftParen.GetType() == TokenType::LEFT_PAREN);
        }
        {
            beginTest("Token: Constructs LEFT_BRACKET and RIGHT_BRACKET tokens with correct types");
            
            const char* bracketString = "[";
            Token leftBracket(TokenType::LEFT_BRACKET, bracketString, 1, 1);
            
            expect(leftBracket.GetType() == TokenType::LEFT_BRACKET);
            
            const char* rightBracketString = "]";
            Token rightBracket(TokenType::RIGHT_BRACKET, rightBracketString, 1, 1);
            
            expect(rightBracket.GetType() == TokenType::RIGHT_BRACKET);
        }
        {
            beginTest("Token: Constructs logic operator tokens (AND, OR, XOR) with correct types");
            
            const char* andString = "&";
            Token andToken(TokenType::AND, andString, 1, 1);
            expect(andToken.GetType() == TokenType::AND);
            
            const char* orString = "|";
            Token orToken(TokenType::OR, orString, 1, 1);
            expect(orToken.GetType() == TokenType::OR);
            
            const char* xorString = "^";
            Token xorToken(TokenType::XOR, xorString, 1, 1);
            expect(xorToken.GetType() == TokenType::XOR);
        }
        {
            beginTest("Token: Constructs comparison operator tokens (==, !=) with correct types and length=2");
            
            const char* eqString = "==";
            Token eqToken(TokenType::EQUAL_EQUAL, eqString, 2, 1);
            expect(eqToken.GetType() == TokenType::EQUAL_EQUAL);
            expect(eqToken.mLength == 2);
            
            const char* neqString = "!=";
            Token neqToken(TokenType::BANG_EQUAL, neqString, 2, 1);
            expect(neqToken.GetType() == TokenType::BANG_EQUAL);
        }
        {
            beginTest("Token: Constructs keyword tokens (RANDOM='ran', EUCLIDEAN='euc') with correct types");
            
            const char* randomStr = "ran";
            Token randomToken(TokenType::RANDOM, randomStr, 3, 1);
            expect(randomToken.GetType() == TokenType::RANDOM);
            
            const char* eucStr = "euc";
            Token eucToken(TokenType::EUCLIDEAN, eucStr, 3, 1);
            expect(eucToken.GetType() == TokenType::EUCLIDEAN);
        }
        {
            beginTest("Token: Tracks line numbers correctly (lines 1, 42, 100)");
            
            const char* str = "test";
            Token token1(TokenType::IDENTIFIER, str, 4, 1);
            Token token2(TokenType::IDENTIFIER, str, 4, 42);
            Token token3(TokenType::IDENTIFIER, str, 4, 100);
            
            expect(token1.mLine == 1);
            expect(token2.mLine == 42);
            expect(token3.mLine == 100);
        }
    }
};
