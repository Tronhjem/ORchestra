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

#include "ErrorReporting.h"
#include <algorithm>

#if _DEBUG
#include "ScopedTimer.h"
#endif

#include "VM.h"
#include "StepData.h"
#include "EuclideanGenerator.h"
#include "Defines.h"

namespace ORchestra
{

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif
    unsigned int VM::mRanSeed = static_cast<unsigned int>(rand());

    VM::VM(ErrorReporting& errorReporting) : mErrorReporting(errorReporting),
        mScanner(mErrorReporting),
        mCompiler(mScanner.GetTokens(), mErrorReporting)
    {
        mRuntimeInstructions.reserve(64);
        mVariables.reserve(16);
    }

    bool VM::Prepare(const std::string& data)
    {
        bool success = false;
        success = mScanner.ScanFile(data);
        if (success)
        {
            success = mCompiler.Compile(mRuntimeInstructions);
            if (success)
            {
                mVariableNames = mCompiler.GetVariableNames();
                success = ProcessOpCodes(mRuntimeInstructions);
                mFunctionArrays = mCompiler.GetFunctionArrays();
            }
        }

        return success;
    }

    void VM::Reset()
    {
        mScanner.Reset();
        mCompiler.Reset();
        mVariables.clear();
        mVariableNames.clear();
        mRuntimeInstructions.clear();
        mFunctionArrays.clear();
    }

    bool VM::ProcessOpCodes(std::vector<Instruction>& instructions)
    {
#if _DEBUG
        ScopedTimer timer("VM Process OpCodes");
#endif

        Stack<StepData> stack;

        unsigned long currentIndex = 0;
        auto consume = [&]() -> Instruction& {
            return instructions[currentIndex++];
        };

        for (;;)
        {
            const Instruction& instruction = consume();

            switch (instruction.GetOpCode())
            {
            case (OpCode::SET_IDENTIFIER_VALUE):
            {
                StepData value = stack.Pop();
                std::vector<StepData> vectorData{ value };
                
                // If the Id doesn't exist yet create a new variable
                // else fetch the variable based on the id
                const size_t operandId = static_cast<size_t>(instruction.GetOperand());
                if(operandId >= mVariables.size())
                    mVariables.emplace_back(DataSequence{ vectorData });
                else
                    mVariables[operandId] = DataSequence{ vectorData };

                break;
            }

            case (OpCode::SET_IDENTIFIER_ARRAY):
            {
                const int arrayLength = std::clamp(static_cast<int>(stack.Pop().GetValue(0)), 0, MAX_DATASEQUENCE_LENGTH);
                StepData data[MAX_DATASEQUENCE_LENGTH];
                for (int i = arrayLength - 1; i >= 0; --i)
                {
                    data[i] = stack.Pop();
                }

                std::vector<StepData> vectorData{ data, data + arrayLength };

                // If the Id doesn't exist yet create a new variable
                // else fetch the variable based on the id
                const size_t operandId = static_cast<size_t>(instruction.GetOperand());
                if(operandId >= mVariables.size())
                    mVariables.emplace_back(DataSequence{ vectorData });
                else
                    mVariables[operandId] = DataSequence{ vectorData };

                break;
            }

            case (OpCode::SET_IDENTIFIER_WITH_INDEX):
            {
                const StepData value = stack.Pop();
                const int index = stack.Pop().GetValue(0);
                const size_t operandId = static_cast<size_t>(instruction.GetOperand());
                if (operandId < mVariables.size())
                {
                    mVariables[operandId].SetValue(index, value);
                }
                else
                {
                    mErrorReporting.LogError("VM: Variable '" + VariableName(instruction.GetOperand()) + "' is not defined");
                    return false;
                }

                break;
            }

            case (OpCode::NOTE):
            case (OpCode::CC):
            {
                stack.Pop();
                stack.Pop();
                stack.Pop();
                stack.Pop();
                break;
            }

            case (OpCode::SET_BPM):
            case (OpCode::SET_BPM_DIVISION):
            case (OpCode::SET_TRANSPOSE):
            case (OpCode::PRINT):
            {
                stack.Pop();
                break;
            }

            case (OpCode::UPDATE_IDENTIFIER_VALUE):
            {
                StepData value = stack.Pop();
                std::vector<StepData> vectorData{ value };
                const size_t operandId = static_cast<size_t>(instruction.GetOperand());
                if (operandId >= mVariables.size())
                    mVariables.emplace_back(DataSequence{ vectorData });
                else
                    mVariables[operandId].SetValue(0, value);
                break;
            }

            case (OpCode::EXEC_FUNC_ARRAY):
            {
                stack.Pop();
                break;
            }

            case (OpCode::END):
            {
#if _TEST
                SafeSetTopStackValue(stack);
#endif
                return true;
            }

            default:
            {
                if (!ProcessInstruction(instruction, 0, stack))
                {
#if _TEST
                    SafeSetTopStackValue(stack);
#endif
                    return false;
                }
            }
            }
        }
    }

    bool VM::ExecuteTickInstruction(const Instruction& instruction, const int globalCount,
                                    Stack<StepData>& stack, std::vector<SequenceStep>& stepQueue)
    {
        switch (instruction.GetOpCode())
        {
        case (OpCode::NOTE):
        {
            const StepData channel       = stack.Pop();
            const StepData duration      = stack.Pop();
            const StepData vel           = stack.Pop();
            const StepData note          = stack.Pop();
            const StepData shouldTrigger = stack.Pop();
            stepQueue.emplace_back(SequenceStep{ SequenceStepType::NoteOn, shouldTrigger, note, vel, channel,
                static_cast<int>(duration.GetValue(0)) });
            break;
        }

        case (OpCode::CC):
        {
            const StepData channel = stack.Pop();
            const StepData ccValue = stack.Pop();
            const StepData ccNumber = stack.Pop();
            const StepData shouldTrigger = stack.Pop();
            stepQueue.emplace_back(SequenceStep{ SequenceStepType::CC, shouldTrigger, ccNumber, ccValue, channel, DEFAULT_NOTE_DURATION });
            break;
        }

        case (OpCode::SET_BPM):
        {
            const StepData bpmValue = stack.Pop();
            stepQueue.emplace_back(SequenceStep{ SequenceStepType::BPM, bpmValue, bpmValue, bpmValue, bpmValue, DEFAULT_NOTE_DURATION });
            break;
        }

        case (OpCode::SET_BPM_DIVISION):
        {
            const StepData bpmDivValue = stack.Pop();
            stepQueue.emplace_back(SequenceStep{ SequenceStepType::BPM_DIVISION, bpmDivValue,
                            bpmDivValue, bpmDivValue,
                            bpmDivValue, DEFAULT_NOTE_DURATION });
            break;
        }

        case (OpCode::SET_TRANSPOSE):
        {
            const StepData transposeValue = stack.Pop();
            stepQueue.emplace_back(SequenceStep{ SequenceStepType::TRANSPOSE, transposeValue, transposeValue, transposeValue, transposeValue, DEFAULT_NOTE_DURATION });
            break;
        }

        case (OpCode::PRINT):
        {
            const StepData printValue = stack.Pop();
            stepQueue.emplace_back(SequenceStep{ SequenceStepType::PRINT, printValue, printValue,
                                   printValue, printValue, DEFAULT_NOTE_DURATION });
            break;
        }

        case (OpCode::EXEC_FUNC_ARRAY):
        {
            const size_t arrayId = static_cast<size_t>(instruction.GetOperand());
            const auto& funcArray = mFunctionArrays[arrayId];
            const size_t index = static_cast<size_t>(stack.Pop().GetValue(0)) % funcArray.size();
            const auto& block = funcArray[index];

            for (const Instruction& blockInstr : block)
            {
                if (!ExecuteTickInstruction(blockInstr, globalCount, stack, stepQueue))
                    return false;
            }
            break;
        }

        default:
            return ProcessInstruction(instruction, globalCount, stack);
        }

        return true;
    }

    bool VM::Tick(std::vector<SequenceStep>& stepQueue, const int globalCount)
    {
        Stack<StepData> stack;

        unsigned long currentIndex = 0;
        auto consume = [&]() -> Instruction&
            {
                return mRuntimeInstructions[currentIndex++];
            };

        for (;;)
        {
            const Instruction& instruction = consume();

            if (instruction.GetOpCode() == OpCode::END)
            {
#if _TEST
                SafeSetTopStackValue(stack);
#endif
                return true;
            }

            if (!ExecuteTickInstruction(instruction, globalCount, stack, stepQueue))
            {
#if _TEST
                SafeSetTopStackValue(stack);
#endif
                return false;
            }
        }
    }

    DataUnit VM::RandomValue(const DataUnit low, const DataUnit high)
    {
        if (high <= low)
        {
            return low;
        }

        int calculatedRange = static_cast<int>(high) - static_cast<int>(low) + 1;
        calculatedRange = std::clamp(calculatedRange, 1, DATA_UNIT_MAX_VALUE);

        const int value = fast_rand() % calculatedRange;
        const int result = value + static_cast<int>(low);
        return static_cast<DataUnit>(std::clamp(result, DATA_UNIT_MIN_VALUE, DATA_UNIT_MAX_VALUE));
    }

    bool VM::ProcessInstruction(const Instruction& instruction, const int stepCount, Stack<StepData>& stack)
    {
        switch (instruction.GetOpCode())
        {
        case (OpCode::CONSTANT):
        {
            StepData value {instruction.GetOperand()};
            stack.Push(value);

            break;
        }

        case (OpCode::SET_IDENTIFIER_VALUE):
        {
            const StepData value = stack.Pop();
            mVariables[static_cast<size_t>(instruction.GetOperand())].SetValue(0, value);

            break;
        }

        case (OpCode::SET_IDENTIFIER_ARRAY):
        {
            const int arrayLength = std::clamp(static_cast<int>(stack.Pop().GetValue(0)), 0, MAX_DATASEQUENCE_LENGTH);
            StepData data[MAX_DATASEQUENCE_LENGTH];
            for (int i = arrayLength - 1; i >= 0; --i)
            {
                data[i] = stack.Pop();
            }

            std::vector<StepData> vectorData{ data, data + arrayLength };
            
            // If the Id doesn't exist yet create a new variable
            // else fetch the variable based on the id
            const size_t operandId = static_cast<size_t>(instruction.GetOperand());
            if(operandId >= mVariables.size())
                mVariables.emplace_back(DataSequence{ vectorData });
            else
                mVariables[operandId] = DataSequence{ vectorData };

            break;
        }

        case (OpCode::SET_IDENTIFIER_WITH_INDEX):
        {
            const StepData value = stack.Pop();
            const int index = stack.Pop().GetValue(0);
            const size_t operandId = static_cast<size_t>(instruction.GetOperand());

            if (operandId < mVariables.size())
            {
                mVariables[operandId].SetValue(index, value);
            }
            else
            {
                mErrorReporting.LogError("VM: Variable '" + VariableName(instruction.GetOperand()) + "' is not defined");
                return false;
            }

            break;
        }

        case (OpCode::UPDATE_IDENTIFIER_VALUE):
        {
            const StepData value = stack.Pop();
            mVariables[static_cast<size_t>(instruction.GetOperand())].SetValue(0, value);
            break;
        }

        case (OpCode::SET_SUBSTEP_ARRAY):
        {
            const int subStepArrayLength = std::clamp(static_cast<int>(stack.Pop().GetValue(0)), 0, MAX_SUB_DIVISION_LENGTH);
            DataUnit data[MAX_SUB_DIVISION_LENGTH];

            for (int i = subStepArrayLength - 1; i >= 0; --i)
            {
                data[i] = stack.Pop().GetValue(0);
            }

            StepData newStepData{ data, subStepArrayLength };
            stack.Push(newStepData);

            break;
        }

        case (OpCode::GENERATE_EUCLID_SEQUENCE):
        {
            const int shift  = static_cast<int>(stack.Pop().GetValue(0));
            const int length = 
                std::clamp(static_cast<int>(stack.Pop().GetValue(0)), 0, MAX_DATASEQUENCE_LENGTH);

            const int hits   = std::clamp(static_cast<int>(stack.Pop().GetValue(0)), 0, length);

            StepData data[MAX_DATASEQUENCE_LENGTH];

            GenerateEuclideanSequence(data, hits, length);

            for (int i = 0; i < length; ++i)
            {
                const int shiftedIndex = ((i + shift) % length + length) % length;
                stack.Push(data[shiftedIndex]);
            }

            const int clampedLength = std::clamp(length, DATA_UNIT_MIN_VALUE, DATA_UNIT_MAX_VALUE);
            stack.Push(StepData{ clampedLength });

            break;
        }

        case (OpCode::GET_IDENTIFIER_VALUE):
        {
            const size_t operandId = static_cast<size_t>(instruction.GetOperand());
            if (operandId < mVariables.size())
            {
                const StepData value = mVariables[operandId].GetValue(stepCount);
                stack.Push(value);
            }
            else
            {
                mErrorReporting.LogError("VM: Variable '" + VariableName(instruction.GetOperand()) + "' is not defined");
                return false;
            }

            break;
        }

        case (OpCode::GET_IDENTIFIER_WITH_INDEX):
        {
            const size_t operandIdIdx = static_cast<size_t>(instruction.GetOperand());
            if (operandIdIdx < mVariables.size())
            {
                const int index = stack.Pop().GetValue(0);
                // GetValue is done with modulo inside, so no need to worry about out of bounds value
                const StepData value = mVariables[operandIdIdx].GetValue(index);
                stack.Push(value);
            }
            else
            {
                mErrorReporting.LogError("VM: Variable '" + VariableName(instruction.GetOperand()) + "' is not defined");
                return false;
            }

            break;
        }

        case (OpCode::AND):
        {
            PopDoOperationAndPush(BinaryAND, stack);
            break;
        }

        case (OpCode::OR):
        {
            PopDoOperationAndPush(BinaryOR, stack);
            break;
        }

        case (OpCode::XOR):
        {
            PopDoOperationAndPush(BinaryXOR, stack);
            break;
        }

        case (OpCode::ADD):
        {
            PopDoOperationAndPush(Add, stack);
            break;
        }

        case (OpCode::SUBTRACT):
        {
            PopDoOperationAndPush(Subtract, stack);
            break;
        }

        case (OpCode::MULTIPLY):
        {
            PopDoOperationAndPush(Multiply, stack);
            break;
        }

        case (OpCode::DIVIDE):
        {
            PopDoOperationAndPush(Divide, stack);
            break;
        }

        case (OpCode::MODULO):
        {
            PopDoOperationAndPush(Modulo, stack);
            break;
        }

        case (OpCode::LESS):
        {
            PopDoOperationAndPush(Lesser, stack);
            break;
        }

        case (OpCode::LESS_EQUAL):
        {
            PopDoOperationAndPush(LesserEqual, stack);
            break;
        }

        case (OpCode::GREATER):
        {
            PopDoOperationAndPush(Greater, stack);
            break;
        }

        case (OpCode::GREATER_EQUAL):
        {
            PopDoOperationAndPush(GreaterEqual, stack);
            break;
        }

        case (OpCode::EQUAL):
        {
            PopDoOperationAndPush(Equal, stack);
            break;
        }

        case (OpCode::NOT_EQUAL):
        {
            PopDoOperationAndPush(NotEqual, stack);
            break;
        }

        case (OpCode::NEGATE):
        {
            const StepData val = stack.Pop();
            const StepData negated = val.ApplySequenceWithOperation(val, [](const int a, const int) { return -a; });
            stack.Push(negated);
            break;
        }

        case (OpCode::GET_RANDOM_IN_RANGE):
        {
            const DataUnit high = stack.Pop().GetValue(0);
            const DataUnit low = stack.Pop().GetValue(0);
            const int value = (int)RandomValue(low, high);
            stack.Push(StepData{ value });

            break;
        }

        case (OpCode::GET_GLOBAL_COUNT):
        {
            stack.Push(StepData{ stepCount });

            break;
        }

        default:
            mErrorReporting.LogError("VM: Unexpected opcode " + std::to_string(static_cast<int>(instruction.GetOpCode())));
#if _TEST
            SafeSetTopStackValue(stack);
#endif
            return false;
        }

#if _TEST
        SafeSetTopStackValue(stack);
#endif
        return true;
    }

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace ORchestra
