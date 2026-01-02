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

#include <JuceHeader.h>
#include <memory>
#include <ctime>
#include <mutex>
#include <condition_variable>

#include "TransportData.h"
#include "MidiScheduler.h"
#include "VM.h"
#include "FileLoader.h"
#include "ErrorReporting.h"

namespace ORchestra {

    constexpr int STEP_BUFFER_SIZE = 32;
    constexpr int STEP_BUFFER_SIZE_MASK = STEP_BUFFER_SIZE - 1;
    constexpr int HALF_STEP_BUFFER_SIZE = STEP_BUFFER_SIZE / 2;

    static_assert(STEP_BUFFER_SIZE > 0 && 
                  (STEP_BUFFER_SIZE & (STEP_BUFFER_SIZE - 1)) == 0, 
                  "STEP_BUFFER_SIZE must be a power of two");

    class ORchestraEngine
    {
    public:
        ORchestraEngine();
        ~ORchestraEngine();
        void Tick(const TransportData& transportData, const int bufferLength, juce::MidiBuffer& midiMessages);
        void Compile(const std::string& data);
        const std::string& ImportFromFile(const std::string& filePath);
        void ExportToFile(const std::string& filePath);

        std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE>& GetStepData() { return mStepRingBuffer; }
        int GetGlobalStepCount() { return mCurrentGlobalStep.load(); }
        const std::vector<LogEntry>& GetErrors() { return mVM->GetErrors(); }
        const std::string& GetInstructionData() { return mInstructionData; }
        void SetInstructionData(const std::string& data) { mInstructionData = data; }
        bool IsVMInit() { return mIsVMInit.load(); }

    private:
        void WakeWorker();
        void WorkerThreadLoop();
        bool PreProcessSteps();
        inline void Initialize();

        int mLastStep = -1;
        int64_t mSamplesSinceLastStep = 0;
        std::atomic<int> mReadySteps;
        std::atomic<int> mCurrentGlobalStep;
        std::atomic<int> mCurrentProcessingStep;
        std::atomic<bool> mIsVMInit;
        std::atomic<bool> mShouldExit;
        std::atomic<bool> mHasWork;

        std::mutex mCVMutex;
        std::condition_variable mCV;
        std::thread mWorkerThread;
        std::unique_ptr<VM> mVM;
        std::unique_ptr<FileLoader> mFileLoader;
        std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE> mStepRingBuffer;

        std::string mInstructionData;

        MidiScheduler mMidiScheduler;
    };


} // namespace ORchestra
