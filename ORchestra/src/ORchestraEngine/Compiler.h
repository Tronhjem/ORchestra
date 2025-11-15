#pragma once

#include <vector>
#include <unordered_map>

#include "ORchestraToken.h"
#include "Types.h"
#include "Instruction.h"
#include "StoredFunction.h"
#include "StepData.h"

namespace ORchestra {


class ErrorReporting;

class Compiler
{
public:
    Compiler(std::vector<ORchestraToken>& tokens, ErrorReporting& log);
    bool Compile(std::vector<Instruction>& runtimeInstructions);

private:
    inline ORchestraToken& Consume();
    inline ORchestraToken& Peek();
    inline ORchestraToken& PeekNext();
    inline ORchestraToken& Previous();

    inline void ThrowUnexpectedTokenError(ORchestraToken& tokenForError);
    inline void ThrowMissingExpectedToken(std::string &missingToken);
    inline void ThrowMissingParamCount(int expected, int received);
    inline void ThrowUnexpectedEnd(std::string &missingToken);

    inline bool MakeIdentifierGetter(ORchestraToken& token, std::vector<Instruction> &instructions);
    inline void MakeConstant(ORchestraToken& token, std::vector<Instruction> &instructions);
    bool MakeNoteIntoConstant(ORchestraToken& token, std::vector<Instruction> &instructions);
    inline void MakeOperation(ORchestraTokenType tokenType, std::vector<Instruction> &instructions);

    bool CompileExpression(std::vector<Instruction>& instructions);
    bool CompileArray(std::vector<Instruction>& instructions,
                      StepData &outLength,
                      int maxLength,
                      bool isLastRecursiveLevel);

    bool CompileFunctionCall(std::vector<Instruction>& instructions, std::string &functionName);

    unsigned long mCurrentIndex = 0;
    std::vector<ORchestraToken>& mTokens;
    ErrorReporting &mErrorReporting;
    std::unordered_map<std::string, StoredFunction> mFunctions;
};


} // namespace ORchestra
