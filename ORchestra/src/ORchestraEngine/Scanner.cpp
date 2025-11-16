#include <cstring>
#include <stdio.h>
#include <cstring>

#include "Scanner.h"
#include "ErrorReporting.h"
#include "ORchestraToken.h"
#include "ScopedTimer.h"

namespace ORchestra
{

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

    constexpr std::string_view ERROR_UNEXPECTED_CHAR = "ERROR: Unexpected character ";
    constexpr std::string_view ERROR_NO_END_QUOTE = "ERROR: Expected \" but didn't find one ";

    Scanner::Scanner(ErrorReporting &logger) : mErrorReporting(logger)
    {
        mTokens.reserve(64);
    }

    Scanner::~Scanner()
    {
    }

    void Scanner::Reset()
    {
        mTokens.clear();
        mCurrentLine = 0;
        mStart = nullptr;
        mCurrent = nullptr;
    }

    ORchestraToken Scanner::MakeToken(ORchestraTokenType tokenType)
    {
        return ORchestraToken(tokenType, mStart, static_cast<int>(mCurrent - mStart), mCurrentLine);
    }

    ORchestraToken Scanner::MakeErrorToken(const std::string_view &message, char symbol)
    {
        std::string errorString;
        errorString.reserve(message.size() + 1);
        errorString.append(message.data(), message.size());
        errorString.append(&symbol, 1);

        mErrorReporting.LogError(mCurrentLine, errorString);
        return ORchestraToken(ORchestraTokenType::PARSE_ERROR, nullptr, 0, mCurrentLine);
    }

    void Scanner::SkipWhiteSpace() // append char
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

    bool Scanner::ScanFile(const std::string &data)
    {
#if _DEBUG
        ScopedTimer timer("ScanTokens");
#endif

        mCurrent = data.c_str();
        mStart = data.c_str();

        for (;;)
        {
            const ORchestraToken token = ScanToken();

            if (token.GetType() == ORchestraTokenType::PARSE_ERROR)
            {
                return false;
            }

            mTokens.emplace_back(token);

            if (token.GetType() == ORchestraTokenType::END)
            {
                return true;
            }
        }
    }

    ORchestraToken Scanner::ScanToken()
    {
        SkipWhiteSpace();

        mStart = mCurrent;
        if (IsAtEnd())
            return MakeToken(ORchestraTokenType::END);

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
            return MakeToken(ORchestraTokenType::EOL);
        }

        // LOGIC
        case '&':
            return MakeToken(ORchestraTokenType::AND);
        case '|':
            return MakeToken(ORchestraTokenType::OR);
        case '^':
            return MakeToken(ORchestraTokenType::XOR);

        // Syntax
        case '(':
            return MakeToken(ORchestraTokenType::LEFT_PAREN);
        case ')':
            return MakeToken(ORchestraTokenType::RIGHT_PAREN);
        case '{':
            return MakeToken(ORchestraTokenType::LEFT_BRACE);
        case '}':
            return MakeToken(ORchestraTokenType::RIGHT_BRACE);
        case '[':
            return MakeToken(ORchestraTokenType::LEFT_BRACKET);
        case ']':
            return MakeToken(ORchestraTokenType::RIGHT_BRACKET);
        case '.':
            return MakeToken(ORchestraTokenType::DOT);
        case ',':
            return MakeToken(ORchestraTokenType::COMMA);

        case '!':
            return MakeToken(Match('=') ? ORchestraTokenType::BANG_EQUAL : ORchestraTokenType::BANG);
        case '=':
            return MakeToken(Match('=') ? ORchestraTokenType::EQUAL_EQUAL : ORchestraTokenType::EQUAL);
        case '<':
            return MakeToken(Match('=') ? ORchestraTokenType::LESS_EQUAL : ORchestraTokenType::LESS);
        case '>':
            return MakeToken(Match('=') ? ORchestraTokenType::GREATER_EQUAL : ORchestraTokenType::GREATER);

        // MATH
        case '-':
            return MakeToken(ORchestraTokenType::MINUS);
        case '+':
            return MakeToken(ORchestraTokenType::PLUS);
        case '*':
            return MakeToken(ORchestraTokenType::STAR);
        case '/':
            return MakeToken(ORchestraTokenType::SLASH);
        case '%':
            return MakeToken(ORchestraTokenType::PERCENT);

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
        if (IsAtEnd())
            return false;

        if (*mCurrent != expected)
            return false;

        AdvanceCurrent();
        return true;
    }

    ORchestraTokenType Scanner::IdentifierToken()
    {
        auto checkKeyword = [&](int start, int length,
                                const char *rest, ORchestraTokenType type)
        {
            if (mCurrent - mStart == start + length &&
                memcmp(mStart + start, rest, static_cast<unsigned long>(length)) == 0)
            {
                return type;
            }

            return ORchestraTokenType::IDENTIFIER;
        };

        // Checking if any of these are reserved words.
        switch (mStart[0])
        {
#if _DEBUG
        case 'p':
            return checkKeyword(1, 4, "rint", ORchestraTokenType::PRINT);
#endif
#if _TEST
        case 't':
            return checkKeyword(1, 3, "est", ORchestraTokenType::TEST_KEYWORD);
#endif
        case 'r':
            return checkKeyword(1, 2, "an", ORchestraTokenType::RANDOM);
        case 'e':
            return checkKeyword(1, 2, "uc", ORchestraTokenType::EUCLIDEAN);
        case 'n':
            return checkKeyword(1, 3, "ote", ORchestraTokenType::NOTE);
        case 'c':
            return checkKeyword(1, 1, "c", ORchestraTokenType::CC);

        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'A':
        case 'B':
        {
            return ORchestraTokenType::NOTE_IDENTIFIER;
        }

        default:
            return ORchestraTokenType::IDENTIFIER;

            //    case 'a':
            //        return checkKeyword(1, 2, "nd", ORchestraTokenType::AND);
            //    case 'c':
            //        return checkKeyword(1, 4, "lass", ORchestraTokenType::CLASS);
            //    case 'e':
            //        return checkKeyword(1, 3, "lse", ORchestraTokenType::ELSE);
            //    case 'i':
            //        return checkKeyword(1, 1, "f", ORchestraTokenType::IF);
            //    case 'n':
            //        return checkKeyword(1, 2, "il", ORchestraTokenType::NIL);
            //    case 'o':
            //        return checkKeyword(1, 1, "r", ORchestraTokenType::OR);
            //    case 'r':
            //        return checkKeyword(1, 5, "eturn", ORchestraTokenType::RETURN);
            //    case 's':
            //        return checkKeyword(1, 4, "uper", ORchestraTokenType::SUPER);
            //    case 'v':
            //        return checkKeyword(1, 2, "ar", ORchestraTokenType::VAR);
            //    case 'w':
            //        return checkKeyword(1, 4, "hile", ORchestraTokenType::WHILE);
        }
    }

    ORchestraToken Scanner::BuildIdentifier()
    {
        while (IsAlpha(PeekCurrent()) || IsDigit(PeekCurrent()) || PeekCurrent() == '#')
            AdvanceCurrent();

        return MakeToken(IdentifierToken());
    }

    ORchestraToken Scanner::BuildString()
    {
        while (PeekCurrent() != '"' && !IsAtEnd())
        {
            if (PeekCurrent() == '\n')
            {
                ++mCurrentLine;
                return MakeErrorToken(ERROR_NO_END_QUOTE, PeekCurrent());
            }

            AdvanceCurrent();
        }

        if (IsAtEnd())
        {
            return MakeErrorToken(ERROR_NO_END_QUOTE, PeekCurrent());
        }

        AdvanceCurrent();
        ORchestraToken token = MakeToken(ORchestraTokenType::STRING);
        return token;
    }

    ORchestraToken Scanner::BuildDigit()
    {
        while (IsDigit(PeekCurrent()))
            AdvanceCurrent();

        if (PeekCurrent() == '.' && IsDigit(PeekNext()))
        {
            AdvanceCurrent();

            while (IsDigit(PeekCurrent()))
                AdvanceCurrent();
        }

        return MakeToken(ORchestraTokenType::NUMBER);
    }

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace ORchestra
