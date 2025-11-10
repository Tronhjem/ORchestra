#include "Token.h"

Token::Token(TokenType tokenType,
      const char* start,
      int length,
      int line) : mTokenType(tokenType),
                  mStart(start),
                  mLength(length),
                  mLine(line)
{
}