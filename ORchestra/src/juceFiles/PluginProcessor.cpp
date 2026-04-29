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
#include "ParamConstants.h"
#include "juce_audio_processors/juce_audio_processors.h"


using namespace ORchestra;
//==============================================================================
ORchestraAudioProcessor::ORchestraAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    IsRunning(false),
    mValueTree(*this, nullptr, juce::Identifier("ORchestra"), {})
{

    mORchestraEngine = std::make_unique<ORchestraEngine>();

    mTransportData.timeInSamples = 0;
    mTransportData.sampleRate = 44100;
    mTransportData.bpm = 120.0;
    mTransportData.bpmDivision = 1.0f;

}

ORchestraAudioProcessor::~ORchestraAudioProcessor()
{
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
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
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

    // If the play button is active, use the internal counter for position.
    if (IsRunning)
    {
        mTransportData.timeInSamples = mLocalTimeInSamples;
        mTransportData.isPlaying = true;
    }

    // Making sure that count in, doesn't crash when time is negative.
    if (mTransportData.timeInSamples < 0)
        return;

    mORchestraEngine->Tick(mTransportData, bufferLength, midiMessages);

    if (IsRunning)
        mLocalTimeInSamples += bufferLength;
    else
        mLocalTimeInSamples = 0;
}

void ORchestraAudioProcessor::FillPositionData(TransportData& data)
{
    const auto positionInfo = getPlayHead()->getPosition();

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
            sendChangeMessage();
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ORchestraAudioProcessor();
}


