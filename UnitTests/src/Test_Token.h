#pragma once

#include "catch.hpp"

#include "ORchestraToken.h"


using namespace ORchestra;
TEST_CASE("ORchestraToken: Constructs IDENTIFIER token with correct type, length, line number and pointer", "[ORchestraToken]")
{
    const char* testString = "test";
    ORchestraToken token(ORchestraTokenType::IDENTIFIER, testString, 4, 1);
    
    REQUIRE(token.GetType() == ORchestraTokenType::IDENTIFIER);
    REQUIRE(token.mLength == 4);
    REQUIRE(token.mLine == 1);
    REQUIRE(token.mStart == testString);
}

TEST_CASE("ORchestraToken: Constructs NUMBER token with correct type, length=3, line=5", "[ORchestraToken]")
{
    const char* numberString = "123";
    ORchestraToken token(ORchestraTokenType::NUMBER, numberString, 3, 5);
    
    REQUIRE(token.GetType() == ORchestraTokenType::NUMBER);
    REQUIRE(token.mLength == 3);
    REQUIRE(token.mLine == 5);
}

TEST_CASE("ORchestraToken: Constructs PLUS operator token with correct type, length=1, line=10", "[ORchestraToken]")
{
    const char* opString = "+";
    ORchestraToken token(ORchestraTokenType::PLUS, opString, 1, 10);
    
    REQUIRE(token.GetType() == ORchestraTokenType::PLUS);
    REQUIRE(token.mLength == 1);
    REQUIRE(token.mLine == 10);
}

TEST_CASE("ORchestraToken: Constructs LEFT_PAREN token with correct type", "[ORchestraToken]")
{
    const char* parenString = "(";
    ORchestraToken leftParen(ORchestraTokenType::LEFT_PAREN, parenString, 1, 2);
    
    REQUIRE(leftParen.GetType() == ORchestraTokenType::LEFT_PAREN);
}

TEST_CASE("ORchestraToken: Constructs LEFT_BRACKET and RIGHT_BRACKET tokens with correct types", "[ORchestraToken]")
{
    const char* bracketString = "[";
    ORchestraToken leftBracket(ORchestraTokenType::LEFT_BRACKET, bracketString, 1, 1);
    
    REQUIRE(leftBracket.GetType() == ORchestraTokenType::LEFT_BRACKET);
    
    const char* rightBracketString = "]";
    ORchestraToken rightBracket(ORchestraTokenType::RIGHT_BRACKET, rightBracketString, 1, 1);
    
    REQUIRE(rightBracket.GetType() == ORchestraTokenType::RIGHT_BRACKET);
}

TEST_CASE("ORchestraToken: Constructs logic operator tokens (AND, OR, XOR) with correct types", "[ORchestraToken]")
{
    const char* andString = "&";
    ORchestraToken andToken(ORchestraTokenType::AND, andString, 1, 1);
    REQUIRE(andToken.GetType() == ORchestraTokenType::AND);
    
    const char* orString = "|";
    ORchestraToken orToken(ORchestraTokenType::OR, orString, 1, 1);
    REQUIRE(orToken.GetType() == ORchestraTokenType::OR);
    
    const char* xorString = "^";
    ORchestraToken xorToken(ORchestraTokenType::XOR, xorString, 1, 1);
    REQUIRE(xorToken.GetType() == ORchestraTokenType::XOR);
}

TEST_CASE("ORchestraToken: Constructs comparison operator tokens (==, !=) with correct types and length=2", "[ORchestraToken]")
{
    const char* eqString = "==";
    ORchestraToken eqToken(ORchestraTokenType::EQUAL_EQUAL, eqString, 2, 1);
    REQUIRE(eqToken.GetType() == ORchestraTokenType::EQUAL_EQUAL);
    REQUIRE(eqToken.mLength == 2);
    
    const char* neqString = "!=";
    ORchestraToken neqToken(ORchestraTokenType::BANG_EQUAL, neqString, 2, 1);
    REQUIRE(neqToken.GetType() == ORchestraTokenType::BANG_EQUAL);
}

TEST_CASE("ORchestraToken: Constructs keyword tokens (RANDOM='ran', EUCLIDEAN='euc') with correct types", "[ORchestraToken]")
{
    const char* randomStr = "ran";
    ORchestraToken randomToken(ORchestraTokenType::RANDOM, randomStr, 3, 1);
    REQUIRE(randomToken.GetType() == ORchestraTokenType::RANDOM);
    
    const char* eucStr = "euc";
    ORchestraToken eucToken(ORchestraTokenType::EUCLIDEAN, eucStr, 3, 1);
    REQUIRE(eucToken.GetType() == ORchestraTokenType::EUCLIDEAN);
}

TEST_CASE("ORchestraToken: Tracks line numbers correctly (lines 1, 42, 100)", "[ORchestraToken]")
{
    const char* str = "test";
    ORchestraToken token1(ORchestraTokenType::IDENTIFIER, str, 4, 1);
    ORchestraToken token2(ORchestraTokenType::IDENTIFIER, str, 4, 42);
    ORchestraToken token3(ORchestraTokenType::IDENTIFIER, str, 4, 100);
    
    REQUIRE(token1.mLine == 1);
    REQUIRE(token2.mLine == 42);
    REQUIRE(token3.mLine == 100);
}
