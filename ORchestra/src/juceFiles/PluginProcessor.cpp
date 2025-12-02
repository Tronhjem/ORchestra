/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParamConstants.h"


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
    mShouldSync(false),
    mValueTree(*this, nullptr, juce::Identifier("ORchestra"),
        { std::make_unique<juce::AudioParameterInt>(bpmParamId, "Bpm", 10, 300, 120),
         std::make_unique<juce::AudioParameterChoice>(tempoDivisionId, "Tempo Division", mNoteDivisionsStrings, static_cast<int>(NoteDivision::n4)),
         std::make_unique<juce::AudioParameterChoice>(noteLengthId, "Note Length", mNoteDivisionsStrings, static_cast<int>(NoteDivision::n4)) })
{

    mORchestraEngine = std::make_unique<ORchestraEngine>();

    mTransportData.timeInSamples = 0;
    mTransportData.sampleRate = 44100;

    mBpm = mValueTree.getRawParameterValue(bpmString);
    mTempoDivision = mValueTree.getRawParameterValue(tempoDivisionString);
    mNoteLength = mValueTree.getRawParameterValue(noteLengthString);
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

    // We do this to only fill the position data if we ARE syncing
    // and only fill it when we're NOT syncing but RUNNING.
    if (!mShouldSync && IsRunning)
    {
        FillPositionData(mTransportData);
        mTransportData.timeInSamples = mLocalTimeInSamples;
        mTransportData.bpm = static_cast<double>(*mBpm);
        mTransportData.isPlaying = true;
    }
    else if (mShouldSync && !IsRunning)
    {
        FillPositionData(mTransportData);
    }

    mTransportData.bpmDivision = GetBpmDivision(*mTempoDivision);
    mTransportData.noteLengthInSamples = GetNoteLength(*mNoteLength);

    mORchestraEngine->Tick(mTransportData, bufferLength, midiMessages);

    // For incrementing sample position by the buffer and only when IsRunning
    if (!mShouldSync && IsRunning)
    {
        mLocalTimeInSamples += bufferLength; 
    }
    else if (!mShouldSync && !IsRunning)
    {
        mLocalTimeInSamples = 0;
        mTransportData.isPlaying = false;
        mTransportData.timeInSamples = mLocalTimeInSamples;
    }
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

float ORchestraAudioProcessor::GetBpmDivision(float noteDiv)
{
    const NoteDivision div = static_cast<NoteDivision>(noteDiv);

    switch (div)
    {
    case NoteDivision::n1:
        return 0.25f;
    case NoteDivision::n2:
        return 0.5f;
    case NoteDivision::n4:
        return 1.f;
    case NoteDivision::n8:
        return 2.f;
    case NoteDivision::n16:
        return 4.f;
    case NoteDivision::n32:
        return 8.f;
    case NoteDivision::n64:
        return 16.f;
    default:
        return 0.f;
    }
}

int ORchestraAudioProcessor::GetNoteLength(float noteDiv)
{
    return static_cast<int>(mSampleRate * (60.0 / static_cast<double>((*mBpm * GetBpmDivision(noteDiv)))));
}
