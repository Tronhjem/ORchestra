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

#include <cmath>
#include <string>

#include "ORchestraEngine.h"
#include "Defines.h"
#if defined(_DEBUG)
#include "ScopedTimer.h"
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
        mFileLoader->SaveToFile(filePath, mInstructionData);
    }

    const std::string& ORchestraEngine::ImportFromFile(const std::string& filePath)
    {
        mInstructionData = mFileLoader->LoadFile(filePath);
        return mInstructionData;
    }

    void ORchestraEngine::Compile(const std::string& data)
    {
        mInstructionData = data;
        Initialize();
    }

    void ORchestraEngine::Initialize()
    {
        mIsVMInit.store(false, std::memory_order_release);

        mReadySteps.store(0, std::memory_order_release);
        mCurrentProcessingStep.store(mCurrentGlobalStep.load(), std::memory_order_release);
        mSamplesPerStep = 0.0;
        mStepOriginInSamples = 0;
        mLastBpm = 0.0;
        mLastBpmDivision = 0.0f;
        mShouldResetScriptBpm.store(true, std::memory_order_release);

        mVM.Reset();

        const bool innitSuccess = mVM.Prepare(&mInstructionData[0]);
        if (innitSuccess)
        {
            mErrorReporting.LogMessage("Compiled Successfully!");
        }
        
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
                    return mHasWork.load(std::memory_order_acquire) || mShouldExit.load(std::memory_order_acquire);
                });
            } // end lock scope
                
            if (!mShouldExit.load(std::memory_order_relaxed)) 
            {
                if (PreProcessSteps())
                    mHasWork.store(false, std::memory_order_release);
            }
        }
    }

    bool ORchestraEngine::PreProcessSteps()
    {
        if (!mIsVMInit.load())
            return false;

        const int readySteps = mReadySteps.load();
        const int stepsToProcess = STEP_BUFFER_SIZE - 1 - readySteps; // leave the last step unprocessed.

#if defined(_DEBUG)
        ScopedTimer timer{ "PreProcess" };
#endif

        // TODO: Make sure this doesn't skip a beat
        const int currentStep = mCurrentProcessingStep.load();
        const int endGlobalStep = stepsToProcess + currentStep;

        for (int i = currentStep; i < endGlobalStep; ++i)
        {
            const int stepWrapped = i & STEP_BUFFER_SIZE_MASK;
            // tick needs global step and StepData needs it wrapped for ring buffer.

            std::vector<SequenceStep>& currentData = mStepRingBuffer[static_cast<unsigned long>(stepWrapped)];
            currentData.clear();

            mVM.Tick(currentData, i);
            mReadySteps.fetch_add(1, std::memory_order_acq_rel);
        }

        mCurrentProcessingStep.fetch_add(stepsToProcess, std::memory_order_acq_rel);
        
        return true;
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
            // If the script hasn't set a BPM, use the host BPM as the active value.
            if (transportData.bpmFromScript == 0.0)
                transportData.bpmFromScript = transportData.bpm;

            const double newSamplesPerStep =
                static_cast<double>(transportData.sampleRate)
                * (60.0 / (transportData.bpmFromScript * transportData.bpmDivision));

            // When BPM or division changes, rebase the step origin so the step counter
            // advances continuously without jumping. A jump causes false resets and retriggering.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
            const bool timingChanged = (transportData.bpmFromScript != mLastBpm)
                                    || (transportData.bpmDivision   != mLastBpmDivision);
#pragma clang diagnostic pop

            if (timingChanged && mSamplesPerStep > 0.0)
            {
                const int lastStep = mCurrentGlobalStep.load();
                mStepOriginInSamples = transportData.timeInSamples
                    - static_cast<int64_t>(std::round(static_cast<double>(lastStep) * newSamplesPerStep));
            }

            mLastBpm = transportData.bpmFromScript;
            mLastBpmDivision = transportData.bpmDivision;
            mSamplesPerStep = newSamplesPerStep;
            const double samplesPerStep = mSamplesPerStep;

            const int currentStep = static_cast<int>(ceil(
                static_cast<double>(transportData.timeInSamples - mStepOriginInSamples) / samplesPerStep));

            // Check if we skipped count, to regenerate everything.
            const int stepDifference = currentStep - mCurrentGlobalStep.load();
            mCurrentGlobalStep.store(currentStep, std::memory_order_release);

            if (stepDifference > 1 || stepDifference < 0)
            {
                // Real time jump (seek or restart) — clear the origin so next tick
                // recomputes from scratch with the actual time position.
                mSamplesPerStep = 0.0;
                mStepOriginInSamples = 0;
                mReadySteps.store(1, std::memory_order_release);
                mCurrentProcessingStep.store(currentStep, std::memory_order_release);
            }

            const int nextStepInSamples = static_cast<int>(
                static_cast<double>(mStepOriginInSamples) + samplesPerStep * static_cast<double>(currentStep));
            const int endOfBufferInSamples = static_cast<int>(transportData.timeInSamples + bufferLength);

            // if the end of the buffer is longer than the next tick time
            // Check if we should tick in this buffer.
            if (endOfBufferInSamples >= nextStepInSamples && currentStep != mLastStep)
            {
#if defined(_DEBUG)
//               ScopedTimer timer{ "Process Beat" };
#endif
               mLastStep = currentStep;

               mSamplesSinceLastStep = transportData.timeInSamples;
               const int wrappedGlobalStep = currentStep & STEP_BUFFER_SIZE_MASK;
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
                       case ORchestra::SequenceStepType::BPM_DIVISION:
                       {
                           transportData.bpmDivision = ToBpmDivision(step.mFirst.GetValue(0));

                           break;
                       }
                       case ORchestra::SequenceStepType::PRINT:
                       {
                           const std::string mes = 
                               std::to_string(static_cast<int>(step.mFirst.GetValue(0)));

                           mErrorReporting.LogMessage(mes);

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
                               ToBpmDivision(static_cast<DataUnit>(step.mDuration));
                           const int noteDurationSamples =
                               static_cast<int>(
                                   static_cast<double>(transportData.sampleRate)
                                   * (60.0 / (transportData.bpmFromScript * noteDivFloat)));

                           for (int i = 0; i < triggerLength; ++i)
                           {
                               const DataUnit shouldTrigger = step.mShouldTrigger.GetValue(i);

                               if (!shouldTrigger)
                                    continue;

                               const int rawFirstByte =
                                   static_cast<int>(step.mFirst.GetEquivalentValueAtIndex(i, triggerLength));

                               const int transposedFirstByte =
                                   (step.mType == ORchestra::SequenceStepType::NoteOn)
                                   ? rawFirstByte + transportData.transposeOffset
                                   : rawFirstByte;

                               const DataUnit firstByte = static_cast<DataUnit>(transposedFirstByte);
                               const DataUnit secondByte =
                                   step.mSecond.GetEquivalentValueAtIndex(i, triggerLength);

                               const DataUnit channel =
                                   step.mChannel.GetEquivalentValueAtIndex(i, triggerLength);

                               const int timeStamp = nextStepInSamples
                                   + i * (static_cast<int>(samplesPerStep) / triggerLength);

                               ScheduledMidiMessage message{ step.mType, firstByte, secondByte,
                                                             channel, timeStamp, noteDurationSamples };

                               mMidiScheduler.PostMidi(message);
                           }

                           break;
                       }
                   }
               }

               mReadySteps.fetch_sub(1, std::memory_order_acq_rel);
               if (mReadySteps.load() < HALF_STEP_BUFFER_SIZE) /*magic number for processing steps earlier than half*/
                   WakeWorker();
            }

            // Process all Midi.
            mMidiScheduler.ProcessMidiPosts(midiMessages, bufferLength, endOfBufferInSamples);
        }
        else
        {
            mMidiScheduler.ClearAllData(midiMessages);
        }
    }

    void ORchestraEngine::RequestClearErrors()
    { 
        if (mIsRunning.load())
            mErrorReporting.RequestClear();
        else
            mErrorReporting.Clear();
    }

    float ORchestraEngine::ToBpmDivision(DataUnit divValue)
    {
        const int divIndex = std::clamp(static_cast<int>(divValue), 1, 7);
        float bpmDivision = 1.0f;
        switch (divIndex)
        {
        case 1:
            bpmDivision = 0.25f;
            break;
        case 2:
            bpmDivision = 0.5f;
            break;
        case 3:
            bpmDivision = 1.0f;
            break;
        case 4:
            bpmDivision = 2.0f;
            break;
        case 5:
            bpmDivision = 4.0f;
            break;
        case 6:
            bpmDivision = 8.0f;
            break;
        case 7:
            bpmDivision = 16.0f;
            break;
        }
        
        return bpmDivision;
    }
} // namespace ORchestra
