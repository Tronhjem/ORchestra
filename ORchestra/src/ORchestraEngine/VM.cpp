#include <algorithm>

#include "VM.h"

#include "ScopedTimer.h"
#include "EuclideanGenerator.h"
#include "Defines.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

VM::VM()
{
    mErrorReporting = std::make_unique<ErrorReporting>();
}

bool VM::Prepare(const char *data)
{
    Scanner scanner{*mErrorReporting};
    Compiler compiler{scanner.GetTokens(), *mErrorReporting};

    bool success = false;
    success = scanner.ScanFile(data);
    if (success)
    {
        success = compiler.Compile(mRuntimeInstructions);
        if (success)
        {
            success = ProcessOpCodes(mRuntimeInstructions);
        }
    }

    mStack.Clear();
    return success;
}

void VM::Reset()
{
    mVariables.clear();
    mRuntimeInstructions.clear();
    mErrorReporting->Clear();
}

const std::vector<LogEntry> &VM::GetErrors()
{
    return mErrorReporting->GetErrors();
}

bool VM::ProcessOpCodes(std::vector<Instruction> &instructions)
{
    ScopedTimer timer("VM Process OpCodes");

    unsigned long currentIndex = 0;
    auto consume = [&]() -> Instruction &
    {
        return instructions[currentIndex++];
    };

    for (;;)
    {
        const Instruction &instruction = consume();

        switch (instruction.opCode)
        {
        case (OpCode::SET_IDENTIFIER_VALUE):
        {
            StepData value = mStack.Pop();
            std::vector<StepData> vectorData{value};
            mVariables[instruction.mNameValue] = DataSequence{vectorData};

            break;
        }

        case (OpCode::SET_IDENTIFIER_ARRAY):
        {
            const int arrayLength = std::clamp(static_cast<int>(mStack.Pop().GetValue(0)), 0, 32);
            StepData data[32];
            for (int i = arrayLength - 1; i >= 0; --i)
            {
                data[i] = mStack.Pop();
            }

            std::vector<StepData> vectorData{data, data + arrayLength};
            mVariables[instruction.mNameValue] = DataSequence{vectorData};

            break;
        }

        case (OpCode::NOTE):
        case (OpCode::CC):
            break;

        case (OpCode::END):
            return true;

        default:
        {
            if (!ProcessInstruction(instruction, 0))
                return false;
        }
        }
    }
}

bool VM::Tick(std::vector<SequenceStep> &stepQueue, const int globalCount)
{
    unsigned long currentIndex = 0;
    auto consume = [&]() -> Instruction &
    {
        return mRuntimeInstructions[currentIndex++];
    };

    for (;;)
    {
        const Instruction &instruction = consume();

        switch (instruction.opCode)
        {
        case (OpCode::NOTE):
        {
            const StepData channel = mStack.Pop();
            const StepData vel = mStack.Pop();
            const StepData note = mStack.Pop();
            const StepData shouldTrigger = mStack.Pop();

            stepQueue.emplace_back(SequenceStep{MidiType::NoteOn, shouldTrigger, note, vel, channel, DEFAULT_NOTE_DURATION});

            break;
        }

        case (OpCode::CC):
        {
            const StepData channel = mStack.Pop();
            const StepData ccValue = mStack.Pop();
            const StepData ccNumber = mStack.Pop();
            const StepData shouldTrigger = mStack.Pop();

            stepQueue.emplace_back(SequenceStep{MidiType::CC, shouldTrigger, ccNumber, ccValue, channel, DEFAULT_NOTE_DURATION});

            break;
        }

        case (OpCode::END):
            return true;

        default:
            if (!ProcessInstruction(instruction, globalCount))
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
    calculatedRange = std::clamp(calculatedRange, 1, MAX_UCHAR_VALUE);

    const int value = rand() % calculatedRange;
    const int result = value + static_cast<int>(low);
    return static_cast<DataUnit>(std::clamp(result, 0, MAX_UCHAR_VALUE));
}

bool VM::ProcessInstruction(const Instruction &instruction, const int stepCount)
{
    switch (instruction.opCode)
    {
    case (OpCode::CONSTANT):
    {
        mStack.Push(instruction.mDataValue);

        break;
    }

    case (OpCode::SET_IDENTIFIER_VALUE):
    {
        const StepData value = mStack.Pop();
        mVariables[instruction.mNameValue].SetValue(0, value);

        break;
    }

    case (OpCode::SET_IDENTIFIER_ARRAY):
    {
        const int arrayLength = std::clamp(static_cast<int>(mStack.Pop().GetValue(0)), 0, MAX_DATASEQUENCE_LENGTH);

        for (int i = arrayLength - 1; i >= 0; --i)
        {
            mVariables[instruction.mNameValue].SetValue(i, mStack.Pop());
        }

        break;
    }

    case (OpCode::SET_SUBSTEP_ARRAY):
    {
        const int subStepArrayLength = std::clamp(static_cast<int>(mStack.Pop().GetValue(0)), 0, MAX_SUB_DIVISION_LENGTH);
        DataUnit data[MAX_SUB_DIVISION_LENGTH];

        for (int i = subStepArrayLength - 1; i >= 0; --i)
        {
            data[i] = mStack.Pop().GetValue(0);
        }

        StepData newStepData{data, subStepArrayLength};
        mStack.Push(newStepData);

        break;
    }

    case (OpCode::GENERATE_EUCLID_SEQUENCE):
    {
        const int length = std::clamp(static_cast<int>(mStack.Pop().GetValue(0)), 0, MAX_DATASEQUENCE_LENGTH);
        const int hits = std::clamp(static_cast<int>(mStack.Pop().GetValue(0)), 0, length);
        StepData data[32];

        GenerateEuclideanSequence(data, hits, length);

        for (int i = 0; i < length; ++i)
        {
            mStack.Push(data[i]);
        }

        const int clampedLength = std::clamp(length, 0, MAX_UCHAR_VALUE);
        mStack.Push(StepData{clampedLength});

        break;
    }

    case (OpCode::GET_IDENTIFIER_VALUE):
    {
        if (mVariables.find(instruction.mNameValue) != mVariables.end())
        {
            const StepData value = mVariables[instruction.mNameValue].GetValue(stepCount);
            mStack.Push(value);
        }
        else
        {
            const std::string error = std::string("VM: Variable not defined");
            mErrorReporting->LogError(error);
            return false;
        }

        break;
    }

    case (OpCode::GET_IDENTIFIER_WITH_INDEX):
    {
        if (mVariables.find(instruction.mNameValue) != mVariables.end())
        {
            const int index = mStack.Pop().GetValue(0);
            // GetGalue is done with modulo inside, so no need to worry about out of bounds value
            const StepData value = mVariables[instruction.mNameValue].GetValue(index);
            mStack.Push(value);
        }
        else
        {
            const std::string error = std::string("VM: Variable not defined");
            mErrorReporting->LogError(error);
            return false;
        }

        break;
    }

    case (OpCode::AND):
    {
        PopDoOperationAndPush(BinaryAND);
        break;
    }

    case (OpCode::OR):
    {
        PopDoOperationAndPush(BinaryOR);
        break;
    }

    case (OpCode::XOR):
    {
        PopDoOperationAndPush(BinaryXOR);
        break;
    }

    case (OpCode::ADD):
    {
        PopDoOperationAndPush(Add);
        break;
    }

    case (OpCode::SUBTRACT):
    {
        PopDoOperationAndPush(Subtract);
        break;
    }

    case (OpCode::MULTIPLY):
    {
        PopDoOperationAndPush(Multiply);
        break;
    }

    case (OpCode::DIVIDE):
    {
        PopDoOperationAndPush(Divide);
        break;
    }

    case (OpCode::LESS):
    {
        PopDoOperationAndPush(Lesser);
        break;
    }

    case (OpCode::LESS_EQUAL):
    {
        PopDoOperationAndPush(LesserEqual);
        break;
    }

    case (OpCode::GREATER):
    {
        PopDoOperationAndPush(Greater);
        break;
    }

    case (OpCode::GREATER_EQUAL):
    {
        PopDoOperationAndPush(GreaterEqual);
        break;
    }

    case (OpCode::EQUAL):
    {
        PopDoOperationAndPush(Equal);
        break;
    }

    case (OpCode::NOT_EQUAL):
    {
        PopDoOperationAndPush(NotEqual);
        break;
    }

    case (OpCode::GET_RANDOM_IN_RANGE):
    {
        const DataUnit high = mStack.Pop().GetValue(0);
        const DataUnit low = mStack.Pop().GetValue(0);
        const int value = (int)RandomValue(low, high);
        mStack.Push(StepData{value});

        break;
    }

#if _DEBUG
    case (OpCode::PRINT):
    {
        const DataUnit value = mStack.Pop().GetValue(0);
        const std::string message = "PRINT: " + std::to_string(static_cast<int>(value));
        std::cout << message << std::endl;
        mErrorReporting->LogMessage(message);

        break;
    }
#endif
    default:
        const std::string err{"Unexpected Operation code"};
        mErrorReporting->LogError(err);

        return false;
    }

    return true;
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
