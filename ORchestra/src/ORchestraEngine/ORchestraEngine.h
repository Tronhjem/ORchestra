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
#include <shared_mutex>
#include <condition_variable>

#include "TransportData.h"
#include "MidiScheduler.h"
#include "VM.h"
#include "FileLoader.h"
#include "ErrorReporting.h"
#include "Defines.h"

namespace ORchestra
{
    class ORchestraEngine
    {
    public:
        ORchestraEngine();
        ~ORchestraEngine();
        void Tick(TransportData& transportData, const int bufferLength, juce::MidiBuffer& midiMessages);
        void Compile(const std::string& data);
        std::string ImportFromFile(const std::string& filePath);
        void ExportToFile(const std::string& filePath);

        // Returns a copy of one ring buffer slot, taken under that slot's lock.
        // Readers (Timeline, stress harness) must use this; the ring buffer is
        // rewritten by the worker thread.
        std::vector<SequenceStep> GetStepDataSlotCopy(const size_t slotIndex)
        {
            std::shared_lock lock {mRingBufferMutexes[slotIndex]};
            return mStepRingBuffer[slotIndex];
        }

        // Copy-assign reuses out's capacity, so steady-state callers allocate
        // nothing. The lock is held only for the copy.
        void CopyStepDataSlot(const size_t slotIndex, std::vector<SequenceStep>& out)
        {
            std::shared_lock lock {mRingBufferMutexes[slotIndex]};
            out = mStepRingBuffer[slotIndex];
        }

        int GetGlobalStepCount() { return mCurrentGlobalStep.load(); }
        std::vector<LogEntry> GetErrors() { return mErrorReporting.GetErrors(); }

        std::string GetInstructionData()
        {
            std::scoped_lock lock {mInstructionDataMutex};
            return mInstructionData;
        }

        void SetInstructionData(const std::string& data)
        {
            std::scoped_lock lock {mInstructionDataMutex};
            mInstructionData = data;
        }
        bool IsVMInit() { return mIsVMInit.load(); }
        
        void SetErrorListener(ErrorReportingListener* listener) { mErrorReporting.SetListener(listener); }
#if defined(_DEBUG)
        void SetLogSink(LogSinkFn sink) { mErrorReporting.SetSink(std::move(sink)); }
#endif
        void RequestClearErrors();

    private:
        inline void Reset();
        void WakeWorker();
        void WorkerThreadLoop();
        bool PreProcessSteps();
        void HandleSeekRequest();
        inline bool ProcessStepData(TransportData& transportData, const int currentStep, const int nextStepInSamples, const double samplesPerStep);
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
        // One shared_mutex per ring slot: worker takes it exclusively while
        // clearing/filling; audio (try) and UI both take it shared, so readers
        // never block each other.
        std::array<std::shared_mutex, STEP_BUFFER_SIZE> mRingBufferMutexes;
        // Guards mInstructionData and mPendingInstructionData (UI vs worker).
        std::mutex mInstructionDataMutex;

        std::string mInstructionData;
        std::string mPendingInstructionData;

        MidiScheduler mMidiScheduler;
        ErrorReporting mErrorReporting;
        VM mVM;
    };

} // namespace ORchestra
