#pragma once

#include <vector>
#include <unordered_map>

#include "Token.h"
#include "Types.h"
#include "Instruction.h"
#include "StoredFunction.h"

class ErrorReporting;

class Compiler
{
public:
    Compiler(std::vector<Token>& tokens, ErrorReporting& log);
    bool Compile(std::vector<Instruction>& runtimeInstructions);

private:
    Token& Consume();
    Token& Peek();
    Token& PeekNext();
    Token& Previous();
    
    inline void ThrowUnexpectedTokenError(Token& tokenForError);
    inline void ThrowMissingExpectedToken(std::string& missingToken);
    inline void ThrowMissingParamCount(int expected, int received);
    inline void ThrowUnexpectedEnd(std::string& missingToken);
    
    inline bool MakeIdentifierGetter(Token& token, std::vector<Instruction>& instructions);
    inline void MakeConstant(Token& token, std::vector<Instruction>& instructions);
    bool MakeNoteIntoConstant(Token& token, std::vector<Instruction>& instructions);
    inline void MakeOperation(TokenType tokenType, std::vector<Instruction>& instructions);

    bool CompileExpression(std::vector<Instruction>& instructions);
    bool CompileArray(std::vector<Instruction>& instructions,
                      StepData& outLength,
                      int maxLength,
                      bool isLastRecursiveLevel);
    
    bool CompileFunctionCall(std::vector<Instruction>& instructions, std::string& functionName);

    int mCurrentIndex = 0;
    std::vector<Token>& mTokens;
    ErrorReporting& mErrorReporting;
    std::unordered_map<std::string, StoredFunction> mFunctions;
};
