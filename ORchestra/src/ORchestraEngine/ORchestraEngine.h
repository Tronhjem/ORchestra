#pragma once

#include <JuceHeader.h>
#include <memory>
#include <ctime>

#include "TransportData.h"
#include "MidiScheduler.h"
#include "VM.h"
#include "FileLoader.h"
#include "StepData.h"
#include "ErrorReporting.h"

namespace ORchestra {


    constexpr int STEP_BUFFER_SIZE = 32;
    constexpr int HALF_STEP_BUFFER_SIZE = STEP_BUFFER_SIZE / 2;

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
        void WorkerThreadLoop();
        void PreProcessSteps();
        inline void Initialize();

        int mLastStep = -1;
        int64_t mSamplesSinceLastStep = 0;
        std::atomic<int> mReadySteps;
        std::atomic<int> mCurrentGlobalStep;
        std::atomic<int> mCurrentProcessingStep;
        std::atomic<bool> mIsVMInit;
        std::atomic<bool> mShouldExit;

        std::thread mWorkerThread;
        std::unique_ptr<VM> mVM;
        std::unique_ptr<FileLoader> mFileLoader;
        std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE> mStepRingBuffer;

        std::string mInstructionData;

        MidiScheduler mMidiScheduler;
        
        TransportData mScriptTransportData;
    };


} // namespace ORchestra
