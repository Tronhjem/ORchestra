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

#include <vector>
#include <unordered_map>

#include "ORchestraToken.h"
#include "Instruction.h"
#include "StoredFunction.h"


namespace ORchestra
{
    class ErrorReporting;

    enum class Precedence
    {
        NONE = 0,
        ASSIGNMENT,   // lowest: used as minimum for full expression parsing
        COMPARISON,   // > >= < <= == != & | ^
        TERM,         // + -
        FACTOR,       // * / %
        UNARY,        // unary -
    };

    class Compiler
    {
    public:
        Compiler(const std::vector<ORchestraToken>& tokens, ErrorReporting& log);
        bool Compile(std::vector<Instruction>& runtimeInstructions);
        void Reset();
        const std::vector<std::vector<std::vector<Instruction>>>& GetFunctionArrays() const { return mFunctionArrays; }
        std::vector<std::string> GetVariableNames() const;

    private:
        Compiler() = delete;

        inline const ORchestraToken& Consume();
        inline const ORchestraToken& Peek();
        inline const ORchestraToken& Previous();

        inline void ThrowUnexpectedTokenError(const ORchestraToken& tokenForError);
        inline void ThrowUnexpectedStart(const ORchestraToken& tokenForError);
        inline void ThrowMissingExpectedToken(const std::string& missingToken);
        inline void ThrowMissingParamCount(int expected, int received);
        inline void ThrowUnexpectedEnd(const std::string& missingToken);
        inline void ThrowUnknownFunctionOrVariable(const std::string& name);

        bool CompilePatternDefinition();
        inline bool MakeIdentifierGetter(const ORchestraToken& token, std::vector<Instruction>& instructions);
        inline void MakeConstant(const ORchestraToken& token, std::vector<Instruction>& instructions);
        bool MakeNoteIntoConstant(const ORchestraToken& token, std::vector<Instruction>& instructions);
        inline void MakeOperation(ORchestraTokenType tokenType, std::vector<Instruction>& instructions);

        // Pratt parser
        struct ParseRule;
        static ParseRule GetRule(ORchestraTokenType type);
        bool ParsePrecedence(Precedence minPrecedence, std::vector<Instruction>& instructions);
        bool ParseNumber(std::vector<Instruction>& instructions);
        bool ParseNoteIdentifier(std::vector<Instruction>& instructions);
        bool ParseIdentifier(std::vector<Instruction>& instructions);
        bool ParseGrouping(std::vector<Instruction>& instructions);
        bool ParseDollar(std::vector<Instruction>& instructions);
        bool ParseRandom(std::vector<Instruction>& instructions);
        bool ParseBinary(std::vector<Instruction>& instructions);
        bool ParseUnary(std::vector<Instruction>& instructions);

        bool CompileExpression(std::vector<Instruction>& instructions);
        bool CompileArray(
                std::vector<Instruction>& instructions,
                DataUnit& outLength,
                int maxLength,
                bool isLastRecursiveLevel);

        inline void BuildFunctions();
        bool CompileFunctionCall(std::vector<Instruction>& instructions, const std::string& functionName);
        bool CompileFunctionArray(const std::string& name);
        bool CompileFunctionArrayCall(std::vector<Instruction>& instructions, DataUnit arrayId);

        enum class StatementResult { SUCCESS, END_OF_INPUT, END_OF_FUNCTION, COMPILE_ERROR };
        StatementResult CompileStatement(std::vector<Instruction>& instructions);
        bool CompileFunctionDefinition(std::vector<Instruction>& mainInstructions);
        bool mInsideFunctionDefinition = false;

        inline DataUnit GetOrCreateVariableID(const std::string& varName);

        DataUnit mVariableIdCounter;
        unsigned long mCurrentIndex = 0;
        const std::vector<ORchestraToken>& mTokens;
        ErrorReporting& mErrorReporting;
        std::unordered_map<std::string, StoredFunction> mFunctions;
        std::unordered_map<std::string, StoredFunction> mPatterns;
        std::unordered_map<std::string, DataUnit> mVariableIDMap;
        std::vector<std::vector<std::vector<Instruction>>> mFunctionArrays;
        std::unordered_map<std::string, DataUnit> mFunctionArrayNames;
    };
} // namespace ORchestra
