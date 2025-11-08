#include <stdio.h>

#include "Scanner.h"
#include "ErrorReporting.h"
#include "Token.h"
#include "ScopedTimer.h"

Scanner::Scanner(ErrorReporting &logger) : mErrorReporting(logger)
{
}

Scanner::~Scanner()
{
}

Token Scanner::MakeToken(TokenType tokenType)
{
    return Token(tokenType, mStart, static_cast<int>(mCurrent - mStart), mCurrentLine);
}

Token Scanner::MakeErrorToken(char* message, char symbol)
{
    size_t len = strlen(message);
    message[len] = symbol;
    message[len + 1] = '\0';
    return Token(TokenType::ERROR, message, (int)strlen(message), mCurrentLine);
}

void Scanner::SkipWhiteSpace()// append char
{
    for (;;)
    {
        char c = PeekCurrent();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            AdvanceCurrent();
            break;

        case '/':
        {
            if (PeekNext() == '/')
            {
                // A comment goes until the end of the line.
                while (PeekCurrent() != '\n' && !IsAtEnd())
                    AdvanceCurrent();
            }
            else
            {
                return;
            }
            break;
        }
        default:
            return;
        }
    }
}

bool Scanner::ScanFile(const char* data)
{
    ScopedTimer timer("ScanTokens");

    mCurrent = data;
    mStart = data;

    for (;;)
    {
        Token t = ScanToken();

        if(t.GetType() == TokenType::ERROR)
        {
            std::string errorString = std::string(t.mStart, t.mLength);
            mErrorReporting.LogError(mCurrentLine, errorString);
            return false;
        }

        mTokens.emplace_back(t);

        if (t.GetType() == TokenType::END)
        {
            return true;
        }
    }
}

Token Scanner::ScanToken()
{
    SkipWhiteSpace();

    mStart = mCurrent;
    if(IsAtEnd())
        return MakeToken(TokenType::END);

    char c = AdvanceCurrent();

    if (IsAlpha(c))
        return BuildIdentifier();

    if (IsDigit(c))
        return BuildDigit();

    switch (c)
    {
        case ('\n'):
        {
            mCurrentLine++;
            return MakeToken(TokenType::EOL);
        }
            
        //LOGIC
        case '&':
            return MakeToken(TokenType::AND);
        case '|':
            return MakeToken(TokenType::OR);
        case '^':
            return MakeToken(TokenType::XOR);
            
        //Syntax
        case '(':
            return MakeToken(TokenType::LEFT_PAREN);
        case ')':
            return MakeToken(TokenType::RIGHT_PAREN);
        case '{':
            return MakeToken(TokenType::LEFT_BRACE);
        case '}':
            return MakeToken(TokenType::RIGHT_BRACE);
        case '[':
            return MakeToken(TokenType::LEFT_BRACKET);
        case ']':
            return MakeToken(TokenType::RIGHT_BRACKET);
        case '.':
            return MakeToken(TokenType::DOT);
        case ',':
            return MakeToken(TokenType::COMMA);
            
        case '!':
            return MakeToken(Match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=':
            return MakeToken(Match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<':
            return MakeToken(Match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return MakeToken(Match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            
        //MATH
        case '-':
            return MakeToken(TokenType::MINUS);
        case '+':
            return MakeToken(TokenType::PLUS);
        case '*':
            return MakeToken(TokenType::STAR);
        case '/':
            return MakeToken(TokenType::SLASH);
            
        default:
            return MakeErrorToken(ERROR_UNEXPECTED_CHAR, c);
    }
}

bool Scanner::IsAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Scanner::IsDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool Scanner::IsAtEnd()
{
    return *mCurrent == '\0';
}

char Scanner::PeekCurrent()
{
    if (IsAtEnd())
        return '\0';

    return *mCurrent;
}

char Scanner::PeekNext()
{
    if (IsAtEnd())
        return '\0';

    return mCurrent[1];
}

char Scanner::AdvanceCurrent()
{
    if (IsAtEnd())
        return '\0';

    ++mCurrent;
    return mCurrent[-1];
}

bool Scanner::Match(char expected)
{
    if(IsAtEnd())
        return false;

    if(*mCurrent != expected)
        return false;

    AdvanceCurrent();
    return true;
}

TokenType Scanner::IdentifierToken()
{
    auto checkKeyword = [&](int start, int length,
                            const char* rest, TokenType type)
    {
        if (mCurrent - mStart == start + length &&
            memcmp(mStart + start, rest, length) == 0)
        {
            return type;
        }

        return TokenType::IDENTIFIER;
    };

    // Checking if any of these are reserved words.
    switch (mStart[0])
    {
#if _DEBUG
    case 'p':
        return checkKeyword(1, 4, "rint", TokenType::PRINT);
#endif
#if _TEST
    case 't':
        return checkKeyword(1, 3, "est", TokenType::TEST);
#endif
    case 'r':
        return checkKeyword(1, 2, "an", TokenType::RANDOM);
    case 'e':
        return checkKeyword(1, 2, "uc", TokenType::EUCLIDEAN);
    case 'n':
        return checkKeyword(1, 3, "ote", TokenType::NOTE);
    case 'c':
        return checkKeyword(1, 1, "c", TokenType::CC);
            
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'A':
    case 'B':
    {
        return TokenType::NOTE_IDENTIFIER;
    }

    default:
        return TokenType::IDENTIFIER;
            
//    case 'a':
//        return checkKeyword(1, 2, "nd", TokenType::AND);
//    case 'c':
//        return checkKeyword(1, 4, "lass", TokenType::CLASS);
//    case 'e':
//        return checkKeyword(1, 3, "lse", TokenType::ELSE);
//    case 'i':
//        return checkKeyword(1, 1, "f", TokenType::IF);
//    case 'n':
//        return checkKeyword(1, 2, "il", TokenType::NIL);
//    case 'o':
//        return checkKeyword(1, 1, "r", TokenType::OR);
//    case 'r':
//        return checkKeyword(1, 5, "eturn", TokenType::RETURN);
//    case 's':
//        return checkKeyword(1, 4, "uper", TokenType::SUPER);
//    case 'v':
//        return checkKeyword(1, 2, "ar", TokenType::VAR);
//    case 'w':
//        return checkKeyword(1, 4, "hile", TokenType::WHILE);
    }
}

Token Scanner::BuildIdentifier()
{
    while (IsAlpha(PeekCurrent()) || IsDigit(PeekCurrent()) || PeekCurrent() == '#')
        AdvanceCurrent();

    return MakeToken(IdentifierToken());
}

Token Scanner::BuildString()
{
    while (PeekCurrent() != '"' && !IsAtEnd())
    {
        if(PeekCurrent() == '\n')
        {
            ++mCurrentLine;
            return MakeErrorToken(ERROR_NO_END_QUOTE, PeekCurrent());
        }

        AdvanceCurrent();
    }

    if(IsAtEnd())
    {
        return MakeErrorToken(ERROR_NO_END_QUOTE, PeekCurrent());
    }

    AdvanceCurrent();
    Token token = MakeToken(TokenType::STRING);
    return token;
}

Token Scanner::BuildDigit()
{
    while( IsDigit( PeekCurrent() )) 
        AdvanceCurrent(); 

    if (PeekCurrent() == '.' && IsDigit(PeekNext()))
    {
        AdvanceCurrent();

        while (IsDigit(PeekCurrent()))
            AdvanceCurrent();
    }

    return MakeToken(TokenType::NUMBER);
}
