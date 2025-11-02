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
    char* GetLoadedFileData();
    std::string GetSavedFilePath();
    std::vector<LogEntry>& GetErrors();
    char* LoadFile(std::string& filePath);
    void SaveFile(std::string& data);
    std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE>& GetStepData() { return mStepRingBuffer; }
    int GetGlobalStepCount() { return mCurrentGlobalStep.load(); }
    void WorkerThreadLoop();
    
private:
    void PreProcessSteps();
    int64_t samplesSinceLastStep = 0;
    std::atomic<int> mReadySteps;
    std::atomic<int> mCurrentGlobalStep;
    std::atomic<int> mCurrentProcessingStep;
    std::atomic<bool> mIsVMInit { false };
    std::atomic<bool> shouldExit { false };
    
    std::thread workerThread;
    std::unique_ptr<VM> mVM;
    std::unique_ptr<FileLoader> mFileLoader;
    std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE> mStepRingBuffer;
    MidiScheduler mMidiScheduler;
};
