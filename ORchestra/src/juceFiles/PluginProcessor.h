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

#include "ORchestraEngine.h"
#include "ErrorReporting.h"
#include "Defines.h"

using namespace ORchestra;
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

class ORchestraAudioProcessor : public juce::AudioProcessor, public juce::ChangeBroadcaster
#if JucePlugin_Enable_ARA
  ,
  public juce::AudioProcessorARAExtension
#endif
{
public:
    ORchestraAudioProcessor();
    ~ORchestraAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& GetValueTree() { return mValueTree; }
    const TransportData& GetTransportData() { return mTransportData; }

    const std::string& ImportFromFile(const std::string& filePath) { return mORchestraEngine->ImportFromFile(filePath); }
    void ExportToFile(const std::string& data) { mORchestraEngine->ExportToFile(data); }
    void Compile(const std::string& data) { mORchestraEngine->Compile(data); }
    void SetInstructionData(const std::string& data) { mORchestraEngine->SetInstructionData(data); }
    const std::string& GetInstructionData() { return mORchestraEngine->GetInstructionData(); }
    int GetGlobalStepCount() { return mORchestraEngine->GetGlobalStepCount(); }
    std::array<std::vector<SequenceStep>, STEP_BUFFER_SIZE>& GetStepData() { return mORchestraEngine->GetStepData(); }
    const std::vector<LogEntry>& GetErrors() { return mORchestraEngine->GetErrors(); }
    bool IsORchestraVMInit() { return mORchestraEngine->IsVMInit(); }

    bool IsRunning = false;
    double mSampleRate = 44100;

private:
    void FillPositionData(TransportData& data);
    TransportData mTransportData;
    std::unique_ptr<ORchestraEngine> mORchestraEngine;
    inline float GetBpmDivision(float noteDiv);
    inline int GetNoteLength(float noteDiv);

    int mLocalTimeInSamples = 0;
    std::atomic<float>* mBpm;
    std::atomic<float>* mTempoDivision;
    std::atomic<float>* mNoteLength;
    std::atomic<float>* mShouldSync;

    juce::String mSavedFilePath{ "" };
    juce::StringArray mNoteDivisionsStrings{ "1n", "2n", "4n", "8n", "16n", "32n", "64n" };
    juce::AudioProcessorValueTreeState mValueTree;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ORchestraAudioProcessor)
};
