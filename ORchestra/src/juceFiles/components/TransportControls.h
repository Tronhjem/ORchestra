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

class TransportControls : public juce::Component,
                          public juce::Button::Listener
{
public:
    static constexpr int BUTTON_HEIGHT = 20;
    static constexpr int BUTTON_WIDTH = 50;

    TransportControls();
    ~TransportControls() override;

    void resized() override;

    void setPlayButtonCallback(std::function<void()> callback);
    void updatePlayButtonState(bool isPlaying);
    void setButtonLookAndFeel(juce::LookAndFeel* laf);

    constexpr int getPreferredWidth() const { return BUTTON_WIDTH; }
    constexpr int getPreferredHeight() const { return BUTTON_HEIGHT; }

private:
    void buttonClicked(juce::Button* button) override;

    juce::TextButton mPlayButton{ "Play" };
    std::function<void()> mPlayButtonCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControls)
};
