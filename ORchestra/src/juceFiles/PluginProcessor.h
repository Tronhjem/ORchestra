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

    // UI-facing mirrors published by the audio thread in processBlock. UI
    // components must read these instead of mTransportData, which the audio
    // thread mutates.
    float GetUiBpmDivision() const { return mUiBpmDivision.load(std::memory_order_acquire); }
    double GetUiBpmFromScript() const { return mUiBpmFromScript.load(std::memory_order_acquire); }

    std::string ImportFromFile(const std::string& filePath) { return mORchestraEngine->ImportFromFile(filePath); }
    void ExportToFile(const std::string& data) { mORchestraEngine->ExportToFile(data); }
    void Compile(const std::string& data) { mORchestraEngine->Compile(data); }
    void SetInstructionData(const std::string& data) { mORchestraEngine->SetInstructionData(data); }
    std::string GetInstructionData() { return mORchestraEngine->GetInstructionData(); }
    int GetGlobalStepCount() { return mORchestraEngine->GetGlobalStepCount(); }
    void CopyStepDataSlot(const size_t slotIndex, std::vector<SequenceStep>& out) { mORchestraEngine->CopyStepDataSlot(slotIndex, out); }

    std::vector<LogEntry> GetErrors() { return mORchestraEngine->GetErrors(); }
    bool IsORchestraVMInit() { return mORchestraEngine->IsVMInit(); }
    
    void SetErrorListener(ErrorReportingListener* listener) { mORchestraEngine->SetErrorListener(listener); }
#if defined(_DEBUG)
    void SetLogSink(LogSinkFn sink) { mORchestraEngine->SetLogSink(std::move(sink)); }
#endif
    void RequestClearErrors() { mORchestraEngine->RequestClearErrors(); }

    void SetEditorSize(int w, int h) { mEditorWidth = w; mEditorHeight = h; }
    int GetEditorWidth() const { return mEditorWidth; }
    int GetEditorHeight() const { return mEditorHeight; }

    void SetCodePanelWidth(int w) { mCodePanelWidth = w; }
    int GetCodePanelWidth() const { return mCodePanelWidth; }

    void SetConsoleHeight(int h) { mConsoleHeight = h; }
    int GetConsoleHeight() const { return mConsoleHeight; }

    bool IsRunning = false;
    double mSampleRate = 44100;

private:
    void FillPositionData(TransportData& data);
    TransportData mTransportData;
    // Written only on the audio thread (processBlock), read on the UI thread.
    std::atomic<float> mUiBpmDivision {1.0f};
    std::atomic<double> mUiBpmFromScript {0.0};
#if defined(_DEBUG)
    std::unique_ptr<juce::FileLogger> mLogFileLogger;
#endif
    std::unique_ptr<ORchestraEngine> mORchestraEngine;

    int mLocalTimeInSamples = 0;

    juce::String mSavedFilePath{ "" };
    juce::AudioProcessorValueTreeState mValueTree;

    int mEditorWidth = 0;
    int mEditorHeight = 0;
    int mCodePanelWidth = 0;
    int mConsoleHeight = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ORchestraAudioProcessor)
};
