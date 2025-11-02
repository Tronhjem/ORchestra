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

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int COMPONENT_MARGIN = 15;
constexpr int OUTER_MARGIN = 20;
const int buttonWidth = 50;
constexpr int buttonHeight = 20;
constexpr int codeEditorWidth = 400;
constexpr int codeEditorHeight = 300;

//==============================================================================
ORchestraAudioProcessorEditor::ORchestraAudioProcessorEditor (ORchestraAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (WINDOW_WIDTH, WINDOW_HEIGHT);
    
    audioProcessor.addChangeListener(this);
    
    mGeneralLookAndFeel = std::make_unique<GeneralLookAndFeel>();
    mButtonLookAndFeel = std::make_unique<ButtonLookAndFeel>();
    mTextEditorLookAndFeel = std::make_unique<TextEditorLookAndFeel>();
    
//    codeEditor.reset(new juce::CodeEditorComponent(codeDocument, &tokeniser));
//    codeEditor->setTabSize(4, true);
//    codeEditor->setLineNumbersShown(true);
//    codeEditor->loadContent("print(\"Hello, world!\")");
    
    int buttonXStart = OUTER_MARGIN + buttonWidth * 4.8f + COMPONENT_MARGIN * 2.5f;
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
    mSaveFileButton.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);
    
    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    mLoadFileButton.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);
    
    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    mCompileButton.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);

    buttonXStart += buttonWidth * 1.5f + COMPONENT_MARGIN;
    mNoteDivisonBox.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);

    buttonXStart += buttonWidth * 1.5f + COMPONENT_MARGIN;
    mBpmBox.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);
    
    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    mNoteLengthSelector.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);
    
    // ======== NEW LINE ============
    nextLineY += buttonHeight + COMPONENT_MARGIN;
    mCodeEditorTextBox.setBounds(OUTER_MARGIN, nextLineY, codeEditorWidth, codeEditorHeight);
    
    mErrorTextBox.setBounds(OUTER_MARGIN + codeEditorWidth,
                       nextLineY, WINDOW_WIDTH - codeEditorWidth - OUTER_MARGIN - OUTER_MARGIN,
                       codeEditorHeight);
    
    // ======== NEW LINE ============
    nextLineY += codeEditorHeight + COMPONENT_MARGIN;
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
    
    mBpmBox.setRange(20.0, 300.0, 1.0);
    mBpmBox.setValue(120.0);

    mNoteDivisonBox.addItemList(mNoteDivisions, 3);
    mNoteDivisonBox.setSelectedItemIndex(3);
    
    mNoteLengthSelector.addItemList(mNoteDivisions, 3);
    mNoteLengthSelector.setSelectedItemIndex(3);

    mSaveFileButton.addListener(this);
    mLoadFileButton.addListener(this);
    mCompileButton.addListener(this);
    mCodeEditorTextBox.addListener(this);
    mTogglePlayButton.addListener(this);
    mNoteDivisonBox.addListener(this);
    mNoteLengthSelector.addListener(this);
    mBpmBox.addListener(this);
    
    juce::LookAndFeel::setDefaultLookAndFeel(mGeneralLookAndFeel.get());
    
    mTogglePlayButton.setLookAndFeel(mButtonLookAndFeel.get());
    mSaveFileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mLoadFileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mCompileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mCodeEditorTextBox.setLookAndFeel(mTextEditorLookAndFeel.get());
    mErrorTextBox.setLookAndFeel(mTextEditorLookAndFeel.get());
    mNoteDivisonBox.setLookAndFeel(mGeneralLookAndFeel.get());
    mNoteLengthSelector.setLookAndFeel(mGeneralLookAndFeel.get());
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
    addAndMakeVisible(mSaveFileButton);
    addAndMakeVisible(mLoadFileButton);
    addAndMakeVisible(mCompileButton);
    addAndMakeVisible(mNoteDivisonBox);
    addAndMakeVisible(mNoteLengthSelector);
    addAndMakeVisible(mCodeEditorTextBox);
    addAndMakeVisible(mErrorTextBox);
    addAndMakeVisible(timeline);
    addAndMakeVisible(mBpmBox);
    
    char* data = audioProcessor.GetFileText();
    if(data != nullptr)
    {
        juce::String dataAsString {data};
        mCodeEditorTextBox.setText(dataAsString);
    }
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    mTogglePlayButton.setLookAndFeel(nullptr);
    mSaveFileButton.setLookAndFeel(nullptr);
    mLoadFileButton.setLookAndFeel(nullptr);
    mCodeEditorTextBox.setLookAndFeel(nullptr);
    
    audioProcessor.removeChangeListener(this);
}

void ORchestraAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* broadCaster)
{
    char* data = audioProcessor.GetFileText();
    if(data != nullptr)
    {
        juce::String dataAsString {data};
        mCodeEditorTextBox.setText(dataAsString);
    }
}

void ORchestraAudioProcessorEditor::textEditorTextChanged(juce::TextEditor& editor)
{
}

void ORchestraAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if(button == &mTogglePlayButton)
    {
        audioProcessor.IsRunning = !audioProcessor.IsRunning;
        mTogglePlayButton.setButtonText(audioProcessor.IsRunning ? "Stop" : "Play");
    }
    else if(button == &mSaveFileButton)
    {
        juce::String text = mCodeEditorTextBox.getText();
        std::string utf8Text = text.toRawUTF8();
        audioProcessor.SaveFile(utf8Text);
        
        std::vector<LogEntry>& errors = audioProcessor.GetErrors();
        if(errors.size() > 0)
            mErrorTextBox.setText(errors[0].mMessage);
        else
            mErrorTextBox.setText("");

    }
    else if (button == &mCompileButton)
    {
        juce::String text = mCodeEditorTextBox.getText();
        std::string utf8Text = text.toRawUTF8();
        audioProcessor.Compile(utf8Text);
    }
    else if(button == &mLoadFileButton)
    {
        mFileChooser.launchAsync(mFileChooserFlags, [this] (const juce::FileChooser& fc)
        {
            juce::File file = mFileChooser.getResult();
            std::string filePath {file.getFullPathName().toRawUTF8()};
            char* data = audioProcessor.LoadFile(filePath);
            if(data != nullptr)
            {
                juce::String dataAsString {data};
                mCodeEditorTextBox.setText(dataAsString);
            }
            
            std::vector<LogEntry>& errors = audioProcessor.GetErrors();
            if(errors.size() > 0)
                mErrorTextBox.setText(errors[0].mMessage);
            else
                mErrorTextBox.setText("");
        });
    }
}
void ORchestraAudioProcessorEditor::comboBoxChanged(ComboBox* changedComboBox)
{
    int val = changedComboBox->getSelectedItemIndex();
    if(changedComboBox == &mNoteDivisonBox)
    {
        audioProcessor.SetTempoDivision(static_cast<NoteDivision>(val));
    }
    else if(changedComboBox == &mNoteLengthSelector)
    {
        audioProcessor.SetNoteLength(static_cast<NoteDivision>(val));
    }
}

void ORchestraAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.SetBpm(slider->getValue());
}

//==============================================================================
void ORchestraAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (ORchestraColours::Background);
}

void ORchestraAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
