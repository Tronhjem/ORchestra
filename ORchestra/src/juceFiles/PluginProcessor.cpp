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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "juce_audio_processors/juce_audio_processors.h"

#if defined(_DEBUG)
    #include "ORchestraAssert.h"
#endif


using namespace ORchestra;
//==============================================================================
#if defined(_DEBUG)
namespace
{
    // The file lives at ~/Library/Application Support/ORchestra/ORchestra.log on macOS.
    juce::FileLogger* CreateLogFile()
    {
        const auto logDir = juce::File::getSpecialLocation(
            juce::File::userApplicationDataDirectory)
            .getChildFile("ORchestra");
        logDir.createDirectory();

        const auto logFile = logDir.getChildFile("ORchestra.log");
        if (logFile.exists())
            logFile.replaceWithText(""); // start each load with a fresh log

        juce::SystemStats::setApplicationCrashHandler([](void*) {
            juce::Logger::writeToLog("CRASH\n" + juce::SystemStats::getStackBacktrace());
        });

        return new juce::FileLogger(logFile, "ORchestra session\n", 0);
    }
}
#endif

ORchestraAudioProcessor::ORchestraAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if JucePlugin_Build_VST3 || JucePlugin_Build_Standalone
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
#if !JucePlugin_IsMidiEffect
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    IsRunning(false),
    mValueTree(*this, nullptr, juce::Identifier("ORchestra"), {})
{
#if defined(_DEBUG)
    mLogFileLogger.reset(CreateLogFile());
    juce::Logger::setCurrentLogger(mLogFileLogger.get());

    // Route ORCHESTRA_ASSERT failures (from AssertMutex and any other ORCHESTRA_ASSERT
    // use site) to the FileLogger so violations land in ORchestra.log with a backtrace
    // before the assert fires. Must be installed before any code that can assert.
    AssertSink::Set([](const std::string& condition, const std::string& message,
                       const std::string& backtrace) {
        juce::String line = "[ASSERT] " + juce::String(condition);
        if (!message.empty())
            line += " - " + juce::String(message);
        if (!backtrace.empty())
            line += "\n" + juce::String(backtrace);
        juce::Logger::writeToLog(line);
    });
#endif

    mORchestraEngine = std::make_unique<ORchestraEngine>();

#if defined(_DEBUG)
    mORchestraEngine->SetLogSink([](const LogEntry& entry) {
        juce::String prefix;
        switch (entry.mEntryType)
        {
            case EntryType::Error:   prefix = "[ERROR] ";   break;
            case EntryType::Warning: prefix = "[WARN] ";    break;
            default:                 prefix = "[INFO] ";    break;
        }
        juce::Logger::writeToLog(prefix + juce::String(entry.mMessage));
    });
#endif

    mTransportData.timeInSamples = 0;
    mTransportData.sampleRate = 44100;
    mTransportData.bpm = 120.0;
    mTransportData.beatDivision = 1.0f;

}

ORchestraAudioProcessor::~ORchestraAudioProcessor()
{
#if defined(_DEBUG)
    mORchestraEngine.reset();
    juce::Logger::setCurrentLogger(nullptr);
    mLogFileLogger.reset();
#endif
}

//==============================================================================
const juce::String ORchestraAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ORchestraAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ORchestraAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ORchestraAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ORchestraAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ORchestraAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int ORchestraAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ORchestraAudioProcessor::setCurrentProgram(int index)
{
    UNUSED(index);
}

const juce::String ORchestraAudioProcessor::getProgramName(int index)
{
    UNUSED(index);
    return {};
}

void ORchestraAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    UNUSED(index);
    UNUSED(newName);
}

//==============================================================================
void ORchestraAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mSampleRate = sampleRate;
    UNUSED(samplesPerBlock);
}

void ORchestraAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ORchestraAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if JucePlugin_Build_VST3
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void ORchestraAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int bufferLength = buffer.getNumSamples();
    buffer.clear();

    // Always fill host BPM and DAW position.
    FillPositionData(mTransportData);
    const bool dawIsPlaying = mTransportData.isPlaying;

    // DAW takes precedence. Only use the internal counter when the DAW is not playing
    // but the plugin play button is pressed (standalone testing mode).
    if (!dawIsPlaying && IsRunning)
    {
        mTransportData.timeInSamples = mLocalTimeInSamples;
        mTransportData.isPlaying = true;
    }

    // Making sure that count in, doesn't crash when time is negative.
    if (mTransportData.timeInSamples < 0)
        return;

    mORchestraEngine->Tick(mTransportData, bufferLength, midiMessages);

    // Publish UI-facing mirrors; Timeline/TriggerRectangle read these, not mTransportData.
    mUiBpmDivision.store(mTransportData.beatDivision, std::memory_order_release);
    mUiBpmFromScript.store(mTransportData.bpmFromScript, std::memory_order_release);

    // Advance the internal counter only when it is in use.
    // Reset it in all other cases so standalone mode always starts from 0.
    if (!dawIsPlaying && IsRunning)
        mLocalTimeInSamples += bufferLength;
    else
        mLocalTimeInSamples = 0;
}

void ORchestraAudioProcessor::FillPositionData(TransportData& data)
{
    auto* playHead = getPlayHead();
    if (playHead == nullptr)
        return;

    const auto positionInfo = playHead->getPosition();
    if (!positionInfo.hasValue())
        return;

    if (positionInfo->getBpm().hasValue())
    {
        data.bpm = static_cast<double>(*positionInfo->getBpm());
    }

    if (positionInfo->getTimeInSamples().hasValue())
    {
        data.timeInSamples = static_cast<int64_t>(*positionInfo->getTimeInSamples());
    }

    data.isPlaying = static_cast<bool>(positionInfo->getIsPlaying());
    data.sampleRate = mSampleRate;
}

//==============================================================================
bool ORchestraAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ORchestraAudioProcessor::createEditor()
{
    return new ORchestraAudioProcessorEditor(*this);
}

//==============================================================================
void ORchestraAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = mValueTree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->setAttribute("data", mORchestraEngine->GetInstructionData());
    xml->setAttribute("editorWidth", mEditorWidth);
    xml->setAttribute("editorHeight", mEditorHeight);
    xml->setAttribute("codePanelWidth", mCodePanelWidth);
    xml->setAttribute("consoleHeight", mConsoleHeight);
    copyXmlToBinary(*xml, destData);
}

void ORchestraAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(mValueTree.state.getType()))
        {
            mValueTree.replaceState(juce::ValueTree::fromXml(*xmlState));
        }

        const juce::String pluginData = xmlState->getStringAttribute("data", "");
        if (pluginData.length() > 0)
        {
            const std::string convertedData = pluginData.toStdString();
            SetInstructionData(convertedData);
            Compile(convertedData);
        }

        mEditorWidth      = xmlState->getIntAttribute("editorWidth", 0);
        mEditorHeight     = xmlState->getIntAttribute("editorHeight", 0);
        mCodePanelWidth   = xmlState->getIntAttribute("codePanelWidth", 0);
        mConsoleHeight    = xmlState->getIntAttribute("consoleHeight", 0);
        sendChangeMessage();
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ORchestraAudioProcessor();
}


