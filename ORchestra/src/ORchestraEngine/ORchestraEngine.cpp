#include <cmath>

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
        mHasWork(false)
    {
        mVM = std::make_unique<VM>();
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
        
        // Reset script transport data to default values (0 means not set)
        mScriptTransportData.bpm = 0.0;
        mScriptTransportData.bpmDivision = 0.0f;
        
        mVM->Reset();

        const bool innitSuccess = mVM->Prepare(&mInstructionData[0]);
        mIsVMInit.store(innitSuccess);
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

            mVM->Tick(currentData, i);
            mReadySteps.fetch_add(1, std::memory_order_acq_rel);
        }

        mCurrentProcessingStep.fetch_add(stepsToProcess, std::memory_order_acq_rel);
        
        return true;
    }

    void ORchestraEngine::Tick(const TransportData& transportData,
        const int bufferLength,
        juce::MidiBuffer& midiMessages)
    {
        if (transportData.isPlaying && mIsVMInit)
        {
            // Use script-provided BPM and division if they were set, otherwise use the provided values
            const double bpm = mScriptTransportData.bpm > 0.0 ? mScriptTransportData.bpm : transportData.bpm;
            const float bpmDivision = mScriptTransportData.bpmDivision > 0.0f ? mScriptTransportData.bpmDivision : transportData.bpmDivision;
            
            const double samplesPerStep = static_cast<double>(transportData.sampleRate) * (60.0 / (bpm * bpmDivision));
            const int currentStep = static_cast<int>(ceil(static_cast<double>(transportData.timeInSamples) / samplesPerStep));

            // Check if we skipped count, to regenerate everything.
            const int stepDifference = currentStep - mCurrentGlobalStep.load();
            mCurrentGlobalStep.store(currentStep, std::memory_order_release);

            if (stepDifference > 1 || stepDifference < 0)
            {
                // TODO: should it be 1 or 0?
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
               const int wrappedGlobalStep = currentStep % STEP_BUFFER_SIZE;
               const std::vector<SequenceStep>& currentData = mStepRingBuffer[static_cast<unsigned long>(wrappedGlobalStep)];
               for (const SequenceStep& step : currentData)
               {
                   switch (step.mType)
                   {
                       case ORchestra::MidiType::BPM:
                       {
                           mScriptTransportData.bpm = step.mFirst.GetValue(0);
                           break;
                       }
                       case ORchestra::MidiType::NOTE_DIVISION:
                       {
                           mScriptTransportData.bpmDivision = ToBpmDivision(step.mFirst.GetValue(0));
                           break;
                       }
                       case ORchestra::MidiType::NoteOn:
                       case ORchestra::MidiType::NoteOff:
                       case ORchestra::MidiType::CC:
                       {
                           const int triggerLength = static_cast<int>(step.mShouldTrigger.GetLength());
                           for (int i = 0; i < triggerLength; ++i)
                           {
                               const DataUnit shouldTrigger = step.mShouldTrigger.GetValue(i);

                               if (!shouldTrigger)
                                    continue;

                               const DataUnit firstByte = step.mFirst.GetEquivalentValueAtIndex(i, triggerLength);
                               const DataUnit secondByte = step.mSecond.GetEquivalentValueAtIndex(i, triggerLength);
                               const DataUnit channel = step.mChannel.GetEquivalentValueAtIndex(i, triggerLength);
                               const int timeStamp = nextStepInSamples + i * (static_cast<int>(samplesPerStep) / triggerLength);

                               // TODO: Change to use step.mDuration
                               // ScheduledMidiMessage message {step.mType, firstByte, secondByte, channel, timeStamp, step.mDuration};
                               ScheduledMidiMessage message{ step.mType, firstByte, secondByte, 
                                                             channel, timeStamp, transportData.noteLengthInSamples };

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
