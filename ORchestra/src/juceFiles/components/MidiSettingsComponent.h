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
#include <functional>

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

    juce::Label mOutputLabel;
    juce::ComboBox mOutputCombo;
    juce::StringArray mOutputIds;

    juce::Label mAudioDriverLabel;
    juce::ComboBox mAudioDriverCombo;
    juce::StringArray mAudioDeviceNames;

    juce::Label mFileLabel;
    juce::TextButton mImportButton{ "Import" };
    juce::TextButton mExportButton{ "Export" };
    std::function<void()> mImportCallback;
    std::function<void()> mExportCallback;

    juce::Label mVersionLabel;
};
