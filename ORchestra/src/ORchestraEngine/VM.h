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

#pragma once

#include <vector>
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
    struct TransportData;

    class VM
    {
    public:
        VM(ErrorReporting& ErrorReporting);
        ~VM() = default;

        bool Prepare(const std::string& data);
        bool Tick(std::vector<SequenceStep>& stepQueue, const int globalCount);
        void Reset();

#if _TEST
        StepData GetTopStackValue() { return mTopStackValue; }
#endif

    private:
        VM() = delete;
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
        bool ExecuteTickInstruction(const Instruction& instruction, const int globalCount,
                                    Stack<StepData>& stack, std::vector<SequenceStep>& stepQueue);

        ErrorReporting& mErrorReporting;
        Scanner mScanner;
        Compiler mCompiler;

        std::vector<DataSequence> mVariables;
        std::vector<Instruction> mRuntimeInstructions;
        std::vector<std::vector<std::vector<Instruction>>> mFunctionArrays;
        inline DataUnit RandomValue(const DataUnit low, const DataUnit high);
        
        template <typename Operation>
        inline void PopDoOperationAndPush(Operation op, Stack<StepData>& stack)
        {
            static_assert(std::is_invocable_v<Operation, const int, const int>,
                "Operation must be callable with two int parameters");

            const StepData& b = stack.Pop();
            const StepData& a = stack.Pop();
            const StepData result = a.ApplySequenceWithOperation(b, op);
            stack.Push(StepData{ result });
        }
        
        static unsigned int mRanSeed;
        inline int fast_rand() {
            mRanSeed = (214013 * mRanSeed + 2531011);
            return (mRanSeed >> 16) & 0x7FFF;
        }
    };

} // namespace ORchestra
