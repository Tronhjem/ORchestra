#include <string>
#include <cassert>
#include <stack>

#include "Compiler.h"
#include "ErrorReporting.h"
#include "ScopedTimer.h"
#include "StepData.h"
#include "Types.h"
#include "Defines.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

static std::string ranFunctionName = "ran";
static std::string eucFunctionName = "euc";

Compiler::Compiler(std::vector<Token> &tokens, ErrorReporting &log) : mTokens(tokens), mErrorReporting(log)
{
    // populate built in functions
    std::vector<Instruction> printInstructions;
#if _DEBUG
    printInstructions.emplace_back(Instruction{OpCode::PRINT});
    mFunctions["print"] = StoredFunction(1, printInstructions);
#endif

    std::vector<Instruction> noteInstructions;
    noteInstructions.emplace_back(Instruction{OpCode::NOTE});
    mFunctions["note"] = StoredFunction(4, noteInstructions);

    std::vector<Instruction> ccInstructions;
    ccInstructions.emplace_back(Instruction{OpCode::CC});
    mFunctions["cc"] = StoredFunction(4, ccInstructions);

    std::vector<Instruction> ranInstructions;
    ranInstructions.emplace_back(Instruction{OpCode::GET_RANDOM_IN_RANGE});
    mFunctions[ranFunctionName] = StoredFunction(2, ranInstructions);

    std::vector<Instruction> eucInstructions;
    eucInstructions.emplace_back(Instruction{OpCode::GENERATE_EUCLID_SEQUENCE});
    mFunctions[eucFunctionName] = StoredFunction(2, eucInstructions);
}

Token &Compiler::Consume()
{
    return mTokens[mCurrentIndex++];
}

Token &Compiler::Peek()
{
#if _DEBUG
    assert(mCurrentIndex < mTokens.size());
#endif
    return mTokens[mCurrentIndex];
}

Token &Compiler::Previous()
{
#if _DEBUG
    assert(mCurrentIndex < mTokens.size());
    assert(mCurrentIndex - 1 >= 0);
#endif
    return mTokens[mCurrentIndex - 1];
}

bool Compiler::MakeIdentifierGetter(Token &token, std::vector<Instruction> &instructions)
{
    std::string name = std::string(token.mStart, static_cast<unsigned long>(token.mLength));

    if (Peek().mTokenType == TokenType::LEFT_BRACKET)
    {
        Consume(); // for consuming left bracket

        CompileExpression(instructions);

        if (Peek().mTokenType != TokenType::RIGHT_BRACKET)
        {
            std::string missingBracket = "]";
            ThrowMissingExpectedToken(missingBracket);
            return false;
        }

        Consume(); // for consuming right bracket

        instructions.emplace_back(Instruction{OpCode::GET_IDENTIFIER_WITH_INDEX, name});
    }
    else
    {
        instructions.emplace_back(Instruction{OpCode::GET_IDENTIFIER_VALUE, name});
    }

    return true;
}

void Compiler::MakeConstant(Token &token, std::vector<Instruction> &instructions)
{
    int value = std::stoi(std::string(token.mStart, static_cast<unsigned long>(token.mLength)));
    if (value > 127)
    {
        value = 127;
        std::string message = std::string("Value can't be greater than 127, correcting to 127");
        mErrorReporting.LogWarning(message);
    }
    if (value < 0)
    {
        value = 0;
        std::string message = std::string("Value can't be smaller than 0, correcting to 0");
        mErrorReporting.LogWarning(message);
    }

    instructions.emplace_back(Instruction{OpCode::CONSTANT, static_cast<StepData>(value)});
}

bool Compiler::MakeNoteIntoConstant(Token &token, std::vector<Instruction> &instructions)
{
    auto noteToValue = [](char letter) -> DataUnit
    {
        switch (letter)
        {
        case 'C':
            return 12;
        case 'D':
            return 14;
        case 'E':
            return 16;
        case 'F':
            return 17;
        case 'G':
            return 19;
        case 'A':
            return 21;
        case 'B':
            return 23;
        default:
            return 255;
        }
    };

    DataUnit baseNote = noteToValue(*token.mStart);
    if (baseNote == 255 || token.mLength < 2)
    {
        std::string message = std::string("Notes need at least a capital letter for the note name, and an octave from 0-10. Optional is # or b");
        mErrorReporting.LogError(message);
        return false;
    }

    int octaveIndex = 1;
    if (token.mStart[1] == '#')
    {
        baseNote += 1;
        octaveIndex = 2;
    }
    else if (token.mStart[1] == 'b')
    {
        baseNote -= 1;
        octaveIndex = 2;
    }

    int value = -1;
    std::string numAsString;
    try
    {
        numAsString = std::string(token.mStart + octaveIndex, static_cast<unsigned long>(token.mLength - octaveIndex));
        value = std::stoi(numAsString) * 12 + baseNote;
    }
    catch (std::exception &err)
    {
        std::string message = std::string("Note Letters are rerved for notes, couldn't convert this to note: ") + numAsString;
        mErrorReporting.LogError(message);
        return false;
    }

    if (value > 127)
    {
        value = 127;
        std::string message = std::string("Value can't be greater than 127, correcting to 127");
        mErrorReporting.LogWarning(message);
    }
    if (value < 0)
    {
        value = 0;
        std::string message = std::string("Value can't be smaller than 0, correcting to 0");
        mErrorReporting.LogWarning(message);
    }

    instructions.emplace_back(Instruction{OpCode::CONSTANT, static_cast<StepData>(value)});

    return true;
}

void Compiler::MakeOperation(TokenType tokenType, std::vector<Instruction> &instructions)
{
    OpCode code = OpCode::END;
    switch (tokenType)
    {
    case TokenType::PLUS:
        code = OpCode::ADD;
        break;
    case TokenType::MINUS:
        code = OpCode::SUBTRACT;
        break;
    case TokenType::STAR:
        code = OpCode::MULTIPLY;
        break;
    case TokenType::SLASH:
        code = OpCode::DIVIDE;
        break;
    case TokenType::PERCENT:
        code = OpCode::MODULO;
        break;
    case TokenType::AND:
        code = OpCode::AND;
        break;
    case TokenType::OR:
        code = OpCode::OR;
        break;
    case TokenType::XOR:
        code = OpCode::XOR;
        break;
    case TokenType::GREATER:
        code = OpCode::GREATER;
        break;
    case TokenType::GREATER_EQUAL:
        code = OpCode::GREATER_EQUAL;
        break;
    case TokenType::LESS:
        code = OpCode::LESS;
        break;
    case TokenType::LESS_EQUAL:
        code = OpCode::LESS_EQUAL;
        break;
    case TokenType::EQUAL_EQUAL:
        code = OpCode::EQUAL;
        break;
    case TokenType::BANG_EQUAL:
        code = OpCode::NOT_EQUAL;
        break;

    default:
        break;
    }

    instructions.emplace_back(Instruction{code});
}

bool Compiler::CompileFunctionCall(std::vector<Instruction> &instructions, std::string &functionName)
{
    if (mFunctions.find(functionName) == mFunctions.end())
    {
        return false;
    }

    StoredFunction &functon = mFunctions[functionName];

    Consume(); // For Left Parenteses
    int paramCounter = 0;

    while (paramCounter != functon.mNumOfParams &&
           Peek().mTokenType != TokenType::RIGHT_PAREN)
    {
        Token &currentToken = Peek();
        switch (currentToken.mTokenType)
        {
        case TokenType::COMMA:
        {
            Consume();
            break;
        }

        case TokenType::END:
        case TokenType::EOL:
        {
            ThrowMissingParamCount(functon.mNumOfParams, paramCounter);
            return false;
        }

        default:
        {
            if (!CompileExpression(instructions))
            {
                ThrowUnexpectedTokenError(Peek());
                return false;
            }
            ++paramCounter;
            break;
        }
        }
    }

    if (Peek().mTokenType != TokenType::RIGHT_PAREN)
    {
        ThrowUnexpectedTokenError(Peek());
        return false;
    }

    if (functon.mNumOfParams != paramCounter)
    {
        ThrowMissingParamCount(functon.mNumOfParams, paramCounter);
        return false;
    }

    for (Instruction &funcInstruction : functon.mInstructions)
    {
        instructions.emplace_back(funcInstruction);
    }

    Consume(); // For Right Paren

    return true;
}

bool Compiler::CompileArray(std::vector<Instruction> &instructions,
                            StepData &outLength,
                            int maxLength,
                            bool isLastRecursiveLevel)
{
    Consume(); // For the Left Bracket

    if (isLastRecursiveLevel && Peek().mTokenType == TokenType::LEFT_BRACKET)
    {
        ThrowUnexpectedTokenError(Peek());
        return false;
    }

    if (Peek().mTokenType != TokenType::NUMBER &&
        Peek().mTokenType != TokenType::NOTE_IDENTIFIER &&
        Peek().mTokenType != TokenType::IDENTIFIER &&
        Peek().mTokenType != TokenType::RANDOM &&
        Peek().mTokenType != TokenType::LEFT_BRACKET)
    {
        ThrowUnexpectedTokenError(Peek());
        return false;
    }

    int valueCounter = 0;
    bool expectsValue = true;
    while (Peek().mTokenType != TokenType::RIGHT_BRACKET)
    {
        if (valueCounter >= maxLength)
        {
            std::string message = std::string("Max Length for array exceeded. Can't be longer than ") + std::to_string(maxLength);
            mErrorReporting.LogError(Peek().mLine, message);
            return false;
        }

        Token &currentToken = Peek();
        switch (currentToken.mTokenType)
        {
        case TokenType::COMMA:
        {
            if (expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            Consume();
            expectsValue = true;
            break;
        }

        case TokenType::LEFT_BRACKET:
        {
            if (!expectsValue || isLastRecursiveLevel)
            {
                return false;
            }

            StepData arrayLength;
            const bool isNextRecursionLast = true;
            if (CompileArray(instructions, arrayLength, MAX_SUB_DIVISION_LENGTH, isNextRecursionLast))
            {
                instructions.emplace_back(Instruction{OpCode::CONSTANT, arrayLength});
                instructions.emplace_back(Instruction{OpCode::SET_SUBSTEP_ARRAY});
            }
            else
            {
                return false;
            }

            ++valueCounter;
            expectsValue = false;
            break;
        }

        case TokenType::NUMBER:
        case TokenType::NOTE_IDENTIFIER:
        case TokenType::IDENTIFIER:
        case TokenType::LEFT_PAREN:
        {
            if (!expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            if (!CompileExpression(instructions))
            {
                return false;
            }

            ++valueCounter;
            expectsValue = false;
            break;
        }

        case TokenType::RANDOM:
        {
            if (!expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            Consume(); // Consumes the ran function token

            if (!CompileFunctionCall(instructions, ranFunctionName))
                return false;

            ++valueCounter;
            expectsValue = false;
            break;
        }

        case TokenType::EOL:
        case TokenType::END:
        {
            std::string missingToken{"]"};
            ThrowUnexpectedEnd(missingToken);
            return false;
        }

        default:
        {
            ThrowUnexpectedTokenError(currentToken);
            return false;
        }
        }
    }

    if (Consume().mTokenType != TokenType::RIGHT_BRACKET)
    {
        std::string missingToken{"]"};
        ThrowMissingExpectedToken(missingToken);
        return false;
    }

    // TODO: Find a way to make this nicer, maybe a operator overload for =
    const DataUnit data[1] = {static_cast<DataUnit>(valueCounter)};
    outLength.SetData(data, 1);

    return true;
}

bool Compiler::CompileExpression(std::vector<Instruction> &instructions)
{
    auto isOperator = [&](const TokenType t) -> bool
    {
        return t == TokenType::PLUS ||
               t == TokenType::MINUS ||
               t == TokenType::STAR ||
               t == TokenType::SLASH ||
               t == TokenType::AND ||
               t == TokenType::OR ||
               t == TokenType::XOR ||
               t == TokenType::GREATER ||
               t == TokenType::GREATER_EQUAL ||
               t == TokenType::LESS ||
               t == TokenType::LESS_EQUAL ||
               t == TokenType::EQUAL_EQUAL ||
               t == TokenType::BANG_EQUAL;
    };

    auto precedence = [&](const TokenType t) -> int
    {
        if (t == TokenType::PLUS || t == TokenType::MINUS)
            return 1;
        if (t == TokenType::STAR || t == TokenType::SLASH)
            return 2;

        return 0;
    };

    std::stack<TokenType> ops;

    bool expectsValue = Peek().mTokenType != TokenType::LEFT_PAREN;
    int leftParen = 0;
    int rightParen = 0;

    while (Peek().mTokenType != TokenType::EOL &&
           Peek().mTokenType != TokenType::END &&
           Peek().mTokenType != TokenType::COMMA &&
           Peek().mTokenType != TokenType::RIGHT_BRACKET &&
           Peek().mTokenType != TokenType::RIGHT_BRACE)
    {
        // We assume this means we found the end of a function parenteses.
        if (Peek().mTokenType == TokenType::RIGHT_PAREN &&
            rightParen == leftParen)
            break;

        Token &currentToken = Consume();
        const TokenType tType = currentToken.mTokenType;

        // For nested parenteses where there can be more left parens after each other.
        if (tType == TokenType::LEFT_PAREN)
            expectsValue = false;

        if (tType == TokenType::IDENTIFIER)
        {
            if (!expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            if (!MakeIdentifierGetter(currentToken, instructions))
                return false;

            expectsValue = false;
        }

        else if (tType == TokenType::NUMBER)
        {
            if (!expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            MakeConstant(currentToken, instructions);
            expectsValue = false;
        }
        else if (tType == TokenType::NOTE_IDENTIFIER)
        {
            if (!expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            if (!MakeNoteIntoConstant(currentToken, instructions))
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            expectsValue = false;
        }

        else if (tType == TokenType::RANDOM)
        {
            CompileFunctionCall(instructions, ranFunctionName);
            expectsValue = false;
        }

        else if (isOperator(tType))
        {
            if (expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            while (!ops.empty() && isOperator(ops.top()))
            {
                TokenType top = ops.top();

                if ((precedence(top) > precedence(tType)) ||
                    (precedence(top) == precedence(tType)))
                {
                    MakeOperation(top, instructions);
                    ops.pop();
                }
                else
                {
                    break;
                }
            }

            expectsValue = true;
            ops.push(tType);
        }

        else if (tType == TokenType::LEFT_PAREN)
        {
            if (expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }
            ops.push(tType);
            expectsValue = true;
            ++leftParen;
        }

        else if (tType == TokenType::RIGHT_PAREN)
        {
            if (expectsValue)
            {
                ThrowUnexpectedTokenError(currentToken);
                return false;
            }

            while (!ops.empty() && ops.top() != TokenType::LEFT_PAREN)
            {
                MakeOperation(ops.top(), instructions);
                ops.pop();
            }
            if (!ops.empty() && ops.top() == TokenType::LEFT_PAREN)
            {
                ops.pop(); // discard left paren
            }

            expectsValue = false;
            ++rightParen;
        }
        else
        {
            ThrowUnexpectedTokenError(currentToken);
            return false;
        }
    }

    if (expectsValue)
    {
        ThrowUnexpectedTokenError(Peek());
        return false;
    }

    if (leftParen > rightParen)
    {
        std::string missingToken{")"};
        ThrowMissingExpectedToken(missingToken);
        return false;
    }

    if (rightParen > leftParen)
    {
        std::string missingToken{"("};
        ThrowMissingExpectedToken(missingToken);
        return false;
    }

    while (!ops.empty())
    {
        MakeOperation(ops.top(), instructions);
        ops.pop();
    }

    return true;
}

bool Compiler::Compile(std::vector<Instruction> &instructions)
{
#if _DEBUG
    ScopedTimer timer("Compile");
#endif

    for (;;)
    {
        Token &token = Consume();

        switch (token.mTokenType)
        {
        case TokenType::IDENTIFIER:
        {
            std::string name = std::string(token.mStart, static_cast<unsigned long>(token.mLength));

            if (Peek().mTokenType == TokenType::EQUAL)
            {
                Consume(); // consumes the equal sign

                TokenType tokenType = Peek().mTokenType;
                switch (tokenType)
                {
                    // Data Array
                case TokenType::LEFT_BRACKET:
                {
                    StepData arrayLength;
                    const bool isLastRecursiveLevel = false;
                    if (CompileArray(instructions, arrayLength, MAX_DATASEQUENCE_LENGTH, isLastRecursiveLevel))
                    {
                        instructions.emplace_back(Instruction{OpCode::CONSTANT, arrayLength});
                        instructions.emplace_back(Instruction{OpCode::SET_IDENTIFIER_ARRAY, name});
                    }
                    else
                    {
                        return false;
                    }

                    break;
                }
                case TokenType::NUMBER:
                case TokenType::NOTE_IDENTIFIER:
                case TokenType::IDENTIFIER:
                case TokenType::LEFT_PAREN:
                case TokenType::RANDOM:
                {
                    if (!CompileExpression(instructions))
                        return false;

                    instructions.emplace_back(Instruction{OpCode::SET_IDENTIFIER_VALUE, name});
                    break;
                }
                case TokenType::EUCLIDEAN:
                {
                    Consume();
                    if (!CompileFunctionCall(instructions, eucFunctionName))
                        return false;

                    instructions.emplace_back(Instruction{OpCode::SET_IDENTIFIER_ARRAY, name});

                    break;
                }

                default:
                {
                    ThrowUnexpectedTokenError(Peek());
                    return false;
                }
                }
            }
            // For Functions
            else if (Peek().mTokenType == TokenType::LEFT_PAREN)
            {
                if (!CompileFunctionCall(instructions, name))
                    return false;
            }
            else
            {
                ThrowUnexpectedTokenError(Peek());
                return false;
            }

            break;
        }

#if _DEBUG
        case TokenType::PRINT:
#endif
        case TokenType::NOTE:
        case TokenType::CC:
        {
            std::string functionName = std::string(token.mStart, static_cast<unsigned long>(token.mLength));
            if (!CompileFunctionCall(instructions, functionName))
                return false;

            break;
        }

#if _TEST
        case TokenType::TEST:
        {
            if (Peek().mTokenType == TokenType::IDENTIFIER || Peek().mTokenType == TokenType::NUMBER)
            {
                CompileExpression(instructions);
            }
            else
            {
                ThrowUnexpectedTokenError(Peek());
                return false;
            }
            break;
        }
#endif

        case TokenType::END:
            instructions.emplace_back(Instruction{OpCode::END});
            return true;

        case TokenType::EOL:
            break;

        default:
            ThrowUnexpectedTokenError(token);
            return false;
        }
    }
}

void Compiler::ThrowUnexpectedTokenError(Token &tokenForError)
{
    std::string token = std::string(tokenForError.mStart, static_cast<unsigned long>(tokenForError.mLength));
    std::string message = std::string("Unexpected Character ") + token;
    mErrorReporting.LogError(Peek().mLine, message);
}

void Compiler::ThrowMissingExpectedToken(std::string &missingToken)
{
    std::string message = std::string("Missing a ") + missingToken;
    mErrorReporting.LogError(Peek().mLine, message);
}

void Compiler::ThrowMissingParamCount(int expected, int received)
{
    std::string message = std::string("Expected ") +
                          std::to_string(expected) +
                          std::string(" but received ") +
                          std::to_string(received);

    mErrorReporting.LogError(Peek().mLine, message);
}

void Compiler::ThrowUnexpectedEnd(std::string &missingToken)
{
    std::string message = "Unexpected end, you're missing a " + missingToken;
    mErrorReporting.LogError(Peek().mLine, message);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
