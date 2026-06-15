/*
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

#include "ORchestraCodeEditorTokenizer.h"
#include "Colors.h"

namespace ORchestra
{
    static bool isNoteLetter (juce_wchar c) noexcept
    {
        return (c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
                c == 'G' || c == 'A' || c == 'B');
    }

    static bool isDigitChar (juce_wchar c) noexcept
    {
        return c >= '0' && c <= '9';
    }

    // Returns tokenType_keyword for structural keywords (fn, end, ptn).
    // Returns tokenType_function for callable builtins and user-defined.
    // Returns -1 if not a keyword at all.
    static int classifyKeyword (String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char* const structural2Char[] = { "fn", nullptr };
        static const char* const structural3Char[] = { "end", "ptn", nullptr };

        static const char* const function2Char[]  = { "cc", nullptr };
        static const char* const function3Char[]  = { "euc", "ran", "bpm", nullptr };
        static const char* const function4Char[]  = { "note", "beat", nullptr };
        static const char* const function5Char[]  = { "print", nullptr };
        static const char* const function6Char[]  = { "return", nullptr };
        static const char* const function9Char[]  = { "transpose", nullptr };

        const char* const* k = nullptr;

        switch (tokenLength)
        {
            case 2: k = structural2Char; break;
            case 3: k = structural3Char; break;
            default: break;
        }

        if (k != nullptr)
        {
            for (int i = 0; k[i] != nullptr; ++i)
                if (token.compare (CharPointer_ASCII (k[i])) == 0)
                    return ORchestraCodeEditorTokenizer::tokenType_keyword;
        }

        k = nullptr;
        switch (tokenLength)
        {
            case 2: k = function2Char; break;
            case 3: k = function3Char; break;
            case 4: k = function4Char; break;
            case 5: k = function5Char; break;
            case 6: k = function6Char; break;
            case 9: k = function9Char; break;
            default: break;
        }

        if (k != nullptr)
        {
            for (int i = 0; k[i] != nullptr; ++i)
                if (token.compare (CharPointer_ASCII (k[i])) == 0)
                    return ORchestraCodeEditorTokenizer::tokenType_function;
        }

        return -1;
    }

    void ORchestraCodeEditorTokenizer::scanForUserFunctions (const String& documentContent)
    {
        mUserFunctionNames.clear();

        const auto text = documentContent.toStdString();
        const size_t len = text.size();
        size_t pos = 0;

        while (pos < len)
        {
            // Find next occurrence of "fn " or "ptn " at start of line
            while (pos < len && text[pos] != '\n' && text[pos] != 'f' && text[pos] != 'p')
                ++pos;

            if (pos >= len) break;

            // Skip newlines
            if (text[pos] == '\n') { ++pos; continue; }

            // Check for "fn " or "ptn " preceded by start-of-line or whitespace
            bool atLineStart = (pos == 0 || text[pos - 1] == '\n');
            if (!atLineStart) { ++pos; continue; }

            const char* rest = text.c_str() + pos;
            bool isFn = (len - pos >= 3 && strncmp (rest, "fn ", 3) == 0);
            bool isPtn = (len - pos >= 4 && strncmp (rest, "ptn ", 4) == 0);

            if (!isFn && !isPtn) { ++pos; continue; }

            // Advance past "fn " or "ptn "
            pos += isFn ? 3 : 4;

            // Skip whitespace
            while (pos < len && (text[pos] == ' ' || text[pos] == '\t'))
                ++pos;

            // Read the identifier (function/pattern name)
            size_t nameStart = pos;
            while (pos < len && (juce::CharacterFunctions::isLetterOrDigit (static_cast<juce_wchar> (text[pos]))
                                 || text[pos] == '_'))
                ++pos;

            if (pos > nameStart)
            {
                const String name (text.c_str() + nameStart, pos - nameStart);
                if (name.isNotEmpty() && !mUserFunctionNames.contains (name))
                    mUserFunctionNames.add (name);
            }
        }
    }

    void ORchestraCodeEditorTokenizer::syncWithDocument (const CodeDocument& doc)
    {
        const auto content = doc.getAllContent();
        const juce::int64 hash = content.hashCode64();
        if (hash == mLastDocumentHash)
            return;

        mLastDocumentHash = hash;
        scanForUserFunctions (content);
    }

    struct ORchestraTokenizerFunctions
    {
        template <typename Iterator>
        static int parseNoteIdentifier (Iterator& source) noexcept
        {
            auto c = source.peekNextChar();
            if (c == '#' || c == 'b')
                source.skip();

            while (isDigitChar (source.peekNextChar()))
                source.skip();

            return ORchestraCodeEditorTokenizer::tokenType_note;
        }

        template <typename Iterator>
        static int parseIdentifier (Iterator& source, ORchestraCodeEditorTokenizer& tokenizer) noexcept
        {
            int tokenLength = 0;
            String::CharPointerType::CharType possibleIdentifier[100] = {};
            String::CharPointerType possible (possibleIdentifier);

            while (CppTokeniserFunctions::isIdentifierBody (source.peekNextChar()))
            {
                auto c = source.nextChar();

                if (tokenLength < 20)
                    possible.write (c);

                ++tokenLength;
            }

            if (tokenLength >= 1 && tokenLength <= 16)
            {
                possible.writeNull();

                int keywordType = classifyKeyword (String::CharPointerType (possibleIdentifier), tokenLength);
                if (keywordType >= 0)
                    return keywordType;

                // Check user-defined function/pattern names
                const String name (possibleIdentifier, static_cast<size_t> (tokenLength));
                if (tokenizer.isUserFunctionName (name))
                    return ORchestraCodeEditorTokenizer::tokenType_function;
            }

            return ORchestraCodeEditorTokenizer::tokenType_identifier;
        }

        template <typename Iterator>
        static int readNextToken (Iterator& source, ORchestraCodeEditorTokenizer& tokenizer)
        {
            source.skipWhitespace();

            auto firstChar = source.peekNextChar();

            switch (firstChar)
            {
            case 0:
                break;

            case '0':   case '1':   case '2':   case '3':   case '4':
            case '5':   case '6':   case '7':   case '8':   case '9':
            case '.':
            {
                auto result = CppTokeniserFunctions::parseNumber (source);

                if (result == ORchestraCodeEditorTokenizer::tokenType_error)
                {
                    source.skip();

                    if (firstChar == '.')
                        return ORchestraCodeEditorTokenizer::tokenType_punctuation;
                }

                return result;
            }

            case ',':
                source.skip();
                return ORchestraCodeEditorTokenizer::tokenType_punctuation;

            case '(':   case ')':
            case '[':   case ']':
                source.skip();
                return ORchestraCodeEditorTokenizer::tokenType_bracket;

            case '+':
                source.skip();
                CppTokeniserFunctions::skipIfNextCharMatches (source, '+', '=');
                return ORchestraCodeEditorTokenizer::tokenType_operator;

            case '/':
            {
                source.skip();
                auto result = CppTokeniserFunctions::parseNumber (source);

                if (source.peekNextChar() == '/')
                {
                    source.skipToEndOfLine();
                    return ORchestraCodeEditorTokenizer::tokenType_comment;
                }

                if (result == ORchestraCodeEditorTokenizer::tokenType_error)
                {
                    CppTokeniserFunctions::skipIfNextCharMatches (source, '/');
                    return ORchestraCodeEditorTokenizer::tokenType_operator;
                }

                return result;
            }

            case '*':
            case '-':
            case '%':
            case '=':
            case '$':
            case '|':
            case '&':
            case '^':
                source.skip();
                return ORchestraCodeEditorTokenizer::tokenType_operator;

            case '<':   case '>':
                source.skip();
                CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
                CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
                return ORchestraCodeEditorTokenizer::tokenType_operator;

            default:
                if (isNoteLetter (firstChar))
                {
                    source.nextChar ();
                    auto second = source.peekNextChar ();
                    if (second == '#' || second == 'b' || isDigitChar (second))
                        return parseNoteIdentifier (source);
                }

                if (CppTokeniserFunctions::isIdentifierStart (firstChar))
                    return parseIdentifier (source, tokenizer);

                source.skip();
                break;
            }

            return ORchestraCodeEditorTokenizer::tokenType_error;
        }
    };


    int ORchestraCodeEditorTokenizer::readNextToken (CodeDocument::Iterator& source)
    {
        return ORchestraTokenizerFunctions::readNextToken (source, *this);
    }

    CodeEditorComponent::ColourScheme ORchestraCodeEditorTokenizer::getDefaultColourScheme()
    {
        static const CodeEditorComponent::ColourScheme::TokenType types[] =
        {
            { "Error",          Colour (CodeColor::Error) },
            { "Comment",        Colour (CodeColor::Comment) },
            { "Keyword",        Colour (CodeColor::Keyword) },
            { "Function",       Colour (CodeColor::Function) },
            { "Operator",       Colour (CodeColor::Operator) },
            { "Identifier",     Colour (CodeColor::Identifier) },
            { "Integer",        Colour (CodeColor::Integer) },
            { "Float",          Colour (CodeColor::Float) },
            { "String",         Colour (CodeColor::String) },
            { "Bracket",        Colour (CodeColor::Bracket) },
            { "Punctuation",    Colour (CodeColor::Punctuation) },
            { "Note",           Colour (CodeColor::Note) }
        };

        CodeEditorComponent::ColourScheme cs;

        for (auto& t : types)
            cs.set (t.name, Colour (t.colour));

        return cs;
    }
} // namespace ORchestra
