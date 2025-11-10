#include <cmath>

#include "ORchestraEngine.h"
#include "ScopedTimer.h"

ORchestraEngine::ORchestraEngine() :
    mReadySteps(0),
    mCurrentGlobalStep(0),
    mCurrentProcessingStep(0),
    mIsVMInit(false),
    shouldExit(false)
{
    mVM = std::make_unique<VM>();
    mFileLoader = std::make_unique<FileLoader>();
    
    mWorkerThread = std::thread([this]() { WorkerThreadLoop(); });
}

ORchestraEngine::~ORchestraEngine()
{
    shouldExit.store(true);
    if (mWorkerThread.joinable())
        mWorkerThread.join();
}

void ORchestraEngine::ExportToFile(const std::string& filePath)
{
    const bool fileSaved = mFileLoader->SaveToFile(filePath, mInstructionData);
    
    if(fileSaved)
    {
        Compile(mInstructionData);
    }
}

const std::string& ORchestraEngine::ImportFromFile(const std::string& filePath)
{
    mInstructionData = mFileLoader->LoadFile(filePath);
    
    if (mInstructionData.length() > 0)
    {
        Compile(mInstructionData);
    }
    
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
    mVM->Reset();
    
    mIsVMInit.store(mVM->Prepare(&mInstructionData[0]));
}

void ORchestraEngine::WorkerThreadLoop()
{
    while (!shouldExit.load())
    {
        PreProcessSteps();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void ORchestraEngine::PreProcessSteps()
{
    if (!mIsVMInit.load())
        return;
    
    const int readySteps = mReadySteps.load();
    const int stepsToProcess = STEP_BUFFER_SIZE - 1 - readySteps; // leave the last step unprocessed.
    
    if(stepsToProcess < HALF_STEP_BUFFER_SIZE - 5 /*magic number for processing steps earlier than half*/)
        return;
    
#if _DEBUG
    ScopedTimer timer {"PreProcess"};
#endif
    
    //TODO: Make sure this doesn't skip a beat
    const int currentStep = mCurrentProcessingStep.load();
    const int endGlobalStep = stepsToProcess + currentStep;
    for (int i = currentStep; i < endGlobalStep; ++i)
    {
        const int stepWrapped = i % STEP_BUFFER_SIZE;
        // tick needs global step and StepData needs it wrapped for ring buffer.
        
        std::vector<SequenceStep>& currentData = mStepRingBuffer[stepWrapped];
        currentData.clear();
        
        mVM->Tick(currentData, i);
#if _DEBUG
        std::cout << "global step process " << stepWrapped << " " << (int)currentData[0].mFirst.GetValue(0) << std::endl;
#endif
        mReadySteps.fetch_add(1, std::memory_order_acq_rel);
    }
    
    mCurrentProcessingStep.fetch_add(stepsToProcess, std::memory_order_acq_rel);
}

void ORchestraEngine::Tick(const TransportData& transportData,
                           const int bufferLength,
                           juce::MidiBuffer& midiMessages)
{
    if (transportData.isPlaying)
    {
        const double samplesPerStep = static_cast<double>(transportData.sampleRate) * (60.0 / (transportData.bpm * transportData.bpmDivision));
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
        
//        if (mIsVMInit && endOfBufferInSamples >= nextStepInSamples && currentStep != mLastStep)
        if (mIsVMInit && endOfBufferInSamples >= nextStepInSamples)
        {
#if _DEBUG
            if (mLastStep == currentStep)
            {
                JUCE_BREAK_IN_DEBUGGER;
            }
            lastTimeInSamples = transportData.timeInSamples;
            mLastStep = currentStep;
#endif
            
//            ScopedTimer timer {"Process Beat"};
            mSamplesSinceLastStep = transportData.timeInSamples;
            const int wrappedGlobalStep = currentStep % STEP_BUFFER_SIZE;
            const std::vector<SequenceStep>& currentData = mStepRingBuffer[wrappedGlobalStep];
            
            for (const SequenceStep& step : currentData)
            {
                const int triggerLength = static_cast<int>(step.mShouldTrigger.GetLength());
                
                for (int i = 0; i < triggerLength; ++i)
                {
                    const uChar shouldTrigger = step.mShouldTrigger.GetValue(i);
                    
                    if(!shouldTrigger)
                        continue;
                    
                    const uChar firstByte = step.mFirst.GetEquivalentValueAtIndex(i, triggerLength);
                    const uChar secondByte = step.mSecond.GetEquivalentValueAtIndex(i, triggerLength);
                    const uChar channel = step.mChannel.GetEquivalentValueAtIndex(i, triggerLength);
                    const int timeStamp = nextStepInSamples + i * (samplesPerStep / triggerLength);

                    // TODO: Change to use step.mDuration
                    // ScheduledMidiMessage message {step.mType, firstByte, secondByte, channel, timeStamp, step.mDuration};
                    ScheduledMidiMessage message {step.mType, firstByte, secondByte, channel, timeStamp, transportData.noteLengthInSamples};
                    
#if _DEBUG
                    std::cout << "play step " << wrappedGlobalStep << " " << (int)firstByte << std::endl;
#endif
                    mMidiScheduler.PostMidi(message);
                }
            }
            
            mReadySteps.fetch_sub(1, std::memory_order_acq_rel);
        }
    
        // Process all Midi.
        mMidiScheduler.ProcessMidiPosts(midiMessages, bufferLength, endOfBufferInSamples);
    }
    else
    {
        mMidiScheduler.ClearAllData(midiMessages);
    }
}
