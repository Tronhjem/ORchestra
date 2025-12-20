#include <algorithm>

#include "VM.h"

#if _DEBUG
#include "ScopedTimer.h"
#endif

#include "EuclideanGenerator.h"
#include "Defines.h"

namespace ORchestra
{

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

    VM::VM() : mErrorReporting(),
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
                success = ProcessOpCodes(mRuntimeInstructions);
            }
        }

        return success;
    }

    void VM::Reset()
    {
        mErrorReporting.Clear();
        mScanner.Reset();
        mCompiler.Reset();
        mVariables.clear();
        mRuntimeInstructions.clear();
    }

    const std::vector<LogEntry>& VM::GetErrors()
    {
        return mErrorReporting.GetErrors();
    }

    bool VM::ProcessOpCodes(std::vector<Instruction>& instructions)
    {
#if _DEBUG
        ScopedTimer timer("VM Process OpCodes");
#endif

        Stack<StepData> stack;

        unsigned long currentIndex = 0;
        auto consume = [&]() -> Instruction&
            {
                return instructions[currentIndex++];
            };

        for (;;)
        {
            const Instruction& instruction = consume();

            switch (instruction.opCode)
            {
            case (OpCode::SET_IDENTIFIER_VALUE):
            {
                StepData value = stack.Pop();
                std::vector<StepData> vectorData{ value };
                mVariables.emplace_back(DataSequence{ vectorData });

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
                mVariables.emplace_back(DataSequence{ vectorData });

                break;
            }

            case (OpCode::SET_IDENTIFIER_WITH_INDEX):
            {
                const StepData value = stack.Pop();
                const int index = stack.Pop().GetValue(0);
                
                if (instruction.mId < mVariables.size())
                {
                    mVariables[instruction.mId].SetValue(index, value);
                }
                else
                {
                    const std::string error = std::string("VM: Variable not defined");
                    mErrorReporting.LogError(error);
                    return false;
                }

                break;
            }

            case (OpCode::NOTE):
            case (OpCode::CC):
                break;

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

            switch (instruction.opCode)
            {
            case (OpCode::NOTE):
            {
                const StepData channel = stack.Pop();
                const StepData vel = stack.Pop();
                const StepData note = stack.Pop();
                const StepData shouldTrigger = stack.Pop();

                stepQueue.emplace_back(SequenceStep{ MidiType::NoteOn, shouldTrigger, note, vel, channel, DEFAULT_NOTE_DURATION });

                break;
            }

            case (OpCode::CC):
            {
                const StepData channel = stack.Pop();
                const StepData ccValue = stack.Pop();
                const StepData ccNumber = stack.Pop();
                const StepData shouldTrigger = stack.Pop();

                stepQueue.emplace_back(SequenceStep{ MidiType::CC, shouldTrigger, ccNumber, ccValue, channel, DEFAULT_NOTE_DURATION });

                break;
            }

            case (OpCode::END):
#if _TEST
                SafeSetTopStackValue(stack);
#endif
                return true;

            default:
                if (!ProcessInstruction(instruction, globalCount, stack))
                {
#if _TEST
                    SafeSetTopStackValue(stack);
#endif
                    return false;
                }
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
        calculatedRange = std::clamp(calculatedRange, 1, MAX_UCHAR_VALUE);

        const int value = rand() % calculatedRange;
        const int result = value + static_cast<int>(low);
        return static_cast<DataUnit>(std::clamp(result, 0, MAX_UCHAR_VALUE));
    }

    bool VM::ProcessInstruction(const Instruction& instruction, const int stepCount, Stack<StepData>& stack)
    {
        switch (instruction.opCode)
        {
        case (OpCode::CONSTANT):
        {
            stack.Push(instruction.mDataValue);

            break;
        }

        case (OpCode::SET_IDENTIFIER_VALUE):
        {
            const StepData value = stack.Pop();
            mVariables[instruction.mId].SetValue(0, value);

            break;
        }

        case (OpCode::SET_IDENTIFIER_ARRAY):
        {
            const int arrayLength = std::clamp(static_cast<int>(stack.Pop().GetValue(0)), 0, MAX_DATASEQUENCE_LENGTH);

            for (int i = arrayLength - 1; i >= 0; --i)
            {
                mVariables[instruction.mId].SetValue(i, stack.Pop());
            }

            break;
        }

        case (OpCode::SET_IDENTIFIER_WITH_INDEX):
        {
            const StepData value = stack.Pop();
            const int index = stack.Pop().GetValue(0);
            
            if (instruction.mId < mVariables.size())
            {
                mVariables[instruction.mId].SetValue(index, value);
            }
            else
            {
                const std::string error = std::string("VM: Variable not defined");
                mErrorReporting.LogError(error);
                return false;
            }

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
            const int length = std::clamp(static_cast<int>(stack.Pop().GetValue(0)), 0, MAX_DATASEQUENCE_LENGTH);
            const int hits = std::clamp(static_cast<int>(stack.Pop().GetValue(0)), 0, length);
            StepData data[32];

            GenerateEuclideanSequence(data, hits, length);

            for (int i = 0; i < length; ++i)
            {
                stack.Push(data[i]);
            }

            const int clampedLength = std::clamp(length, 0, MAX_UCHAR_VALUE);
            stack.Push(StepData{ clampedLength });

            break;
        }

        case (OpCode::GET_IDENTIFIER_VALUE):
        {
            if (instruction.mId < mVariables.size())
            {
                const StepData value = mVariables[instruction.mId].GetValue(stepCount);
                stack.Push(value);
            }
            else
            {
                const std::string error = std::string("VM: Variable not defined");
                mErrorReporting.LogError(error);
                return false;
            }

            break;
        }

        case (OpCode::GET_IDENTIFIER_WITH_INDEX):
        {
            if (instruction.mId < mVariables.size())
            {
                const int index = stack.Pop().GetValue(0);
                // GetValue is done with modulo inside, so no need to worry about out of bounds value
                const StepData value = mVariables[instruction.mId].GetValue(index);
                stack.Push(value);
            }
            else
            {
                const std::string error = std::string("VM: Variable not defined");
                mErrorReporting.LogError(error);
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

#if _DEBUG
        case (OpCode::PRINT):
        {
            const DataUnit value = stack.Pop().GetValue(0);
            const std::string message = "PRINT: " + std::to_string(static_cast<int>(value));
            std::cout << message << std::endl;
            mErrorReporting.LogMessage(message);

            break;
        }
#endif
        default:
            const std::string err{ "Unexpected Operation code" };
            mErrorReporting.LogError(err);
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
