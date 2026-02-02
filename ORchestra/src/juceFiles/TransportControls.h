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

typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class TransportControls : public juce::Component,
                          public juce::Button::Listener
{
public:
    TransportControls(juce::AudioProcessorValueTreeState& valueTree);
    ~TransportControls() override;

    void resized() override;

    // Callback setters
    void setPlayButtonCallback(std::function<void()> callback);
    void setSyncToggleCallback(std::function<void(bool)> callback);

    // State management
    void updatePlayButtonState(bool isPlaying);
    void setPlayButtonEnabled(bool enabled);
    void setBpmControlsEnabled(bool enabled);

    // Styling
    void setButtonLookAndFeel(juce::LookAndFeel* laf);

private:
    void buttonClicked(juce::Button* button) override;

    juce::TextButton mPlayButton{ "Play" };
    juce::ToggleButton mSyncToggleBox;
    juce::Label mSyncToggleLabel{ "sync", "Sync" };

    std::unique_ptr<ButtonAttachment> mSyncToggleAttachment;
    std::function<void()> mPlayButtonCallback;
    std::function<void(bool)> mSyncToggleCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControls)
};
