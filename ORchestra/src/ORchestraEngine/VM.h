#pragma once

#include <array>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <cassert>

#include "Defines.h"
#include "ErrorReporting.h"
#include "Scanner.h"
#include "Compiler.h"
#include "DataSequence.h"
#include "StepData.h"
#include "CustomStack.h"
#include "SequenceStep.h"

namespace ORchestra
{

    class Instruction;

    /// This is the virtual machine that takes care of initializing the Scanner and Compiler
    /// to first read the file, compile it into OpCodes and then later run the OpCodes on each Tick
    /// to populate the SequenceSteps for the ORChestra Engine and MidiScheduler.
    /// It's a simple stack based virtual machine that goes through all the OpCodes from the compiler
    /// and executes them.
    class VM
    {
    public:
        VM();
        bool Prepare(const std::string& data);
        bool Tick(std::vector<SequenceStep>& stepQueue, const int globalCount);
        void Reset();
        const std::vector<LogEntry>& GetErrors();

#if _TEST
        StepData GetTopStackValue() { return mTopStackValue; }
#endif

    private:
#if _TEST
        StepData mTopStackValue;
        
        inline void SafeSetTopStackValue(Stack<StepData>& stack)
        {
            if (stack.mStackPointer > 0)
                mTopStackValue = stack.Top();
        }
#endif

        bool ProcessOpCodes(std::vector<Instruction>& setupInstructions);
        inline bool ProcessInstruction(const Instruction& instruction, const int stepCount, Stack<StepData>& mStack);

        ErrorReporting mErrorReporting;
        Scanner mScanner;
        Compiler mCompiler;

        std::unordered_map<std::string, DataSequence> mVariables;
        std::vector<Instruction> mRuntimeInstructions;
        inline DataUnit RandomValue(const DataUnit low, const DataUnit high);

        template <typename Operation>
        void PopDoOperationAndPush(Operation op, Stack<StepData>& stack)
        {
            static_assert(std::is_invocable_v<Operation, const int, const int>,
                "Operation must be callable with two int parameters");

            const StepData& b = stack.Pop();
            const StepData& a = stack.Pop();
            const StepData result = a.ApplySequenceWithOperation(b, op);
            stack.Push(StepData{ result });
        }
    };

} // namespace ORchestra
