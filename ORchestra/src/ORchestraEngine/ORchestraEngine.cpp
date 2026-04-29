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
#if _DEBUG
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

#if _DEBUG
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

        if (transportData.isPlaying && mIsVMInit)
        {
            // If the script hasn't set a BPM, use the host BPM as the active value.
            if (transportData.bpmFromScript == 0.0)
                transportData.bpmFromScript = transportData.bpm;

            const double samplesPerStep =
                static_cast<double>(transportData.sampleRate)
                * (60.0 / (transportData.bpmFromScript * transportData.bpmDivision));

            const int currentStep = 
                static_cast<int>(ceil(static_cast<double>(transportData.timeInSamples) / samplesPerStep));

            // Check if we skipped count, to regenerate everything.
            const int stepDifference = currentStep - mCurrentGlobalStep.load();
            mCurrentGlobalStep.store(currentStep, std::memory_order_release);

            if (stepDifference > 1 || stepDifference < 0)
            {
                // TODO: should it be 1 or 0?
                // TODO: Should we clear hanging notes?
                // Do we want to move it entirely down to 0 or still keep the current step we might trigger?
                mReadySteps.store(1, std::memory_order_release);
                mCurrentProcessingStep.store(currentStep, std::memory_order_release);
            }

            const int nextStepInSamples = static_cast<int>(samplesPerStep * currentStep);
            const int endOfBufferInSamples = static_cast<int>(transportData.timeInSamples + bufferLength);

            // if the end of the buffer is longer than the next tick time
            // Check if we should tick in this buffer.
            if (endOfBufferInSamples >= nextStepInSamples && currentStep != mLastStep)
            {
#if _DEBUG
//               ScopedTimer timer{ "Process Beat" };
#endif
               mLastStep = currentStep;

               mSamplesSinceLastStep = transportData.timeInSamples;
               const int wrappedGlobalStep = currentStep & STEP_BUFFER_SIZE_MASK;
               const std::vector<SequenceStep>& currentData = mStepRingBuffer[static_cast<unsigned long>(wrappedGlobalStep)];
                
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
                           const std::string mes = std::to_string(static_cast<int>(step.mFirst.GetValue(0)));
                           mErrorReporting.LogMessage(mes, mCurrentGlobalStep.load());
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
                           for (int i = 0; i < triggerLength; ++i)
                           {
                               const DataUnit shouldTrigger = step.mShouldTrigger.GetValue(i);

                               if (!shouldTrigger)
                                    continue;

                               const int rawFirstByte = static_cast<int>(step.mFirst.GetEquivalentValueAtIndex(i, triggerLength));
                               const int transposedFirstByte = (step.mType == ORchestra::SequenceStepType::NoteOn)
                                   ? rawFirstByte + transportData.transposeOffset
                                   : rawFirstByte;
                               const DataUnit firstByte = static_cast<DataUnit>(transposedFirstByte);
                               const DataUnit secondByte = step.mSecond.GetEquivalentValueAtIndex(i, triggerLength);
                               const DataUnit channel = step.mChannel.GetEquivalentValueAtIndex(i, triggerLength);
                               const int timeStamp = nextStepInSamples + i * (static_cast<int>(samplesPerStep) / triggerLength);

                               const float noteDivFloat = ToBpmDivision(static_cast<DataUnit>(step.mDuration));
                               const int noteDurationSamples = static_cast<int>(
                                   static_cast<double>(transportData.sampleRate) * (60.0 / (transportData.bpmFromScript * noteDivFloat)));
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
