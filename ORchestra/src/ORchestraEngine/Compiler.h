#pragma once

#include <vector>
#include <unordered_map>

#include "ORchestraToken.h"
#include "Instruction.h"
#include "StoredFunction.h"
#include "StepData.h"
#include "Defines.h"

namespace ORchestra {


    class ErrorReporting;

    class Compiler
    {
    public:
        Compiler(const std::vector<ORchestraToken>& tokens, ErrorReporting& log);
        bool Compile(std::vector<Instruction>& runtimeInstructions);
        void Reset();

    private:
        Compiler() = delete;

        inline const ORchestraToken& Consume();
        inline const ORchestraToken& Peek();
        inline const ORchestraToken& PeekNext();
        inline const ORchestraToken& Previous();

        inline void ThrowUnexpectedTokenError(const ORchestraToken& tokenForError);
        inline void ThrowMissingExpectedToken(std::string& missingToken);
        inline void ThrowMissingParamCount(int expected, int received);
        inline void ThrowUnexpectedEnd(std::string& missingToken);

        inline bool MakeIdentifierGetter(const ORchestraToken& token, std::vector<Instruction>& instructions);
        inline void MakeConstant(const ORchestraToken& token, std::vector<Instruction>& instructions);
        bool MakeNoteIntoConstant(const ORchestraToken& token, std::vector<Instruction>& instructions);
        inline void MakeOperation(ORchestraTokenType tokenType, std::vector<Instruction>& instructions);

        bool CompileExpression(std::vector<Instruction>& instructions);
        bool CompileArray(
                std::vector<Instruction>& instructions,
                StepData& outLength,
                int maxLength,
                bool isLastRecursiveLevel);

        bool CompileFunctionCall(std::vector<Instruction>& instructions, const std::string& functionName);

        unsigned long mCurrentIndex = 0;
        const std::vector<ORchestraToken>& mTokens;
        ErrorReporting& mErrorReporting;
        std::unordered_map<std::string, StoredFunction> mFunctions;
    };


} // namespace ORchestra
