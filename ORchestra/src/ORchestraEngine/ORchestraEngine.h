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
#include <mutex>
#include <condition_variable>

#include "TransportData.h"
#include "MidiScheduler.h"
#include "VM.h"
#include "FileLoader.h"
#include "ErrorReporting.h"
#include "Defines.h"

#if defined (_DEBUG)
#include "AssertMutex.h"
#endif

namespace ORchestra 
{
    class ORchestraEngine
    {
    public:
        ORchestraEngine();
        ~ORchestraEngine();
        void Tick(TransportData& transportData, const int bufferLength, juce::MidiBuffer& midiMessages);
        void Compile(const std::string& data);
        const std::string& ImportFromFile(const std::string& filePath);
        void ExportToFile(const std::string& filePath);

        std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE>& GetStepData() { return mStepRingBuffer; }
        int GetGlobalStepCount() { return mCurrentGlobalStep.load(); }
        const std::vector<LogEntry>& GetErrors() { return mErrorReporting.GetErrors(); }
        const std::string& GetInstructionData() { return mInstructionData; }
        void SetInstructionData(const std::string& data) { mInstructionData = data; }
        bool IsVMInit() { return mIsVMInit.load(); }
        
        void SetErrorListener(ErrorReportingListener* listener) { mErrorReporting.SetListener(listener); }
        void SetLogSink(LogSinkFn sink) { mErrorReporting.SetSink(std::move(sink)); }
        void RequestClearErrors();

    private:
        inline void Reset();
        void WakeWorker();
        void WorkerThreadLoop();
        bool PreProcessSteps();
        void HandleSeekRequest();
        inline void ProcessStepData(TransportData& transportData, const int currentStep, const int nextStepInSamples, const double samplesPerStep);
        inline void TickInternal(TransportData& transportData, const int bufferLength);


        int mLastStep = -1;
        int64_t mSamplesSinceLastStep = 0;
        double mSamplesPerStep = 0.0;
        int64_t mStepOriginInSamples = 0;
        double mLastBpm = 0.0;
        float mLastBpmDivision = 0.0f;
        std::atomic<int> mReadySteps;
        std::atomic<int> mCurrentGlobalStep;
        std::atomic<int> mCurrentProcessingStep;
        std::atomic<bool> mIsVMInit;
        std::atomic<bool> mShouldExit;
        std::atomic<bool> mHasWork;
        std::atomic<bool> mIsRunning;
        std::atomic<bool> mShouldResetScriptBpm {false};

        // UI thread bumps mResetRequest to ask the worker to recompile (issue #2).
        std::atomic<int> mResetRequest {0};
        int mResetRequestSeen = 0;

        // Audio thread bumps mSeekRequest to ask the worker to rebase the step origin
        // and re-clear the ring buffer after a DAW seek/restart (issue #1). Pattern is
        // the same as mResetRequest: monotonic counter, worker dedups against seen value.
        // The audio thread does NOT perform the rebase inline; it records the target
        // step and skips ProcessStepData on the seek callback itself, so the worker
        // gets exclusive ownership of the ring buffer slots before clearing them.
        std::atomic<int> mSeekRequest {0};
        int mSeekRequestSeen = 0;
        std::atomic<int> mSeekTargetStep {0};

        std::mutex mCVMutex;
        std::condition_variable mCV;
        std::thread mWorkerThread;
        std::unique_ptr<FileLoader> mFileLoader;
        std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE> mStepRingBuffer;
#if defined (_DEBUG)
        std::array<AssertMutex, STEP_BUFFER_SIZE> mRingBufferMutexes;
#endif

        std::string mInstructionData;
        std::string mPendingInstructionData;

        MidiScheduler mMidiScheduler;
        ErrorReporting mErrorReporting;
        VM mVM;
    };

} // namespace ORchestra
