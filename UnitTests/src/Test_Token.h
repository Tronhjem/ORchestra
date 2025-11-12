#pragma once

#include "../catch.hpp"
#include "Token.h"

TEST_CASE("Token: Constructs IDENTIFIER token with correct type, length, line number and pointer", "[Token]")
{
    const char* testString = "test";
    Token token(TokenType::IDENTIFIER, testString, 4, 1);
    
    REQUIRE(token.GetType() == TokenType::IDENTIFIER);
    REQUIRE(token.mLength == 4);
    REQUIRE(token.mLine == 1);
    REQUIRE(token.mStart == testString);
}

TEST_CASE("Token: Constructs NUMBER token with correct type, length=3, line=5", "[Token]")
{
    const char* numberString = "123";
    Token token(TokenType::NUMBER, numberString, 3, 5);
    
    REQUIRE(token.GetType() == TokenType::NUMBER);
    REQUIRE(token.mLength == 3);
    REQUIRE(token.mLine == 5);
}

TEST_CASE("Token: Constructs PLUS operator token with correct type, length=1, line=10", "[Token]")
{
    const char* opString = "+";
    Token token(TokenType::PLUS, opString, 1, 10);
    
    REQUIRE(token.GetType() == TokenType::PLUS);
    REQUIRE(token.mLength == 1);
    REQUIRE(token.mLine == 10);
}

TEST_CASE("Token: Constructs LEFT_PAREN token with correct type", "[Token]")
{
    const char* parenString = "(";
    Token leftParen(TokenType::LEFT_PAREN, parenString, 1, 2);
    
    REQUIRE(leftParen.GetType() == TokenType::LEFT_PAREN);
}

TEST_CASE("Token: Constructs LEFT_BRACKET and RIGHT_BRACKET tokens with correct types", "[Token]")
{
    const char* bracketString = "[";
    Token leftBracket(TokenType::LEFT_BRACKET, bracketString, 1, 1);
    
    REQUIRE(leftBracket.GetType() == TokenType::LEFT_BRACKET);
    
    const char* rightBracketString = "]";
    Token rightBracket(TokenType::RIGHT_BRACKET, rightBracketString, 1, 1);
    
    REQUIRE(rightBracket.GetType() == TokenType::RIGHT_BRACKET);
}

TEST_CASE("Token: Constructs logic operator tokens (AND, OR, XOR) with correct types", "[Token]")
{
    const char* andString = "&";
    Token andToken(TokenType::AND, andString, 1, 1);
    REQUIRE(andToken.GetType() == TokenType::AND);
    
    const char* orString = "|";
    Token orToken(TokenType::OR, orString, 1, 1);
    REQUIRE(orToken.GetType() == TokenType::OR);
    
    const char* xorString = "^";
    Token xorToken(TokenType::XOR, xorString, 1, 1);
    REQUIRE(xorToken.GetType() == TokenType::XOR);
}

TEST_CASE("Token: Constructs comparison operator tokens (==, !=) with correct types and length=2", "[Token]")
{
    const char* eqString = "==";
    Token eqToken(TokenType::EQUAL_EQUAL, eqString, 2, 1);
    REQUIRE(eqToken.GetType() == TokenType::EQUAL_EQUAL);
    REQUIRE(eqToken.mLength == 2);
    
    const char* neqString = "!=";
    Token neqToken(TokenType::BANG_EQUAL, neqString, 2, 1);
    REQUIRE(neqToken.GetType() == TokenType::BANG_EQUAL);
}

TEST_CASE("Token: Constructs keyword tokens (RANDOM='ran', EUCLIDEAN='euc') with correct types", "[Token]")
{
    const char* randomStr = "ran";
    Token randomToken(TokenType::RANDOM, randomStr, 3, 1);
    REQUIRE(randomToken.GetType() == TokenType::RANDOM);
    
    const char* eucStr = "euc";
    Token eucToken(TokenType::EUCLIDEAN, eucStr, 3, 1);
    REQUIRE(eucToken.GetType() == TokenType::EUCLIDEAN);
}

TEST_CASE("Token: Tracks line numbers correctly (lines 1, 42, 100)", "[Token]")
{
    const char* str = "test";
    Token token1(TokenType::IDENTIFIER, str, 4, 1);
    Token token2(TokenType::IDENTIFIER, str, 4, 42);
    Token token3(TokenType::IDENTIFIER, str, 4, 100);
    
    REQUIRE(token1.mLine == 1);
    REQUIRE(token2.mLine == 42);
    REQUIRE(token3.mLine == 100);
}
