#pragma once

#include <JuceHeader.h>
#include <memory>

#include "TransportData.h"
#include "MidiScheduler.h"
#include "VM.h"
#include "FileLoader.h"
#include "StepData.h"
#include "ErrorReporting.h"

constexpr int STEP_BUFFER_SIZE = 32;
constexpr int HALF_STEP_BUFFER_SIZE = STEP_BUFFER_SIZE / 2;

class ORchestraEngine
{
public:
    ORchestraEngine();
    ~ORchestraEngine();
    void Tick(const TransportData& transportData, const int bufferLength, juce::MidiBuffer& midiMessages);
    const std::string& GetSavedFilePath();
    const std::vector<LogEntry>& GetErrors();
    const std::string& LoadFile(const std::string& filePath);
    void SaveToFile(const std::string& filePath);
    void Compile(const std::string& data);
    std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE>& GetStepData() { return mStepRingBuffer; }
    int GetGlobalStepCount() { return mCurrentGlobalStep.load(); }
    
    const std::string& GetInstructionData() { return mInstructionData; }
    void SetInstructionData(const std::string& data) { mInstructionData = data; }
    
    void WorkerThreadLoop();
    
private:
    void PreProcessSteps();
    inline void Initialize();
    std::string mInstructionData;
    
    int64_t mSamplesSinceLastStep = 0;
    std::atomic<int> mReadySteps;
    std::atomic<int> mCurrentGlobalStep;
    std::atomic<int> mCurrentProcessingStep;
    std::atomic<bool> mIsVMInit;
    std::atomic<bool> shouldExit;
    
    std::thread mWorkerThread;
    std::unique_ptr<VM> mVM;
    std::unique_ptr<FileLoader> mFileLoader;
    std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE> mStepRingBuffer;
    
    MidiScheduler mMidiScheduler;
};
