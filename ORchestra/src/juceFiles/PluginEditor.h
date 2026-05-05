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

#include "PluginProcessor.h"

#include "Timeline.h"
#include "TriggerRectangle.h"
#include "CodeEditorPanel.h"

#include "GeneralLookAndFeel.h"
#include "ButtonsLookAndFeel.h"
#include "TextEditorLookAndFeel.h"
#include "ErrorReporting.h"
#include "FileOperationsToolbar.h"
#include "ConsolePanel.h"
#include "juce_gui_basics/juce_gui_basics.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

//==============================================================================
class MidiSettingsComponent : public juce::Component
{
public:
    explicit MidiSettingsComponent(juce::AudioDeviceManager& deviceManager);
    ~MidiSettingsComponent() override;
    void resized() override;
    void setButtonLookAndFeel(juce::LookAndFeel* laf);
    void setImportCallback(std::function<void()> callback);
    void setExportCallback(std::function<void()> callback);

private:
    static constexpr int PADDING     = 12;
    static constexpr int LABEL_H     = 18;
    static constexpr int ROW_H       = 26;
    static constexpr int ROW_GAP     = 4;
    static constexpr int SECTION_GAP = 12;
    static constexpr int PANEL_W     = 300;

    juce::AudioDeviceManager& mDeviceManager;

    juce::Label mInputLabel;
    juce::Label mOutputLabel;
    juce::OwnedArray<juce::TextButton> mInputButtons;
    juce::StringArray mInputIds;
    juce::ComboBox mOutputCombo;
    juce::StringArray mOutputIds;

    juce::Label mFileLabel;
    juce::TextButton mImportButton{ "Import" };
    juce::TextButton mExportButton{ "Export" };
    std::function<void()> mImportCallback;
    std::function<void()> mExportCallback;
};

//==============================================================================
/**
*/
class ORchestraAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::ChangeListener,
    public ORchestraCodeEditorChangeListener,
    public ORchestra::ErrorReportingListener,
    public juce::KeyListener,
    private juce::AsyncUpdater
{
public:
    ORchestraAudioProcessorEditor(ORchestraAudioProcessor&);
    ~ORchestraAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void CodeEditorHasChanged() override;

    // ErrorReportingListener - called from audio thread
    void OnLogUpdated() override;

private:
    // AsyncUpdater - called on message thread
    void handleAsyncUpdate() override;
    void changeListenerCallback(juce::ChangeBroadcaster* broadCaster) override;

    // KeyListener - fires for any key press in the window regardless of focus
    bool keyPressed(const juce::KeyPress& key, juce::Component* origin) override;
    using juce::Component::keyPressed;

    void handleCompile();
    void handleImportFile();
    void handleExportFile();
    void handlePlayButton();
    void handleClearLog();

    ORchestraAudioProcessor& audioProcessor;
    inline void UpdateErrors();

    std::unique_ptr<GeneralLookAndFeel> mGeneralLookAndFeel;
    std::unique_ptr<ButtonLookAndFeel> mButtonLookAndFeel;
    std::unique_ptr<TextEditorLookAndFeel> mTextEditorLookAndFeel;

    juce::FileChooser mFileChooser{ "Select a file to load...", juce::File{}, "*.txt" };
    int mFileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    CodeEditorPanel mCodeEditorPanel;
    FileOperationsToolbar mFileOperationsToolbar;
    TriggerRectangleComponent mTriggerRectangle;
    Timeline mTimeline;
    ConsolePanel mConsolePanel;

    juce::TextButton mSettingsButton{ "..." };
    juce::TextButton mCloseButton{ "X" };

    juce::ComponentDragger mDragger;
    bool mIsDragging = false;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ORchestraAudioProcessorEditor)
};
