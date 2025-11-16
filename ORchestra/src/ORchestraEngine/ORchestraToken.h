#pragma once

#include <string>

namespace ORchestra {

    enum class ORchestraTokenType
    {
        // Single-character tokens.
        LEFT_PAREN = 0,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        COMMA,
        BANG,

        // MATH
        DOT,
        MINUS,
        PLUS,
        SLASH,
        STAR,
        PERCENT,


        // Literals.
        IDENTIFIER,
        STRING,
        NUMBER,

        // Logic
        AND,
        OR,
        XOR,

        EQUAL,
        EQUAL_EQUAL,
        BANG_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Keywords.
        RANDOM,
        EUCLIDEAN,
        PARSE_ERROR,
        NOTE_IDENTIFIER,

        // Specific
        //TRACK,
        NOTE,
        CC,

        // ENDS
        EOL, // End Of Line
        END,

        // DEBUGGING

#if _DEBUG
        PRINT,
#endif
#if _TEST
        TEST_KEYWORD,
#endif
        // UNUSED STUFF
        //    SEMICOLON,
        //    BANG,
        //    CLASS,
        //    ELSE,
        //    FALSE,
        //    FUN,
        //    FOR,
        //    IF,
        //    NIL,
        //    RETURN,
        //    SUPER,
        //    THIS,
        //    TRUE,
        //    VAR,
        //    WHILE,
    };

    class ORchestraToken
    {
    public:
        ORchestraToken(ORchestraTokenType tokenType,
            const char* start,
            int length,
            int line);

        ORchestraTokenType GetType() const { return mTokenType; }

        ORchestraTokenType mTokenType;
        const char* mStart;
        int mLength;
        int mLine;

    private:
        ORchestraToken() = delete;
    };

} // namespace ORchestra

