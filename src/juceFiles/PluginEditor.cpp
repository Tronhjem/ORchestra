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
    
    int buttonXStart = OUTER_MARGIN + buttonWidth * 3 + COMPONENT_MARGIN * 2.5f;
    int nextLineY = 20;
    
    mTempoDivLabel.setBounds(buttonXStart, nextLineY, buttonWidth * 2, buttonHeight);
    
    buttonXStart += buttonWidth * 1.8f + COMPONENT_MARGIN;
    mBpmLabel.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);
    
    buttonXStart += buttonWidth * 0.8f + COMPONENT_MARGIN;
    mNoteLengthLabel.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);
    
    // ======== NEW LINE ============
    nextLineY += buttonHeight;
    buttonXStart = OUTER_MARGIN;
    togglePlay.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);
    
    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    saveFile.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);
    
    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    loadFile.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);

    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    mNoteDivisonBox.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);

    buttonXStart += buttonWidth * 1.5f + COMPONENT_MARGIN;
    mBpmBox.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);
    
    buttonXStart += buttonWidth + COMPONENT_MARGIN;
    mNoteLengthSelector.setBounds(buttonXStart, nextLineY, buttonWidth * 1.5f, buttonHeight);
    
    // ======== NEW LINE ============
    nextLineY += buttonHeight + COMPONENT_MARGIN;
    codeEditor.setBounds(OUTER_MARGIN, nextLineY, codeEditorWidth, codeEditorHeight);
    
    errorBox.setBounds(OUTER_MARGIN + codeEditorWidth,
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

    saveFile.addListener(this);
    codeEditor.addListener(this);
    togglePlay.addListener(this);
    loadFile.addListener(this);
    mNoteDivisonBox.addListener(this);
    mNoteLengthSelector.addListener(this);
    mBpmBox.addListener(this);
    
    juce::LookAndFeel::setDefaultLookAndFeel(mGeneralLookAndFeel.get());
    
    togglePlay.setLookAndFeel(mButtonLookAndFeel.get());
    saveFile.setLookAndFeel(mButtonLookAndFeel.get());
    loadFile.setLookAndFeel(mButtonLookAndFeel.get());
    codeEditor.setLookAndFeel(mTextEditorLookAndFeel.get());
    errorBox.setLookAndFeel(mTextEditorLookAndFeel.get());
    mNoteDivisonBox.setLookAndFeel(mGeneralLookAndFeel.get());
    mNoteLengthSelector.setLookAndFeel(mGeneralLookAndFeel.get());
    mBpmBox.setLookAndFeel(mGeneralLookAndFeel.get());

    codeEditor.setReturnKeyStartsNewLine(true);
    codeEditor.setMultiLine(true);
    codeEditor.setScrollbarsShown(true);
    codeEditor.setCaretVisible(true);
    codeEditor.setFont(MONOSPACE_FONT_OPTIONS);
    codeEditor.setColour(juce::TextEditor::textColourId, ORchestraColours::TextColor);
    
    errorBox.setFont(MONOSPACE_FONT_OPTIONS);
    errorBox.setColour(juce::TextEditor::textColourId, ORchestraColours::TextColor);
    errorBox.setMultiLine(true);
    
    timeline.SetProcessor(&audioProcessor);
//    codeEditor.setPopupMenuEnabled(true);
    
    addAndMakeVisible(mTempoDivLabel);
    addAndMakeVisible(mBpmLabel);
    addAndMakeVisible(mNoteLengthLabel);
    addAndMakeVisible(togglePlay);
    addAndMakeVisible(saveFile);
    addAndMakeVisible(loadFile);
    addAndMakeVisible(mNoteDivisonBox);
    addAndMakeVisible(mNoteLengthSelector);
    addAndMakeVisible(codeEditor);
    addAndMakeVisible(errorBox);
    addAndMakeVisible(timeline);
    addAndMakeVisible(mBpmBox);
    
    char* data = audioProcessor.GetFileText();
    if(data != nullptr)
    {
        juce::String dataAsString {data};
        codeEditor.setText(dataAsString);
    }
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    togglePlay.setLookAndFeel(nullptr);
    saveFile.setLookAndFeel(nullptr);
    loadFile.setLookAndFeel(nullptr);
    codeEditor.setLookAndFeel(nullptr);
    
    audioProcessor.removeChangeListener(this);
}

void ORchestraAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* broadCaster)
{
    char* data = audioProcessor.GetFileText();
    if(data != nullptr)
    {
        juce::String dataAsString {data};
        codeEditor.setText(dataAsString);
    }
}

void ORchestraAudioProcessorEditor::textEditorTextChanged(juce::TextEditor& editor)
{
}

void ORchestraAudioProcessorEditor::buttonClicked(juce::Button* button)
{
//    std::string filePath {"/Users/christiantronhjem/dev/EuclidsCombiPlugin/data/myFile.e"};
    
    if(button == &togglePlay)
    {
        audioProcessor.IsRunning = !audioProcessor.IsRunning;
        togglePlay.setButtonText(audioProcessor.IsRunning ? "Stop" : "Play");
    }
    else if(button == &saveFile)
    {
        juce::String text = codeEditor.getText();
        std::string utf8Text = text.toRawUTF8();
        audioProcessor.SaveFile(utf8Text);
        
        std::vector<LogEntry>& errors = audioProcessor.GetErrors();
        if(errors.size() > 0)
            errorBox.setText(errors[0].mMessage);
        else
            errorBox.setText("");

    }
    else if(button == &loadFile)
    {
        chooser.launchAsync(folderChooserFlags, [this] (const juce::FileChooser& fc)
        {
            juce::File file = chooser.getResult();
            std::string filePath {file.getFullPathName().toRawUTF8()};
            char* data = audioProcessor.LoadFile(filePath);
            if(data != nullptr)
            {
                juce::String dataAsString {data};
                codeEditor.setText(dataAsString);
            }
            
            std::vector<LogEntry>& errors = audioProcessor.GetErrors();
            if(errors.size() > 0)
                errorBox.setText(errors[0].mMessage);
            else
                errorBox.setText("");
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
