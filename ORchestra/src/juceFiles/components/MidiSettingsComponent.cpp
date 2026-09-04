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

#include "MidiSettingsComponent.h"
#include "Colors.h"

MidiSettingsComponent::MidiSettingsComponent(juce::AudioDeviceManager& deviceManager)
    : mDeviceManager(deviceManager)
{
    mOutputLabel.setText("MIDI Output", juce::dontSendNotification);
    mOutputLabel.setColour(juce::Label::textColourId, ORchestra::TextColor);
    addAndMakeVisible(mOutputLabel);

    mOutputCombo.addItem("None", 1);
    const auto currentOutputId = mDeviceManager.getDefaultMidiOutputIdentifier();
    int itemId = 2;
    int selectedId = 1;

    for (const auto& device : juce::MidiOutput::getAvailableDevices())
    {
        mOutputCombo.addItem(device.name, itemId);
        mOutputIds.add(device.identifier);
        if (device.identifier == currentOutputId)
            selectedId = itemId;
        ++itemId;
    }

    mOutputCombo.setSelectedId(selectedId, juce::dontSendNotification);
    mOutputCombo.onChange = [this]()
    {
        const int id = mOutputCombo.getSelectedId();
        if (id == 1)
            mDeviceManager.setDefaultMidiOutputDevice({});
        else if (id >= 2 && (id - 2) < mOutputIds.size())
            mDeviceManager.setDefaultMidiOutputDevice(mOutputIds[id - 2]);
    };

    addAndMakeVisible(mOutputCombo);

    mAudioDriverLabel.setText("Audio Driver", juce::dontSendNotification);
    mAudioDriverLabel.setColour(juce::Label::textColourId, ORchestra::TextColor);
    addAndMakeVisible(mAudioDriverLabel);

    const auto& deviceTypes = mDeviceManager.getAvailableDeviceTypes();
    const juce::String currentDeviceType = mDeviceManager.getCurrentAudioDeviceType();
    int driverItemId = 1;
    int driverSelectedId = 1;
    for (auto* type : deviceTypes)
    {
        mAudioDriverCombo.addItem(type->getTypeName(), driverItemId);
        if (type->getTypeName() == currentDeviceType)
            driverSelectedId = driverItemId;
        ++driverItemId;
    }
    mAudioDriverCombo.setSelectedId(driverSelectedId, juce::dontSendNotification);
    mAudioDriverCombo.onChange = [this]()
    {
        const int id = mAudioDriverCombo.getSelectedId();
        if (id < 1 || id > mAudioDriverCombo.getNumItems())
            return;

        const juce::String type = mAudioDriverCombo.getItemText(id - 1);
        mDeviceManager.setCurrentAudioDeviceType(type, true);
    };
    addAndMakeVisible(mAudioDriverCombo);

    auto audioSetup = mDeviceManager.getAudioDeviceSetup();
    audioSetup.inputDeviceName = "None";

    mFileLabel.setText("File", juce::dontSendNotification);
    mFileLabel.setColour(juce::Label::textColourId, ORchestra::TextColor);
    addAndMakeVisible(mFileLabel);

    mImportButton.setColour(juce::TextButton::textColourOffId, ORchestra::TextColor);
    mImportButton.onClick = [this]() { if (mImportCallback) mImportCallback(); };
    addAndMakeVisible(mImportButton);

    mExportButton.setColour(juce::TextButton::textColourOffId, ORchestra::TextColor);
    mExportButton.onClick = [this]() { if (mExportCallback) mExportCallback(); };
    addAndMakeVisible(mExportButton);

    mVersionLabel.setText(juce::String("v") + ProjectInfo::versionString,
                          juce::dontSendNotification);
    mVersionLabel.setColour(juce::Label::textColourId,
                            ORchestra::TextColor.withAlpha(0.5f));
    mVersionLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(mVersionLabel);

    const int height = PADDING * 2
                     + LABEL_H + ROW_GAP
                     + ROW_H
                     + SECTION_GAP
                     + LABEL_H + ROW_GAP
                     + ROW_H
                     + SECTION_GAP
                     + LABEL_H + ROW_GAP
                     + ROW_H
                     + SECTION_GAP
                     + LABEL_H;

    setSize(PANEL_W, height);
}

MidiSettingsComponent::~MidiSettingsComponent()
{
    mOutputCombo.setLookAndFeel(nullptr);
    mAudioDriverCombo.setLookAndFeel(nullptr);
    mImportButton.setLookAndFeel(nullptr);
    mExportButton.setLookAndFeel(nullptr);
}

void MidiSettingsComponent::resized()
{
    auto bounds = getLocalBounds().reduced(PADDING);

    mOutputLabel.setBounds(bounds.removeFromTop(LABEL_H));
    bounds.removeFromTop(ROW_GAP);
    mOutputCombo.setBounds(bounds.removeFromTop(ROW_H));

    bounds.removeFromTop(SECTION_GAP);
    mAudioDriverLabel.setBounds(bounds.removeFromTop(LABEL_H));
    bounds.removeFromTop(ROW_GAP);
    mAudioDriverCombo.setBounds(bounds.removeFromTop(ROW_H));

    bounds.removeFromTop(SECTION_GAP);
    mFileLabel.setBounds(bounds.removeFromTop(LABEL_H));
    bounds.removeFromTop(ROW_GAP);
    auto fileRow = bounds.removeFromTop(ROW_H);
    const int btnW = (fileRow.getWidth() - ROW_GAP) / 2;
    mImportButton.setBounds(fileRow.removeFromLeft(btnW));
    fileRow.removeFromLeft(ROW_GAP);
    mExportButton.setBounds(fileRow);

    bounds.removeFromTop(SECTION_GAP);
    mVersionLabel.setBounds(bounds.removeFromTop(LABEL_H));
}

void MidiSettingsComponent::setButtonLookAndFeel(juce::LookAndFeel* laf)
{
    mImportButton.setLookAndFeel(laf);
    mExportButton.setLookAndFeel(laf);
}

void MidiSettingsComponent::setImportCallback(std::function<void()> callback)
{
    mImportCallback = std::move(callback);
}

void MidiSettingsComponent::setExportCallback(std::function<void()> callback)
{
    mExportCallback = std::move(callback);
}
