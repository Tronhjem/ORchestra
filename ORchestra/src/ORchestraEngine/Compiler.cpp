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

#include <string>

#include "Compiler.h"
#include "ErrorReporting.h"
#include "Defines.h"

#if _DEBUG
#include "ScopedTimer.h"
#endif

namespace ORchestra
{

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

    static const std::string ranFunctionName = "ran";
    static const std::string eucFunctionName = "euc";
    static const std::string bpmFunctionName = "bpm";
    static const std::string noteDivFunctionName = "noteDiv";

    Compiler::Compiler(const std::vector<ORchestraToken>& tokens, ErrorReporting& log) :
            mVariableIdCounter(0),
            mTokens(tokens),
            mErrorReporting(log)
    {
        mVariableIDMap.reserve(8); // Magic number for size of variables estimated as a good start.

        BuildFunctions();
    }

    void Compiler::BuildFunctions()
    {
        // populate built in functions
        std::vector<Instruction> printInstructions;
        printInstructions.emplace_back(Instruction{ OpCode::PRINT });
        mFunctions["print"] = StoredFunction(1, printInstructions);

        std::vector<Instruction> noteInstructions;
        noteInstructions.emplace_back(Instruction{ OpCode::NOTE });
        mFunctions["note"] = StoredFunction(4, noteInstructions);

        std::vector<Instruction> ccInstructions;
        ccInstructions.emplace_back(Instruction{ OpCode::CC });
        mFunctions["cc"] = StoredFunction(4, ccInstructions);

        std::vector<Instruction> ranInstructions;
        ranInstructions.emplace_back(Instruction{ OpCode::GET_RANDOM_IN_RANGE });
        mFunctions[ranFunctionName] = StoredFunction(2, ranInstructions);

        std::vector<Instruction> eucInstructions;
        eucInstructions.emplace_back(Instruction{ OpCode::GENERATE_EUCLID_SEQUENCE });
        mFunctions[eucFunctionName] = StoredFunction(2, eucInstructions);

        std::vector<Instruction> bpmInstructions;
        bpmInstructions.emplace_back(Instruction{ OpCode::SET_BPM });
        mFunctions[bpmFunctionName] = StoredFunction(1, bpmInstructions);

        std::vector<Instruction> noteDivInstructions;
        noteDivInstructions.emplace_back(Instruction{ OpCode::SET_NOTE_DIVISION });
        mFunctions[noteDivFunctionName] = StoredFunction(1, noteDivInstructions);
    }

    void Compiler::Reset()
    {
        mCurrentIndex = 0;
        mVariableIDMap.clear();
        
        mFunctions.clear();
        BuildFunctions();
        
        mVariableIdCounter = 0;
        mFunctionArrays.clear();
        mFunctionArrayNames.clear();
        mInsideFunctionDefinition = false;
    }

    const ORchestraToken& Compiler::Consume()
    {
        return mTokens[mCurrentIndex++];
    }

    const ORchestraToken& Compiler::Peek()
    {
#if _DEBUG
        assert(mCurrentIndex < mTokens.size());
#endif
        return mTokens[mCurrentIndex];
    }

    const ORchestraToken& Compiler::Previous()
    {
#if _DEBUG
        assert(mCurrentIndex < mTokens.size());
        assert(mCurrentIndex - 1 >= 0);
#endif
        return mTokens[mCurrentIndex - 1];
    }

    bool Compiler::MakeIdentifierGetter(const ORchestraToken& token, std::vector<Instruction>& instructions)
    {
        const std::string name = std::string(token.mStart, static_cast<unsigned long>(token.mLength));
        const DataUnit id = GetOrCreateVariableID(name);

        if (Peek().mTokenType == ORchestraTokenType::LEFT_BRACKET)
        {
            Consume(); // for consuming left bracket

            CompileExpression(instructions);

            if (Peek().mTokenType != ORchestraTokenType::RIGHT_BRACKET)
            {
                std::string missingBracket = "]";
                ThrowMissingExpectedToken(missingBracket);
                return false;
            }

            Consume(); // for consuming right bracket

            instructions.emplace_back(Instruction{ OpCode::GET_IDENTIFIER_WITH_INDEX, id });
        }
        else
        {
            instructions.emplace_back(Instruction{ OpCode::GET_IDENTIFIER_VALUE, id });
        }

        return true;
    }

    void Compiler::MakeConstant(const ORchestraToken& token, std::vector<Instruction>& instructions)
    {
        int value = std::stoi(std::string(token.mStart, static_cast<unsigned long>(token.mLength)));
        if (value > DATA_UNIT_MAX_VALUE)
        {
            value = DATA_UNIT_MAX_VALUE;
            const std::string message = std::string("Value can't be greater than 255, correcting to 255");
            mErrorReporting.LogWarning(message);
        }
        if (value < DATA_UNIT_MIN_VALUE)
        {
            value = DATA_UNIT_MIN_VALUE;
            const std::string message = std::string("Value can't be smaller than 0, correcting to 0");
            mErrorReporting.LogWarning(message);
        }

        instructions.emplace_back(Instruction{ OpCode::CONSTANT, static_cast<DataUnit>(value) });
    }

    bool Compiler::MakeNoteIntoConstant(const ORchestraToken& token, std::vector<Instruction>& instructions)
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
        catch (std::exception& err)
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

        instructions.emplace_back(Instruction{ OpCode::CONSTANT, static_cast<DataUnit>(value) });

        return true;
    }

    void Compiler::MakeOperation(ORchestraTokenType tokenType, std::vector<Instruction>& instructions)
    {
        OpCode code = OpCode::END;
        switch (tokenType)
        {
        case ORchestraTokenType::PLUS:
            code = OpCode::ADD;
            break;
        case ORchestraTokenType::MINUS:
            code = OpCode::SUBTRACT;
            break;
        case ORchestraTokenType::STAR:
            code = OpCode::MULTIPLY;
            break;
        case ORchestraTokenType::SLASH:
            code = OpCode::DIVIDE;
            break;
        case ORchestraTokenType::PERCENT:
            code = OpCode::MODULO;
            break;
        case ORchestraTokenType::AND:
            code = OpCode::AND;
            break;
        case ORchestraTokenType::OR:
            code = OpCode::OR;
            break;
        case ORchestraTokenType::XOR:
            code = OpCode::XOR;
            break;
        case ORchestraTokenType::GREATER:
            code = OpCode::GREATER;
            break;
        case ORchestraTokenType::GREATER_EQUAL:
            code = OpCode::GREATER_EQUAL;
            break;
        case ORchestraTokenType::LESS:
            code = OpCode::LESS;
            break;
        case ORchestraTokenType::LESS_EQUAL:
            code = OpCode::LESS_EQUAL;
            break;
        case ORchestraTokenType::EQUAL_EQUAL:
            code = OpCode::EQUAL;
            break;
        case ORchestraTokenType::BANG_EQUAL:
            code = OpCode::NOT_EQUAL;
            break;

        default:
            break;
        }

        instructions.emplace_back(Instruction{ code });
    }

    bool Compiler::CompileFunctionCall(std::vector<Instruction>& instructions, const std::string& functionName)
    {
        if (mFunctions.find(functionName) == mFunctions.end())
        {
            ThrowUnknownFunctionOrVariable(functionName);
            return false;
        }

        const StoredFunction& function = mFunctions[functionName];

        Consume(); // For Left Parenteses
        int paramCounter = 0;
        bool expectsValue = true;

        while (Peek().mTokenType != ORchestraTokenType::RIGHT_PAREN)
        {
            const ORchestraToken& currentToken = Peek();
            switch (currentToken.mTokenType)
            {
            case ORchestraTokenType::COMMA:
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

            case ORchestraTokenType::END:
            case ORchestraTokenType::EOL:
            {
                const std::string missingRightParen = ")";
                ThrowUnexpectedEnd(missingRightParen);
                return false;
            }

            default:
            {
                if (!expectsValue)
                {
                    ThrowUnexpectedTokenError(currentToken);
                    return false;
                }

                if (!CompileExpression(instructions))
                {
                    ThrowUnexpectedTokenError(Peek());
                    return false;
                }
                ++paramCounter;
                expectsValue = false;
                break;
            }
            }
        }

        if (expectsValue && paramCounter != function.mNumOfParams)
        {
            ThrowMissingParamCount(function.mNumOfParams, paramCounter);
            return false;
        }

        if(Peek().mTokenType != ORchestraTokenType::RIGHT_PAREN)
        {
            const std::string missingRightParen = ")";
            ThrowUnexpectedEnd(missingRightParen);
            return false;
        }

        if (function.mNumOfParams != paramCounter)
        {
            ThrowMissingParamCount(function.mNumOfParams, paramCounter);
            return false;
        }

        // Bind parameters in reverse order (stack is LIFO)
        for (int i = static_cast<int>(function.mParamIds.size()) - 1; i >= 0; --i)
        {
            instructions.emplace_back(Instruction{ OpCode::UPDATE_IDENTIFIER_VALUE, function.mParamIds[i] });
        }

        for (const Instruction& funcInstruction : function.mInstructions)
        {
            instructions.emplace_back(funcInstruction);
        }

        Consume(); // For Right Paren

        return true;
    }

    bool Compiler::CompileArray(std::vector<Instruction>& instructions,
        DataUnit& outLength,
        int maxLength,
        bool isLastRecursiveLevel)
    {
        Consume(); // For the Left Bracket

        if (isLastRecursiveLevel && Peek().mTokenType == ORchestraTokenType::LEFT_BRACKET)
        {
            ThrowUnexpectedTokenError(Peek());
            return false;
        }

        if (Peek().mTokenType != ORchestraTokenType::NUMBER &&
            Peek().mTokenType != ORchestraTokenType::NOTE_IDENTIFIER &&
            Peek().mTokenType != ORchestraTokenType::IDENTIFIER &&
            Peek().mTokenType != ORchestraTokenType::RANDOM &&
            Peek().mTokenType != ORchestraTokenType::DOLLAR &&
            Peek().mTokenType != ORchestraTokenType::LEFT_BRACKET)
        {
            ThrowUnexpectedTokenError(Peek());
            return false;
        }

        int valueCounter = 0;
        bool expectsValue = true;
        while (Peek().mTokenType != ORchestraTokenType::RIGHT_BRACKET)
        {
            if (valueCounter >= maxLength)
            {
                std::string message = std::string("Max Length for array exceeded. Can't be longer than ") + std::to_string(maxLength);
                mErrorReporting.LogError(Peek().mLine, message);
                return false;
            }

            const ORchestraToken& currentToken = Peek();
            switch (currentToken.mTokenType)
            {
            case ORchestraTokenType::COMMA:
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

            case ORchestraTokenType::LEFT_BRACKET:
            {
                if (!expectsValue || isLastRecursiveLevel)
                {
                    return false;
                }

                DataUnit arrayLength;
                const bool isNextRecursionLast = true;
                if (CompileArray(instructions, arrayLength, MAX_SUB_DIVISION_LENGTH, isNextRecursionLast))
                {
                    instructions.emplace_back(Instruction{ OpCode::CONSTANT, arrayLength });
                    instructions.emplace_back(Instruction{ OpCode::SET_SUBSTEP_ARRAY });
                }
                else
                {
                    return false;
                }

                ++valueCounter;
                expectsValue = false;
                break;
            }

            case ORchestraTokenType::NUMBER:
            case ORchestraTokenType::NOTE_IDENTIFIER:
            case ORchestraTokenType::IDENTIFIER:
            case ORchestraTokenType::LEFT_PAREN:
            case ORchestraTokenType::DOLLAR:
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

            case ORchestraTokenType::RANDOM:
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

            case ORchestraTokenType::EOL:
            case ORchestraTokenType::END:
            {
                std::string missingToken{ "]" };
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

        if (expectsValue)
        {
            const ORchestraToken& previousToken = Previous();
            const ORchestraToken& unexpectedToken = previousToken.mTokenType == ORchestraTokenType::COMMA ? previousToken : Peek();
            ThrowUnexpectedTokenError(unexpectedToken);
            return false;
        }

        if (Consume().mTokenType != ORchestraTokenType::RIGHT_BRACKET)
        {
            std::string missingToken{ "]" };
            ThrowMissingExpectedToken(missingToken);
            return false;
        }

        outLength = static_cast<DataUnit>(valueCounter);

        return true;
    }

    // Pratt parser: parse rule table
    struct Compiler::ParseRule
    {
        using PrefixFn = bool (Compiler::*)(std::vector<Instruction>&);
        using InfixFn = bool (Compiler::*)(std::vector<Instruction>&);

        PrefixFn prefix;
        InfixFn infix;
        Precedence precedence;
    };

    Compiler::ParseRule Compiler::GetRule(ORchestraTokenType type)
    {
        switch (type)
        {
        case ORchestraTokenType::NUMBER:          
            return { &Compiler::ParseNumber, nullptr, Precedence::NONE };
        case ORchestraTokenType::IDENTIFIER:      
            return { &Compiler::ParseIdentifier, nullptr, Precedence::NONE };
        case ORchestraTokenType::NOTE_IDENTIFIER: 
            return { &Compiler::ParseNoteIdentifier, nullptr, Precedence::NONE };
        case ORchestraTokenType::DOLLAR:          
            return { &Compiler::ParseDollar, nullptr, Precedence::NONE };
        case ORchestraTokenType::RANDOM:          
            return { &Compiler::ParseRandom, nullptr, Precedence::NONE };
        case ORchestraTokenType::LEFT_PAREN:      
            return { &Compiler::ParseGrouping, nullptr, Precedence::NONE };

        case ORchestraTokenType::PLUS:            
            return { nullptr, &Compiler::ParseBinary, Precedence::TERM };
        case ORchestraTokenType::MINUS:           
            return { nullptr, &Compiler::ParseBinary, Precedence::TERM };
        case ORchestraTokenType::STAR:            
            return { nullptr, &Compiler::ParseBinary, Precedence::FACTOR };
        case ORchestraTokenType::SLASH:           
            return { nullptr, &Compiler::ParseBinary, Precedence::FACTOR };
        case ORchestraTokenType::PERCENT:         
            return { nullptr, &Compiler::ParseBinary, Precedence::FACTOR };

        case ORchestraTokenType::AND:             
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        case ORchestraTokenType::OR:              
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        case ORchestraTokenType::XOR:             
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        case ORchestraTokenType::GREATER:         
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        case ORchestraTokenType::GREATER_EQUAL:   
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        case ORchestraTokenType::LESS:            
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        case ORchestraTokenType::LESS_EQUAL:      
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        case ORchestraTokenType::EQUAL_EQUAL:     
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        case ORchestraTokenType::BANG_EQUAL:       
            return { nullptr, &Compiler::ParseBinary, Precedence::COMPARISON };
        default:                                   
            return { nullptr, nullptr, Precedence::NONE };
        }
    }

    // Pratt parser core: parse at given minimum precedence level
    bool Compiler::ParsePrecedence(Precedence minPrecedence, std::vector<Instruction>& instructions)
    {
        Consume();
        const ORchestraToken& prefixToken = Previous();
        ParseRule rule = GetRule(prefixToken.mTokenType);

        if (!rule.prefix)
        {
            ThrowUnexpectedTokenError(prefixToken);
            return false;
        }

        if (!(this->*rule.prefix)(instructions))
            return false;

        while (minPrecedence <= GetRule(Peek().mTokenType).precedence)
        {
            Consume();
            ParseRule infixRule = GetRule(Previous().mTokenType);
            if (!(this->*infixRule.infix)(instructions))
                return false;
        }

        return true;
    }

    bool Compiler::ParseNumber(std::vector<Instruction>& instructions)
    {
        MakeConstant(Previous(), instructions);
        return true;
    }

    bool Compiler::ParseNoteIdentifier(std::vector<Instruction>& instructions)
    {
        return MakeNoteIntoConstant(Previous(), instructions);
    }

    bool Compiler::ParseIdentifier(std::vector<Instruction>& instructions)
    {
        return MakeIdentifierGetter(Previous(), instructions);
    }

    bool Compiler::ParseGrouping(std::vector<Instruction>& instructions)
    {
        if (!ParsePrecedence(Precedence::ASSIGNMENT, instructions))
            return false;

        if (Peek().mTokenType != ORchestraTokenType::RIGHT_PAREN)
        {
            std::string missingToken{ ")" };
            ThrowMissingExpectedToken(missingToken);
            return false;
        }

        Consume();
        return true;
    }

    bool Compiler::ParseDollar(std::vector<Instruction>& instructions)
    {
        instructions.emplace_back(Instruction{ OpCode::GET_GLOBAL_COUNT });
        return true;
    }

    bool Compiler::ParseRandom(std::vector<Instruction>& instructions)
    {
        return CompileFunctionCall(instructions, ranFunctionName);
    }

    bool Compiler::ParseBinary(std::vector<Instruction>& instructions)
    {
        ORchestraTokenType operatorType = Previous().mTokenType;
        ParseRule rule = GetRule(operatorType);

        // +1 for left-associativity: parse right operand at one level higher
        Precedence nextPrecedence = static_cast<Precedence>(static_cast<int>(rule.precedence) + 1);
        if (!ParsePrecedence(nextPrecedence, instructions))
            return false;

        MakeOperation(operatorType, instructions);
        return true;
    }

    bool Compiler::CompileExpression(std::vector<Instruction>& instructions)
    {
        return ParsePrecedence(Precedence::ASSIGNMENT, instructions);
    }

    Compiler::StatementResult Compiler::CompileStatement(std::vector<Instruction>& instructions)
    {
        const ORchestraToken& token = Consume();

        switch (token.mTokenType)
        {
        case ORchestraTokenType::IDENTIFIER:
        {
            const std::string name = std::string(token.mStart, static_cast<unsigned long>(token.mLength));

            if (Peek().mTokenType == ORchestraTokenType::LEFT_BRACKET)
            {
                Consume();

                if (!CompileExpression(instructions))
                {
                    return StatementResult::ERROR;
                }

                if (Peek().mTokenType != ORchestraTokenType::RIGHT_BRACKET)
                {
                    std::string missingBracket = "]";
                    ThrowMissingExpectedToken(missingBracket);
                    return StatementResult::ERROR;
                }

                Consume();

                if (Peek().mTokenType == ORchestraTokenType::EQUAL)
                {
                    Consume();

                    if (!CompileExpression(instructions))
                    {
                        return StatementResult::ERROR;
                    }

                    const DataUnit id = GetOrCreateVariableID(name);
                    instructions.emplace_back(Instruction{ OpCode::SET_IDENTIFIER_WITH_INDEX, id });
                }
                else
                {
                    ThrowUnexpectedTokenError(Peek());
                    return StatementResult::ERROR;
                }
            }
            else if (Peek().mTokenType == ORchestraTokenType::EQUAL)
            {
                Consume();

                ORchestraTokenType tokenType = Peek().mTokenType;
                switch (tokenType)
                {
                    // Data Array
                case ORchestraTokenType::LEFT_BRACKET:
                {
                    // Check if this is a function array
                    if (mCurrentIndex + 1 < mTokens.size() &&
                        mTokens[mCurrentIndex + 1].mTokenType == ORchestraTokenType::IDENTIFIER)
                    {
                        std::string firstName(mTokens[mCurrentIndex + 1].mStart,
                                              static_cast<unsigned long>(mTokens[mCurrentIndex + 1].mLength));
                        if (mFunctions.find(firstName) != mFunctions.end())
                        {
                            if (!CompileFunctionArray(name))
                                return StatementResult::ERROR;
                            break;
                        }
                    }

                    DataUnit arrayLength;
                    const bool isLastRecursiveLevel = false;
                    if (CompileArray(instructions, arrayLength, MAX_DATASEQUENCE_LENGTH, isLastRecursiveLevel))
                    {
                        const DataUnit id = GetOrCreateVariableID(name);
                        instructions.emplace_back(Instruction{ OpCode::CONSTANT, arrayLength });
                        instructions.emplace_back(Instruction{ OpCode::SET_IDENTIFIER_ARRAY, id });
                    }
                    else
                    {
                        return StatementResult::ERROR;
                    }

                    break;
                }
                case ORchestraTokenType::NUMBER:
                case ORchestraTokenType::NOTE_IDENTIFIER:
                case ORchestraTokenType::IDENTIFIER:
                case ORchestraTokenType::LEFT_PAREN:
                case ORchestraTokenType::RANDOM:
                case ORchestraTokenType::DOLLAR:
                {
                    if (!CompileExpression(instructions))
                        return StatementResult::ERROR;

                    const DataUnit id = GetOrCreateVariableID(name);
                    instructions.emplace_back(Instruction{ OpCode::SET_IDENTIFIER_VALUE, id });
                    break;
                }
                case ORchestraTokenType::EUCLIDEAN:
                {
                    Consume();
                    if (!CompileFunctionCall(instructions, eucFunctionName))
                        return StatementResult::ERROR;

                    const DataUnit id = GetOrCreateVariableID(name);
                    instructions.emplace_back(Instruction{ OpCode::SET_IDENTIFIER_ARRAY, id });

                    break;
                }

                default:
                {
                    ThrowUnexpectedTokenError(Peek());
                    return StatementResult::ERROR;
                }
                }
            }
            // For Functions
            else if (Peek().mTokenType == ORchestraTokenType::LEFT_PAREN)
            {
                auto funcArrayIt = mFunctionArrayNames.find(name);
                if (funcArrayIt != mFunctionArrayNames.end())
                {
                    if (!CompileFunctionArrayCall(instructions, funcArrayIt->second))
                        return StatementResult::ERROR;
                }
                else if (!CompileFunctionCall(instructions, name))
                    return StatementResult::ERROR;
            }
            else
            {
                ThrowUnexpectedTokenError(Peek());
                return StatementResult::ERROR;
            }

            break;
        }

        case ORchestraTokenType::PRINT:
        case ORchestraTokenType::NOTE:
        case ORchestraTokenType::CC:
        {
            const std::string functionName = std::string(token.mStart, static_cast<unsigned long>(token.mLength));
            if (!CompileFunctionCall(instructions, functionName))
                return StatementResult::ERROR;

            break;
        }

        case ORchestraTokenType::FN:
        {
            if (!CompileFunctionDefinition(instructions))
                return StatementResult::ERROR;
            break;
        }

#if _TEST
        case ORchestraTokenType::TEST_KEYWORD:
        {
            if (Peek().mTokenType == ORchestraTokenType::IDENTIFIER ||
                Peek().mTokenType == ORchestraTokenType::NUMBER ||
                Peek().mTokenType == ORchestraTokenType::DOLLAR)
            {
                CompileExpression(instructions);
            }
            else
            {
                ThrowUnexpectedTokenError(Peek());
                return StatementResult::ERROR;
            }
            break;
        }
#endif

        case ORchestraTokenType::END_FN:
            return StatementResult::END_OF_FUNCTION;

        case ORchestraTokenType::END:
            return StatementResult::END_OF_INPUT;

        case ORchestraTokenType::EOL:
            break;

        default:
            ThrowUnexpectedTokenError(token);
            return StatementResult::ERROR;
        }

        return StatementResult::SUCCESS;
    }

    bool Compiler::Compile(std::vector<Instruction>& instructions)
    {
#if _DEBUG
        ScopedTimer timer("Compile");
#endif

        for (;;)
        {
            StatementResult result = CompileStatement(instructions);
            switch (result)
            {
            case StatementResult::SUCCESS:
                continue;
            case StatementResult::END_OF_INPUT:
                instructions.emplace_back(Instruction{ OpCode::END });
                return true;
            case StatementResult::END_OF_FUNCTION:
                ThrowUnexpectedTokenError(Previous());
                return false;
            case StatementResult::ERROR:
                return false;
            }
        }
    }

    bool Compiler::CompileFunctionArray(const std::string& name)
    {
        Consume(); // consume '['

        std::vector<std::vector<Instruction>> funcBlocks;

        for (;;)
        {
            if (Peek().mTokenType != ORchestraTokenType::IDENTIFIER)
            {
                ThrowUnexpectedTokenError(Peek());
                return false;
            }

            const ORchestraToken& funcToken = Consume();
            const std::string funcName = std::string(funcToken.mStart, static_cast<unsigned long>(funcToken.mLength));

            auto it = mFunctions.find(funcName);
            if (it == mFunctions.end())
            {
                ThrowUnknownFunctionOrVariable(funcName);
                return false;
            }

            const StoredFunction& func = it->second;
            if (func.mNumOfParams != 0)
            {
                std::string message = std::string("Function '") + funcName + std::string("' has parameters and cannot be used in a function array");
                mErrorReporting.LogError(funcToken.mLine, message);
                return false;
            }

            funcBlocks.push_back(func.mInstructions);

            if (Peek().mTokenType == ORchestraTokenType::COMMA)
            {
                Consume();
                continue;
            }
            break;
        }

        if (Peek().mTokenType != ORchestraTokenType::RIGHT_BRACKET)
        {
            std::string missingToken{ "]" };
            ThrowMissingExpectedToken(missingToken);
            return false;
        }
        Consume(); // consume ']'

        const DataUnit arrayId = static_cast<DataUnit>(mFunctionArrays.size());
        mFunctionArrays.push_back(funcBlocks);
        mFunctionArrayNames[name] = arrayId;

        return true;
    }

    bool Compiler::CompileFunctionArrayCall(std::vector<Instruction>& instructions, DataUnit arrayId)
    {
        Consume(); // consume '('

        if (!CompileExpression(instructions))
        {
            ThrowUnexpectedTokenError(Peek());
            return false;
        }

        if (Peek().mTokenType != ORchestraTokenType::RIGHT_PAREN)
        {
            std::string missingToken{ ")" };
            ThrowMissingExpectedToken(missingToken);
            return false;
        }
        Consume(); // consume ')'

        instructions.emplace_back(Instruction{ OpCode::EXEC_FUNC_ARRAY, arrayId });

        return true;
    }

    bool Compiler::CompileFunctionDefinition(std::vector<Instruction>& mainInstructions)
    {
        if (mInsideFunctionDefinition)
        {
            std::string message = "Cannot define a function inside another function";
            mErrorReporting.LogError(Peek().mLine, message);
            return false;
        }

        if (Peek().mTokenType != ORchestraTokenType::IDENTIFIER)
        {
            ThrowUnexpectedTokenError(Peek());
            return false;
        }

        const ORchestraToken& nameToken = Consume();
        const std::string name = std::string(nameToken.mStart, static_cast<unsigned long>(nameToken.mLength));
        const int fnLine = nameToken.mLine;

        //Check that it doesn't already have a function of this name
        if (mFunctions.find(name) != mFunctions.end())
        {
            std::string message = std::string("Function '") + name + std::string("' is already defined");
            mErrorReporting.LogError(fnLine, message);
            return false;
        }

        // Check that it doesn't clash with a variable
        if (mVariableIDMap.find(name) != mVariableIDMap.end())
        {
            std::string message = std::string("'") + name + std::string("' is already used as a variable name");
            mErrorReporting.LogError(fnLine, message);
            return false;
        }

        // Parse optional parameter list
        std::vector<DataUnit> paramIds;
        if (Peek().mTokenType == ORchestraTokenType::LEFT_PAREN)
        {
            Consume(); // consume '('

            if (Peek().mTokenType != ORchestraTokenType::RIGHT_PAREN)
            {
                for (;;)
                {
                    if (Peek().mTokenType != ORchestraTokenType::IDENTIFIER)
                    {
                        ThrowUnexpectedTokenError(Peek());
                        return false;
                    }

                    const ORchestraToken& paramToken = Consume();
                    const std::string paramName = std::string(paramToken.mStart, static_cast<unsigned long>(paramToken.mLength));
                    const DataUnit paramId = GetOrCreateVariableID(paramName);
                    paramIds.push_back(paramId);

                    // Emit initialization into main instruction stream
                    mainInstructions.emplace_back(Instruction{ OpCode::CONSTANT, 0 });
                    mainInstructions.emplace_back(Instruction{ OpCode::SET_IDENTIFIER_VALUE, paramId });

                    if (Peek().mTokenType == ORchestraTokenType::COMMA)
                    {
                        Consume(); // consume ','
                        continue;
                    }
                    break;
                }
            }

            if (Peek().mTokenType != ORchestraTokenType::RIGHT_PAREN)
            {
                std::string missingParen = ")";
                ThrowMissingExpectedToken(missingParen);
                return false;
            }
            Consume(); // consume ')'
        }

        if (Peek().mTokenType != ORchestraTokenType::EOL)
        {
            ThrowUnexpectedTokenError(Peek());
            return false;
        }
        Consume(); // consume EOL

        mInsideFunctionDefinition = true;
        std::vector<Instruction> bodyInstructions;

        for (;;)
        {
            StatementResult result = CompileStatement(bodyInstructions);
            switch (result)
            {
            case StatementResult::SUCCESS:
                continue;
            case StatementResult::END_OF_FUNCTION:
                mInsideFunctionDefinition = false;
                mFunctions[name] = StoredFunction(static_cast<int>(paramIds.size()), paramIds, bodyInstructions);
                return true;
            case StatementResult::END_OF_INPUT:
            {
                mInsideFunctionDefinition = false;
                std::string message = "Unexpected end, you're missing a end";
                mErrorReporting.LogError(fnLine, message);
                return false;
            }
            case StatementResult::ERROR:
                mInsideFunctionDefinition = false;
                return false;
            }
        }
    }

    void Compiler::ThrowUnknownFunctionOrVariable(const std::string& name)
    {
        std::string message = std::string("Unexpected function or variable '") + name + std::string("'");
        mErrorReporting.LogError(Peek().mLine, message);
    }

    void Compiler::ThrowUnexpectedTokenError(const ORchestraToken& tokenForError)
    {
        std::string token = std::string(tokenForError.mStart, static_cast<unsigned long>(tokenForError.mLength));
        std::string message = std::string("Unexpected Character '") + token + std::string("'");
        mErrorReporting.LogError(tokenForError.mLine, message);
    }

    void Compiler::ThrowUnexpectedStart(const ORchestraToken& tokenForError)
    {
        std::string token = std::string(tokenForError.mStart, static_cast<unsigned long>(tokenForError.mLength));
        std::string message = std::string("Unexpected Character '") 
                                          + token 
                                          + std::string("'. Start a line with an variable identifier, or note() or cc()");
        mErrorReporting.LogError(tokenForError.mLine, message);
    }

    void Compiler::ThrowMissingExpectedToken(const std::string& missingToken)
    {
        std::string message = std::string("Missing a ") + missingToken;
        mErrorReporting.LogError(Peek().mLine, message);
    }

    void Compiler::ThrowMissingParamCount(int expected, int received)
    {
        std::string message = std::string("Expected '") +
            std::to_string(expected) +
            std::string("' function parameters, but received ") +
            std::to_string(received);

        mErrorReporting.LogError(Peek().mLine, message);
    }

    void Compiler::ThrowUnexpectedEnd(const std::string& missingToken)
    {
        std::string message = "Unexpected end, you're missing a " + missingToken;
        mErrorReporting.LogError(Peek().mLine, message);
    }

    DataUnit Compiler::GetOrCreateVariableID(const std::string& varName)
    {
        if (mVariableIDMap.find(varName) != mVariableIDMap.end())
        {
            return mVariableIDMap[varName];
        }
        
        mVariableIDMap[varName] = mVariableIdCounter++;

        if (mVariableIdCounter > 255)
        {
            mErrorReporting.LogWarning("Only 255 unique variables are supported");
        }

        return mVariableIDMap[varName];
    }

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace ORchestra
