
#include "ORchestraCodeEditorTokenizer.h"
#include "Colours.h"

namespace ORchestra
{
    struct ORchestraTokenizerFunctions
    {
        static bool isReservedKeyword (String::CharPointerType token, const int tokenLength) noexcept
        {
            static const char* const keywords2Char[] =
                { "cc", nullptr };

            static const char* const keywords3Char[] =
                { "euc", "ran",  "bpm", "div", nullptr };

            static const char* const keywords4Char[] =
                { "note",  nullptr };

            // static const char* const keywords5Char[] =
            //     {  "false", "local", "until", "while", "break", nullptr };
            //
            // static const char* const keywords6Char[] =
            //     { "repeat", "return", "elseif", nullptr};
            //
            
            static const char* const keywordsOther[] =
                { "function", "@interface", "@end", "@synthesize", "@dynamic", "@public",
                  "@private", "@property", "@protected", "@class", nullptr };

            const char* const* k;

            switch (tokenLength)
            {
                case 2:   k = keywords2Char; break;
                case 3:   k = keywords3Char; break;
                case 4:   k = keywords4Char; break;
                // case 5:   k = keywords5Char; break;
                // case 6:   k = keywords6Char; break;

                default:
                    if (tokenLength < 2 || tokenLength > 16)
                        return false;

                    k = keywordsOther;
                    break;
            }

            for (int i = 0; k[i] != nullptr; ++i)
                if (token.compare (CharPointer_ASCII (k[i])) == 0)
                    return true;

            return false;
        }

        template <typename Iterator>
        static int parseIdentifier (Iterator& source) noexcept
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

            if (tokenLength > 1 && tokenLength <= 16)
            {
                possible.writeNull();

                if (isReservedKeyword (String::CharPointerType (possibleIdentifier), tokenLength))
                    return ORchestraCodeEditorTokenizer::tokenType_keyword;
            }

            return ORchestraCodeEditorTokenizer::tokenType_identifier;
        }

        template <typename Iterator>
        static int readNextToken (Iterator& source)
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

            // case ';':
            // case ':':
            case ',':
                source.skip();
                return ORchestraCodeEditorTokenizer::tokenType_punctuation;

            // case '{':   case '}':
            case '(':   case ')':
            case '[':   case ']':
                source.skip();
                return ORchestraCodeEditorTokenizer::tokenType_bracket;

            // case '"':
            // case '\'':
            //     CppTokeniserFunctions::skipQuotedString (source);
            //     return ORchestraCodeEditorTokenizer::tokenType_string;

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
                    // CppTokeniserFunctions::skipIfNextCharMatches (source, '/', '=');
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
                // CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
                return ORchestraCodeEditorTokenizer::tokenType_operator;

            case '<':   case '>':
                source.skip();
                CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
                CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
                return ORchestraCodeEditorTokenizer::tokenType_operator;

            default:
                if (CppTokeniserFunctions::isIdentifierStart (firstChar))
                    return parseIdentifier (source);

                source.skip();
                break;
            }

            return ORchestraCodeEditorTokenizer::tokenType_error;
        }
    };


    int ORchestraCodeEditorTokenizer::readNextToken (CodeDocument::Iterator& source)
    {
        return ORchestraTokenizerFunctions::readNextToken (source);
    }

    CodeEditorComponent::ColourScheme ORchestraCodeEditorTokenizer::getDefaultColourScheme()
    {
        static const CodeEditorComponent::ColourScheme::TokenType types[] =
        {
            { "Error",          Colour (CodeColor::Error) },
            { "Comment",        Colour (CodeColor::Comment) },
            { "Keyword",        Colour (CodeColor::Keyword) },
            { "Operator",       Colour (CodeColor::Operator) },
            { "Identifier",     Colour (CodeColor::Identifier) },
            { "Integer",        Colour (CodeColor::Integer) },
            { "Float",          Colour (CodeColor::Float) },
            { "Bracket",        Colour (CodeColor::Bracket) },
            { "Punctuation",    Colour (CodeColor::Punctuation) }
        };

        CodeEditorComponent::ColourScheme cs;

        for (auto& t : types)
            cs.set (t.name, Colour (t.colour));

        return cs;
    }
} // namespace ORchestra
