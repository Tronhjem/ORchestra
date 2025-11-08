/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

#include "ORchestraEngine.h"
#include "StepData.h"
#include "ErrorReporting.h"

const StringRef bpmString {"bpm"};
const StringRef noteDivisionString {"note-division"};
const juce::ParameterID bpmParamId {bpmString, 1};
const juce::ParameterID noteDivisionId {noteDivisionString, 1};

enum class NoteDivision
{
    n1,
    n2,
    n4,
    n8,
    n16,
    n32,
    n64
};

class ORchestraAudioProcessor  : public juce::AudioProcessor, public juce::ChangeBroadcaster
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    ORchestraAudioProcessor();
    ~ORchestraAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState& GetValueTree() { return mValueTree; };
    
    const TransportData& GetTransportData() { return mTransportData; };
    char* GetFileText();
    std::vector<LogEntry>& GetErrors();
    std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE>& GetStepData() { return mORchestraEngine->GetStepData(); }
    int GetGlobalStepCount() { return mORchestraEngine->GetGlobalStepCount(); }
    
    char* LoadFile(std::string& filePath);
    void SaveFile(std::string& data);
    void Compile(std::string& data);
    
//    void SetTempoDivision(NoteDivision division) { mTempoDivision = division; }
    void SetNoteLength(NoteDivision division) { mNoteLength = division; }
    void SetBpm(double bpm) { }
    
    bool IsRunning = false;
    double mSampleRate = 44100.0;

private:
    void FillPositionData(TransportData& data);
    TransportData mTransportData;
    std::unique_ptr<ORchestraEngine> mORchestraEngine;
    inline float GetBpmDivision(NoteDivision noteDiv);
    inline int GetNoteLength(NoteDivision noteDiv);

    std::atomic<float>* mBpm;
    std::atomic<float>* mNoteDivision;
//    NoteDivision mTempoDivision;
    NoteDivision mNoteLength;
    
    juce::String mSavedFilePath {""};
    juce::StringArray mNoteDivisions{ "1n", "2n", "4n", "8n", "16n", "32n", "64n"};
    juce::AudioProcessorValueTreeState mValueTree;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ORchestraAudioProcessor)
};
