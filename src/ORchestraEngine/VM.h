#pragma once

#include <array>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <cassert>

#include "ErrorReporting.h"
#include "Scanner.h"
#include "Compiler.h"
#include "Types.h"
#include "MidiScheduler.h"
#include "DataSequence.h"
#include "StepData.h"
#include "CustomStack.h"

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
    bool Prepare(const char* data);
    bool Tick(std::vector<SequenceStep>& stepQueue, const int globalCount);
    void Reset();
    StepData GetTopStackValue() { return mStack.Top(); }
    const std::vector<LogEntry>& GetErrors();

private:
    bool ProcessOpCodes(std::vector<Instruction>& setupInstructions);
    bool ProcessInstruction(const Instruction& instruction, const int stepCount);
    
    std::unique_ptr<ErrorReporting> mErrorReporting;
    std::unordered_map<std::string, DataSequence> mVariables;
    std::vector<Instruction> mRuntimeInstructions;
    inline uChar RandomValue(const uChar low, const uChar high);

    Stack<StepData> mStack;
    
    template<typename Operation>
    void PopDoOperationAndPush(Operation op)
    {
        static_assert(std::is_invocable_v<Operation, const int, const int>,
                             "Operation must be callable with two int parameters");
        
        const StepData& b = mStack.Pop();
        const StepData& a = mStack.Pop();
        const StepData result = a.ApplySequenceWithOperation(b, op);
        mStack.Push(StepData{result});
    }
};
