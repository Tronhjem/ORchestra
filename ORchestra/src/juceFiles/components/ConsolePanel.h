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

class ConsolePanel : public juce::Component
{
public:
    static constexpr int HEADER_HEIGHT = 30;
    static constexpr int LOG_HEIGHT    = 200;

    ConsolePanel();
    ~ConsolePanel() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void setText(const juce::String& text);
    void setMessageCount(int count);
    void setClearCallback(std::function<void()> callback);
    void setButtonLookAndFeel(juce::LookAndFeel* laf);
    void setTextEditorLookAndFeel(juce::LookAndFeel* laf);
    void applyDefaultStyling();
    
    static constexpr int GetPreferredHeight() { return HEADER_HEIGHT + LOG_HEIGHT; }

private:
    static constexpr int CLEAR_BTN_W = 44;
    static constexpr int CLEAR_BTN_H = 15;

    juce::TextEditor mLogBox;
    juce::TextButton mClearButton{ "Clear" };
    int mMessageCount = 0;
    std::function<void()> mClearCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConsolePanel)
};
