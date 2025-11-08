/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParamConstants.h"

//==============================================================================
ORchestraAudioProcessor::ORchestraAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    mValueTree(*this, nullptr, juce::Identifier("ORchestra"),
    {
        std::make_unique<juce::AudioParameterFloat>(bpmParamId, "Bpm", 0, 300, 120),
        std::make_unique<juce::AudioParameterChoice>(tempoDivisionId, "Tempo Division", mNoteDivisionsStrings, static_cast<int>(NoteDivision::n4)),
        std::make_unique<juce::AudioParameterChoice>(noteLengthId, "Note Length", mNoteDivisionsStrings, static_cast<int>(NoteDivision::n4))
    })
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

char* ORchestraAudioProcessor::GetFileText()
{
    return mORchestraEngine->GetLoadedFileData();
}

char* ORchestraAudioProcessor::LoadFile(std::string& filePath)
{
    char* loadedFile = mORchestraEngine->LoadFile(filePath);
    if(loadedFile != nullptr)
    {
        return loadedFile;
    }
    
    return nullptr;
}

void ORchestraAudioProcessor::SaveFile(std::string& data)
{
    mORchestraEngine->SaveFile(data);
}

void ORchestraAudioProcessor::Compile(std::string &data)
{
    mORchestraEngine->Compile(data);
}

std::vector<LogEntry>& ORchestraAudioProcessor::GetErrors()
{
    return mORchestraEngine->GetErrors();
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ORchestraAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ORchestraAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ORchestraAudioProcessor::getProgramName (int index)
{
    return {};
}

void ORchestraAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ORchestraAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSampleRate = sampleRate;
}

void ORchestraAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ORchestraAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
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
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    int bufferLength = buffer.getNumSamples();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, bufferLength);
    
    if(IsRunning)
        mTransportData.isPlaying = IsRunning; // Set the playing to true when standalone
    
//    FillPositionData(mTransportData);
    
    mTransportData.bpmDivision = GetBpmDivision(*mTempoDivision);
    mTransportData.bpm = static_cast<double>(*mBpm);
    mTransportData.noteLengthInSamples = GetNoteLength(*mNoteLength);
    
    mORchestraEngine->Tick(mTransportData, bufferLength, midiMessages);
    
    if (IsRunning)
    {
        mTransportData.timeInSamples += bufferLength; // Need to increment the position in samples ourselves when standalone.
    }
    else if(!IsRunning && !mTransportData.isPlaying)
    {
        mTransportData.timeInSamples = 0;
    }
}

void ORchestraAudioProcessor::FillPositionData(TransportData& data)
{
    auto positionInfo = getPlayHead()->getPosition();

    if(positionInfo->getBpm().hasValue())
    {
        data.bpm = static_cast<double>(*positionInfo->getBpm());
    }

    if(positionInfo->getTimeInSamples().hasValue())
    {
        data.timeInSamples = static_cast<int64_t>(*positionInfo->getTimeInSamples());
    }

    data.isPlaying = static_cast<bool>(positionInfo->getIsPlaying());

    data.sampleRate = mSampleRate;

//    if(positionInfo->getTimeInSeconds().hasValue())
//    {
//        data.time = static_cast<double>(*positionInfo->getTimeInSeconds());
//    }

//    if(positionInfo->getPpqPosition().hasValue())
//    {
//        data.ppq = static_cast<double>(*positionInfo->getPpqPosition());
//    }

//    if(positionInfo->getTimeSignature().hasValue())
//    {
//        auto timeSig = *positionInfo->getTimeSignature();
//        int barLength = timeSig.numerator;
//
//        int beatCount = static_cast<int>(data.time / (60.0 / data.bpm));
//        data.beat =  beatCount % barLength;
//        data.bar = static_cast<int> (beatCount / barLength);
//    }
}

//==============================================================================
bool ORchestraAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ORchestraAudioProcessor::createEditor()
{
    return new ORchestraAudioProcessorEditor (*this);
}

//==============================================================================
void ORchestraAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::XmlElement xml("PluginState");
    xml.setAttribute("filePath", mORchestraEngine->GetSavedFilePath());
    copyXmlToBinary(xml, destData);
}

void ORchestraAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName("PluginState"))
    {
        juce::String filePath = xml->getStringAttribute("filePath", "");
        if(filePath.length() > 0)
        {
            std::string convertedPath = filePath.toStdString();
            LoadFile(convertedPath);
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
    return static_cast<int>(mSampleRate * (60.0 / (*mBpm * GetBpmDivision(noteDiv))));
}
