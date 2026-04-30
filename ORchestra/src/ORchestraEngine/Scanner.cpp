/*scanner.cpp
 * Copyright (C) 2026 Christian Tronhjem
 *
 * This file is part of ORchestra.
 *
 * ORchestra is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORchestra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ORchestra. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstring>
#include <stdio.h>
#include <cstring>

#include "Scanner.h"
#include "ErrorReporting.h"
#include "ORchestraToken.h"
#if _DEBUG
#include "ScopedTimer.h"
#endif

namespace ORchestra
{

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

    std::string_view ERROR_UNEXPECTED_CHAR = "Unexpected Character '";

    Scanner::Scanner(ErrorReporting& logger) : mErrorReporting(logger), mCurrentLine(1)
    {
        mTokens.reserve(64);
    }

    Scanner::~Scanner()
    {
    }

    void Scanner::Reset()
    {
        mTokens.clear();
        mCurrentLine = 1;
        mStart = nullptr;
        mCurrent = nullptr;
    }

    ORchestraToken Scanner::MakeToken(ORchestraTokenType tokenType)
    {
        return ORchestraToken(tokenType, mStart, static_cast<int>(mCurrent - mStart), mCurrentLine);
    }

    ORchestraToken Scanner::MakeErrorToken(const std::string_view& message, char symbol)
    {
        std::string errorString;
        errorString.reserve(message.size() + 2);
        errorString.append(message.data(), message.size());
        errorString.append(&symbol, 1);
        errorString.append("'", 1);

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

    bool Scanner::ScanFile(const std::string& data)
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
            const ORchestraToken token = MakeToken(ORchestraTokenType::EOL);
            mCurrentLine++;
            return token;
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

        case '$':
            return MakeToken(ORchestraTokenType::DOLLAR);

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
            const char* rest, ORchestraTokenType type)
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
#if _TEST
        case 't':
            return checkKeyword(1, 3, "est", ORchestraTokenType::TEST_KEYWORD);
#endif
        case 'p':
        {
            auto result = checkKeyword(1, 4, "rint", ORchestraTokenType::PRINT);
            if (result != ORchestraTokenType::IDENTIFIER)
                return result;
            return checkKeyword(1, 2, "tn", ORchestraTokenType::PTN);
        }
        case 'r':
        {
            auto result = checkKeyword(1, 2, "an", ORchestraTokenType::RANDOM);
            if (result != ORchestraTokenType::IDENTIFIER)
                return result;
            return checkKeyword(1, 5, "eturn", ORchestraTokenType::RETURN);
        }
        case 'e':
        {
            auto result = checkKeyword(1, 2, "uc", ORchestraTokenType::EUCLIDEAN);
            if (result != ORchestraTokenType::IDENTIFIER)
                return result;
            return checkKeyword(1, 2, "nd", ORchestraTokenType::END_FN);
        }
        case 'f':
            return checkKeyword(1, 1, "n", ORchestraTokenType::FN);
        case 'n':
        {
            auto r = checkKeyword(1, 1, "1",  ORchestraTokenType::BPM_DIVISION_IDENTIFIER);
            if (r != ORchestraTokenType::IDENTIFIER) return r;
            r = checkKeyword(1, 1, "2",  ORchestraTokenType::BPM_DIVISION_IDENTIFIER);
            if (r != ORchestraTokenType::IDENTIFIER) return r;
            r = checkKeyword(1, 1, "4",  ORchestraTokenType::BPM_DIVISION_IDENTIFIER);
            if (r != ORchestraTokenType::IDENTIFIER) return r;
            r = checkKeyword(1, 1, "8",  ORchestraTokenType::BPM_DIVISION_IDENTIFIER);
            if (r != ORchestraTokenType::IDENTIFIER) return r;
            r = checkKeyword(1, 2, "16", ORchestraTokenType::BPM_DIVISION_IDENTIFIER);
            if (r != ORchestraTokenType::IDENTIFIER) return r;
            r = checkKeyword(1, 2, "32", ORchestraTokenType::BPM_DIVISION_IDENTIFIER);
            if (r != ORchestraTokenType::IDENTIFIER) return r;
            return checkKeyword(1, 3, "ote", ORchestraTokenType::NOTE);
        }
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
