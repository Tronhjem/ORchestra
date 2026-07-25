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

#include <chrono>
#include <cmath>
#include <string>
#include <thread>

#include "ORchestraEngine.h"
#include "Defines.h"
#include "TransportData.h"
#include "NoteDivision.h"

#if defined(_DEBUG)
#include "ScopedTimer.h"
#include "ORchestraAssert.h"
#endif

namespace ORchestra 
{
    ORchestraEngine::ORchestraEngine() : mReadySteps(0),
        mCurrentGlobalStep(0),
        mCurrentProcessingStep(0),
        mIsVMInit(false),
        mShouldExit(false),
        mHasWork(false),
        mErrorReporting(),
        mVM(mErrorReporting)
    {
        mFileLoader = std::make_unique<FileLoader>();
        mWorkerThread = std::thread([this]() { WorkerThreadLoop(); });

        for(auto& stepBuffer : mStepRingBuffer)
            stepBuffer.reserve(10); // Magic value estimated for a good start size. 
    }

    ORchestraEngine::~ORchestraEngine()
    {
        mShouldExit.store(true, std::memory_order_release);
        mCV.notify_one();

        if (mWorkerThread.joinable())
            mWorkerThread.join();
    }

    void ORchestraEngine::ExportToFile(const std::string& filePath)
    {
        std::scoped_lock lock {mInstructionDataMutex};
        mFileLoader->SaveToFile(filePath, mInstructionData);
    }

    std::string ORchestraEngine::ImportFromFile(const std::string& filePath)
    {
        std::scoped_lock lock {mInstructionDataMutex};
        mInstructionData = mFileLoader->LoadFile(filePath);
        return mInstructionData;
    }

    void ORchestraEngine::Compile(const std::string& data)
    {
        {
            std::scoped_lock lock {mInstructionDataMutex};
            mPendingInstructionData = data;
        }
        mResetRequest.fetch_add(1, std::memory_order_acq_rel);
        WakeWorker();
    }

    void ORchestraEngine::Reset()
    {
        // !!! Worker-thread only !!!
        
        mIsVMInit.store(false, std::memory_order_release);

        mReadySteps.store(0, std::memory_order_release);
        mCurrentProcessingStep.store(mCurrentGlobalStep.load(), std::memory_order_release);
        mSamplesPerStep = 0.0;
        mStepOriginInSamples = 0;
        mLastBpm = 0.0;
        mLastBpmDivision = 0.0f;
        mShouldResetScriptBpm.store(true, std::memory_order_release);

        mVM.Reset();

        std::string instructionDataSnapshot;
        {
            std::scoped_lock lock {mInstructionDataMutex};
            instructionDataSnapshot = mInstructionData;
        }

        const bool innitSuccess = mVM.Prepare(instructionDataSnapshot);
        if (innitSuccess)
            mErrorReporting.LogMessage("Compiled Successfully!");

        mIsVMInit.store(innitSuccess);

        if (innitSuccess)
            WakeWorker();
    }

    void ORchestraEngine::WakeWorker()
    {
        mHasWork.store(true, std::memory_order_release);
        mCV.notify_one();
    }

    void ORchestraEngine::WorkerThreadLoop()
    {
        while (!mShouldExit.load(std::memory_order_relaxed))
        {
            { // Lock scope
                std::unique_lock<std::mutex> lock(mCVMutex);
                mCV.wait(lock, [this] {
                    return mHasWork.load(std::memory_order_acquire)
                        || mResetRequest.load(std::memory_order_acquire) != mResetRequestSeen
                        || mShouldExit.load(std::memory_order_acquire);
                });
            } // end lock scope

            if (mShouldExit.load(std::memory_order_relaxed))
                break;

            // Service a pending recompile before any further VM::Tick calls (#2).
            while (mResetRequest.load(std::memory_order_acquire) != mResetRequestSeen)
            {
                mResetRequestSeen = mResetRequest.load(std::memory_order_acquire);
                {
                    std::scoped_lock lock {mInstructionDataMutex};
                    mInstructionData = mPendingInstructionData;
                }
                Reset();
            }

            // Service a pending seek before any further ring buffer writes (#1). The
            // audio thread records a seek instead of mutating state itself; this is the
            // only place the counters, origin, and ring buffer slots get torn down and
            // rebuilt, so the worker is never iterating slots the audio thread is reading.
            while (mSeekRequest.load(std::memory_order_acquire) != mSeekRequestSeen)
            {
                mSeekRequestSeen = mSeekRequest.load(std::memory_order_acquire);
                HandleSeekRequest();
            }

            if (PreProcessSteps())
                mHasWork.store(false, std::memory_order_release);
        }
    }

    bool ORchestraEngine::PreProcessSteps()
    {
        if (!mIsVMInit.load())
            return false;

        const int readySteps = mReadySteps.load();
#if defined(_DEBUG)
        ORCHESTRA_ASSERT_SIMPLE(readySteps >= 0);
#endif
        int stepsToProcess = STEP_BUFFER_SIZE - 1 - readySteps; // leave the last step unprocessed.
        if (stepsToProcess < 0)
            stepsToProcess = 0;
        if (stepsToProcess > STEP_BUFFER_SIZE - 1)
            stepsToProcess = STEP_BUFFER_SIZE - 1;

#if defined(_DEBUG)
        ScopedTimer timer{ "PreProcess" };
#endif

        // TODO: Make sure this doesn't skip a beat
        const int currentStep = mCurrentProcessingStep.load();
        const int endGlobalStep = stepsToProcess + currentStep;

        for (int i = currentStep; i < endGlobalStep; ++i)
        {
            if (mResetRequest.load(std::memory_order_acquire) != mResetRequestSeen)
                return false;

            // Tick needs global step and StepData needs it wrapped for ring buffer.
            const int stepWrapped = i & STEP_BUFFER_SIZE_MASK;

            std::vector<SequenceStep>& currentData = mStepRingBuffer[static_cast<unsigned long>(stepWrapped)];

            std::scoped_lock slotLock {mRingBufferMutexes[static_cast<unsigned long>(stepWrapped)]};
#if defined(_DEBUG) && defined(ORCHESTRA_RACE_WIDEN)
            std::this_thread::sleep_for(std::chrono::microseconds(500));
#endif

            currentData.clear();

            mVM.Tick(currentData, i);
            mReadySteps.fetch_add(1, std::memory_order_acq_rel);
        }

        mCurrentProcessingStep.fetch_add(stepsToProcess, std::memory_order_acq_rel);

        return true;
    }

    void ORchestraEngine::HandleSeekRequest()
    {
        const int targetStep = mSeekTargetStep.load(std::memory_order_acquire);

        for (size_t i = 0; i < mStepRingBuffer.size(); ++i)
        {
            std::scoped_lock slotLock {mRingBufferMutexes[i]};
#if defined(_DEBUG) && defined(ORCHESTRA_RACE_WIDEN)
            std::this_thread::sleep_for(std::chrono::microseconds(100));
#endif
            mStepRingBuffer[i].clear();
        }

        mStepOriginInSamples = 0;
        mSamplesPerStep = 0.0;

        mCurrentProcessingStep.store(targetStep, std::memory_order_release);
        mCurrentGlobalStep.store(targetStep, std::memory_order_release);
        mReadySteps.store(0, std::memory_order_release);

        mHasWork.store(true, std::memory_order_release);
    }

    void ORchestraEngine::Tick(TransportData& transportData,
        const int bufferLength,
        juce::MidiBuffer& midiMessages)
    {
        mIsRunning.store(transportData.isPlaying);

        mErrorReporting.CheckAndClear();

        if (mShouldResetScriptBpm.exchange(false, std::memory_order_acq_rel))
            transportData.bpmFromScript = 0.0;

        if (transportData.isPlaying && mIsVMInit)
        {
            TickInternal(transportData, bufferLength);

            mMidiScheduler.ProcessMidiPosts(midiMessages, bufferLength);
        }
        else
        {
            mMidiScheduler.ClearAllData(midiMessages);
        }
    }

    void ORchestraEngine::TickInternal(TransportData& transportData, const int bufferLength)
    {
        // If the script hasn't set a BPM, use the host BPM as the active value.
        if (transportData.bpmFromScript == 0.0)
            transportData.bpmFromScript = transportData.bpm;

        const double samplesPerStep =
            static_cast<double>(transportData.sampleRate)
            * (60.0 / (transportData.bpmFromScript * transportData.beatDivision));

        // When BPM or division changes, rebase the step origin so the step counter
        // advances continuously without jumping. A jump causes false resets and retriggering.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
        const bool timingChanged = (transportData.bpmFromScript != mLastBpm)
            || (transportData.beatDivision   != mLastBpmDivision);
#pragma clang diagnostic pop

        if (timingChanged && mSamplesPerStep > 0.0)
        {
            const int lastStep = mCurrentGlobalStep.load();
            mStepOriginInSamples = transportData.timeInSamples
                - static_cast<int64_t>(std::round(static_cast<double>(lastStep) * samplesPerStep));
        }

        mLastBpm = transportData.bpmFromScript;
        mLastBpmDivision = transportData.beatDivision;
        mSamplesPerStep = samplesPerStep;

        const int currentStep = static_cast<int>(ceil(
                    static_cast<double>(transportData.timeInSamples - mStepOriginInSamples) / samplesPerStep));

        // Check if we skipped count, to regenerate everything.
        const int stepDifference = currentStep - mCurrentGlobalStep.load();
        mCurrentGlobalStep.store(currentStep, std::memory_order_release);

        if (stepDifference > 1 || stepDifference < 0)
        {
            // Force the first step after the seek to process even if its step
            // number equals the stale mLastStep. Only written on this thread.
            mLastStep = -1;
            mSeekTargetStep.store(currentStep, std::memory_order_release);
            mSeekRequest.fetch_add(1, std::memory_order_acq_rel);
            WakeWorker();
            return;
        }

        const int nextStepInSamples = static_cast<int>(
                static_cast<double>(mStepOriginInSamples) + samplesPerStep * static_cast<double>(currentStep));

        const int endOfBufferInSamples = static_cast<int>(transportData.timeInSamples + bufferLength);

        if (endOfBufferInSamples >= nextStepInSamples
            && currentStep != mLastStep
            && mReadySteps.load(std::memory_order_acquire) > 0)
        {
            if (ProcessStepData(transportData, currentStep, nextStepInSamples, samplesPerStep))
            {
                int ready = mReadySteps.load(std::memory_order_acquire);
                while (ready > 0
                       && !mReadySteps.compare_exchange_weak(ready, ready - 1,
                                                              std::memory_order_acq_rel))
                {
                }

#if defined(_DEBUG)
                ORCHESTRA_ASSERT_SIMPLE(mReadySteps.load(std::memory_order_acquire) >= 0);
#endif
                if (mReadySteps.load() < HALF_STEP_BUFFER_SIZE)
                    WakeWorker();
            }
        }
    }

    bool ORchestraEngine::ProcessStepData(TransportData& transportData, const int currentStep, const int nextStepInSamples, const double samplesPerStep)
    {
#if defined(_DEBUG)
        // ScopedTimer timer{ "Process Beat" };
#endif
        mSamplesSinceLastStep = transportData.timeInSamples;
        const int wrappedGlobalStep = currentStep & STEP_BUFFER_SIZE_MASK;

        std::shared_lock<std::shared_mutex> slotLock {
            mRingBufferMutexes[static_cast<unsigned long>(wrappedGlobalStep)], std::try_to_lock};
        if (!slotLock.owns_lock())
            return false;

        mLastStep = currentStep;

#if defined(_DEBUG) && defined(ORCHESTRA_RACE_WIDEN)
        // Stress-only window widening.
        std::this_thread::sleep_for(std::chrono::microseconds(500));
#endif

        const std::vector<SequenceStep>& currentData =
            mStepRingBuffer[static_cast<unsigned long>(wrappedGlobalStep)];

        for (const SequenceStep& step : currentData)
        {
            switch (step.mType)
            {
                case ORchestra::SequenceStepType::BPM:
                    {
                        transportData.bpmFromScript = step.mFirst.GetValue(0);

                        break;
                    }
                case ORchestra::SequenceStepType::BEAT:
                    {
                        transportData.beatDivision = DurationToBpmDivision(step.mFirst.GetValue(0));

                        break;
                    }
                case ORchestra::SequenceStepType::PRINT:
                    {
                        const std::string mes =
                            std::to_string(static_cast<int>(step.mFirst.GetValue(0)));

                        // Audio thread must not block on the log mutex; drop on contention.
                        mErrorReporting.TryLogMessage(mes);

                        break;
                    }
                case ORchestra::SequenceStepType::TRANSPOSE:
                    {
                        transportData.transposeOffset = static_cast<int>(step.mFirst.GetValue(0));

                        break;
                    }
                case ORchestra::SequenceStepType::NoteOn:
                case ORchestra::SequenceStepType::NoteOff:
                case ORchestra::SequenceStepType::CC:
                    {
                        const int triggerLength = static_cast<int>(step.mShouldTrigger.GetLength());
                        const float noteDivFloat =
                            DurationToBpmDivision(static_cast<DataUnit>(step.mDuration));
                        const int noteDurationSamples =
                            static_cast<int>(
                                    static_cast<double>(transportData.sampleRate)
                                    * (60.0 / (transportData.bpmFromScript * noteDivFloat)));

                        for (int i = 0; i < triggerLength; ++i)
                        {
                            const DataUnit shouldTrigger = step.mShouldTrigger.GetValue(i);

                            if (!shouldTrigger)
                                continue;

                            const int rawFirst =
                                static_cast<int>(step.mFirst.GetEquivalentValueAtIndex(i, triggerLength));

                            const int transposedFirstByte =
                                (step.mType == ORchestra::SequenceStepType::NoteOn)
                                ? rawFirst + transportData.transposeOffset
                                : rawFirst;

                            const DataUnit firstData = static_cast<DataUnit>(transposedFirstByte);
                            const DataUnit secondData =
                                step.mSecond.GetEquivalentValueAtIndex(i, triggerLength);

                            const DataUnit channel =
                                step.mChannel.GetEquivalentValueAtIndex(i, triggerLength);

                            const int timeStamp = nextStepInSamples + i * (static_cast<int>(samplesPerStep) / triggerLength);
                            const int remainingSamples = timeStamp - static_cast<int>(transportData.timeInSamples);

                            ScheduledMidiMessage message{ step.mType, firstData, secondData,
                                channel, remainingSamples, noteDurationSamples };

                            mMidiScheduler.PostMidi(message);
                        }

                        break;
                    }
            }
        }

        return true;
    }

    void ORchestraEngine::RequestClearErrors()
    {
        if (mIsRunning.load())
            mErrorReporting.RequestClear();
        else
            mErrorReporting.Clear();
    }
} // namespace ORchestra
