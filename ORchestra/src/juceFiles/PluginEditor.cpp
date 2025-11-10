/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "LookAndFeelConstants.h"
#include "Colours.h"
#include "TitleBarComponent.h"
#include "ErrorReporting.h"
#include "ParamConstants.h"
#include <ctime>

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 800;
constexpr int COMPONENT_MARGIN = 15;
constexpr int OUTER_MARGIN = 20;
const int buttonWidth = 50;
constexpr int buttonHeight = 20;
constexpr int codeEditorWidth = WINDOW_WIDTH - 2 * OUTER_MARGIN;
constexpr int codeEditorHeight = 300;

//==============================================================================
ORchestraAudioProcessorEditor::ORchestraAudioProcessorEditor (ORchestraAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), mEditorIsDirty(false)
{
    setSize (WINDOW_WIDTH, WINDOW_HEIGHT);
    
    audioProcessor.addChangeListener(this);
    juce::AudioProcessorValueTreeState& valueTree = audioProcessor.GetValueTree();
    
    mGeneralLookAndFeel = std::make_unique<GeneralLookAndFeel>();
    mButtonLookAndFeel = std::make_unique<ButtonLookAndFeel>();
    mTextEditorLookAndFeel = std::make_unique<TextEditorLookAndFeel>();
    
//    codeEditor.reset(new juce::CodeEditorComponent(codeDocument, &tokeniser));
//    codeEditor->setTabSize(4, true);
//    codeEditor->setLineNumbersShown(true);
    
    int buttonXStart = OUTER_MARGIN + buttonWidth * 2.2f + COMPONENT_MARGIN * 2.5f;
    int nextLineY = 20;
    
    mTempoDivLabel.setBounds(buttonXStart, nextLineY, buttonWidth * 2.f, buttonHeight);
    
    buttonXStart += buttonWidth * 1.8f + COMPONENT_MARGIN;
    mBpmLabel.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);
    
    buttonXStart += buttonWidth * 0.8f + COMPONENT_MARGIN;
    mNoteLengthLabel.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);
    
    // ======== NEW LINE ============
    nextLineY += buttonHeight;
    buttonXStart = OUTER_MARGIN;
    mTogglePlayButton.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);
    
    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    mCompileButton.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);

    buttonXStart += buttonWidth * 1.5f + COMPONENT_MARGIN;
    mTempoDivisionSelectorBox.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);

    buttonXStart += buttonWidth * 1.5f + COMPONENT_MARGIN;
    mBpmBox.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);
    
    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    mNoteLengtSelectorBox.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);
    
    buttonXStart += buttonWidth * 1.5f + COMPONENT_MARGIN;
    mExportToFileButton.setBounds(buttonXStart, nextLineY, buttonWidth * 1.2f, buttonHeight);

    buttonXStart += buttonWidth * 1.2f + COMPONENT_MARGIN;
    mImportFileButton.setBounds(buttonXStart, nextLineY, buttonWidth * 1.2f, buttonHeight);
    
    // ======== NEW LINE ============
    nextLineY += buttonHeight + COMPONENT_MARGIN;
    mCodeEditorTextBox.setBounds(OUTER_MARGIN, nextLineY, codeEditorWidth, codeEditorHeight);
    
    nextLineY += codeEditorHeight;
    mErrorTextBox.setBounds(OUTER_MARGIN, nextLineY, codeEditorWidth, 40);
    
    // ======== NEW LINE ============
    nextLineY += 40 + COMPONENT_MARGIN;
    timeline.setBounds(OUTER_MARGIN, nextLineY, 760, 260);
    
	mBpmBox.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	mBpmBox.setSliderSnapsToMousePosition(false);
    
	mBpmBox.setColour(Slider::backgroundColourId, ORchestraColours::ButtonBackground);
	mBpmBox.setColour(Slider::trackColourId, ORchestraColours::ButtonBackground);
	mBpmBox.setColour(Slider::thumbColourId, ORchestraColours::ButtonBackground);
	mBpmBox.setColour(Slider::textBoxBackgroundColourId, ORchestraColours::ButtonBackground);
	mBpmBox.setColour(Slider::textBoxHighlightColourId, ORchestraColours::ButtonBackground);
	mBpmBox.setColour(Slider::textBoxOutlineColourId, ORchestraColours::ButtonBackground);

    mTempoDivLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    mBpmLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    mNoteLengthLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    
    mTempoDivisionSelectorBox.addItemList(mNoteDivisions, 3);
    mNoteLengtSelectorBox.addItemList(mNoteDivisions, 3);

    mExportToFileButton.addListener(this);
    mImportFileButton.addListener(this);
    mCompileButton.addListener(this);
    mCodeEditorTextBox.addListener(this);
    mTogglePlayButton.addListener(this);
    
    juce::LookAndFeel::setDefaultLookAndFeel(mGeneralLookAndFeel.get());
    
    mTogglePlayButton.setLookAndFeel(mButtonLookAndFeel.get());
    mExportToFileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mImportFileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mCompileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mCodeEditorTextBox.setLookAndFeel(mTextEditorLookAndFeel.get());
    mErrorTextBox.setLookAndFeel(mTextEditorLookAndFeel.get());
    mTempoDivisionSelectorBox.setLookAndFeel(mGeneralLookAndFeel.get());
    mNoteLengtSelectorBox.setLookAndFeel(mGeneralLookAndFeel.get());
    mBpmBox.setLookAndFeel(mGeneralLookAndFeel.get());

    mCodeEditorTextBox.setReturnKeyStartsNewLine(true);
    mCodeEditorTextBox.setMultiLine(true);
    mCodeEditorTextBox.setScrollbarsShown(true);
    mCodeEditorTextBox.setCaretVisible(true);
    mCodeEditorTextBox.setFont(MONOSPACE_FONT_OPTIONS);
    mCodeEditorTextBox.setColour(juce::TextEditor::textColourId, ORchestraColours::TextColor);
    
    mErrorTextBox.setFont(MONOSPACE_FONT_OPTIONS);
    mErrorTextBox.setColour(juce::TextEditor::textColourId, ORchestraColours::TextColor);
    mErrorTextBox.setMultiLine(true);
    
    timeline.SetProcessor(&audioProcessor);
//    codeEditor.setPopupMenuEnabled(true);
    
    addAndMakeVisible(mTempoDivLabel);
    addAndMakeVisible(mBpmLabel);
    addAndMakeVisible(mNoteLengthLabel);
    addAndMakeVisible(mTogglePlayButton);
    addAndMakeVisible(mExportToFileButton);
    addAndMakeVisible(mImportFileButton);
    addAndMakeVisible(mCompileButton);
    addAndMakeVisible(mTempoDivisionSelectorBox);
    addAndMakeVisible(mNoteLengtSelectorBox);
    addAndMakeVisible(mCodeEditorTextBox);
    addAndMakeVisible(mErrorTextBox);
    addAndMakeVisible(timeline);
    addAndMakeVisible(mBpmBox);
    
    const std::string& data = audioProcessor.GetInstructionData();
    juce::String dataAsString {data};
    mCodeEditorTextBox.setText(dataAsString);
    
    mBpmSliderAttachment.reset (new SliderAttachment(valueTree, bpmString, mBpmBox));
    mTempoDivisionAttachment.reset (new ComboBoxAttachment(valueTree, tempoDivisionString, mTempoDivisionSelectorBox));
    mNoteLengthAttachment.reset (new ComboBoxAttachment(valueTree, noteLengthString, mNoteLengtSelectorBox));
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    mTogglePlayButton.setLookAndFeel(nullptr);
    mExportToFileButton.setLookAndFeel(nullptr);
    mImportFileButton.setLookAndFeel(nullptr);
    mCodeEditorTextBox.setLookAndFeel(nullptr);
    
    audioProcessor.removeChangeListener(this);
}

void ORchestraAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* broadCaster)
{
    const std::string& data = audioProcessor.GetInstructionData();
    const juce::String dataAsString {data};
    mCodeEditorTextBox.setText(dataAsString);
}

void ORchestraAudioProcessorEditor::textEditorTextChanged(juce::TextEditor& editor)
{
    mEditorIsDirty = true;
}

void ORchestraAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if(button == &mTogglePlayButton)
    {
        audioProcessor.IsRunning = !audioProcessor.IsRunning;
        mTogglePlayButton.setButtonText(audioProcessor.IsRunning ? "Stop" : "Play");
    }
    else if(button == &mExportToFileButton)
    {
        //TODO: Should we compile before saving?
//        juce::String text = mCodeEditorTextBox.getText();
//        std::string utf8Text = text.toRawUTF8();
//        audioProcessor.Compile(utf8Text);
        
        mFileChooser.launchAsync(mFileChooserFlags, [this] (const juce::FileChooser& fc)
        {
            juce::File file = mFileChooser.getResult();
            std::string filePath {file.getFullPathName().toRawUTF8()};
            audioProcessor.ExportToFile(filePath);
        });
        
        UpdateErrors();
    }
    else if(button == &mImportFileButton)
    {
        mFileChooser.launchAsync(mFileChooserFlags, [this] (const juce::FileChooser& fc)
        {
            juce::File file = mFileChooser.getResult();
            std::string filePath {file.getFullPathName().toRawUTF8()};
            const std::string& data = audioProcessor.ImportFromFile(filePath);
            juce::String dataAsString {data};
            mCodeEditorTextBox.setText(dataAsString);
            
            UpdateErrors();
        });
    }
    else if (button == &mCompileButton)
    {
        juce::String text = mCodeEditorTextBox.getText();
        std::string utf8Text = text.toRawUTF8();
        audioProcessor.Compile(utf8Text);
        mEditorIsDirty = false;
        
        UpdateErrors();
    }
}

void ORchestraAudioProcessorEditor::UpdateErrors()
{
    const std::vector<LogEntry>& errors = audioProcessor.GetErrors();
    if(errors.size() > 0)
        mErrorTextBox.setText(errors[0].mMessage);
    else
        mErrorTextBox.setText("Compiled successfully!");
}

//==============================================================================
void ORchestraAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (ORchestraColours::Background);
    mCompileButton.setEnabled(mEditorIsDirty);
}

void ORchestraAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
