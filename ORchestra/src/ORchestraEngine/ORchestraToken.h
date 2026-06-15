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

#pragma once

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
        DOLLAR,

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
        PRINT,
        RANDOM,
        EUCLIDEAN,
        PARSE_ERROR,
        NOTE_IDENTIFIER,
        BEAT_IDENTIFIER,

        // Specific
        //TRACK,
        NOTE,
        CC,

        // Function definition
        FN,
        END_FN,
        RETURN,

        // ENDS
        EOL, // End Of Line
        END,

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
