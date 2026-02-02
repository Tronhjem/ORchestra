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

class FileOperationsToolbar : public juce::Component,
                              public juce::Button::Listener
{
public:
    static constexpr int BUTTON_HEIGHT = 20;
    static constexpr int BUTTON_WIDTH = 60;
    static constexpr int BUTTON_SPACING = 15;

    FileOperationsToolbar();
    ~FileOperationsToolbar() override;

    void resized() override;

    void setImportCallback(std::function<void()> callback);
    void setExportCallback(std::function<void()> callback);
    void setCompileCallback(std::function<void()> callback);

    void setCompileButtonEnabled(bool enabled);

    void setButtonLookAndFeel(juce::LookAndFeel* laf);

    constexpr int getPreferredWidth() const { return 3 * BUTTON_WIDTH + 3 * BUTTON_SPACING; }
    constexpr int getPreferredHeight() const { return BUTTON_HEIGHT; }

private:
    void buttonClicked(juce::Button* button) override;

    juce::TextButton mImportButton{ "Import" };
    juce::TextButton mExportButton{ "Export" };
    juce::TextButton mCompileButton{ "Compile" };

    std::function<void()> mImportCallback;
    std::function<void()> mExportCallback;
    std::function<void()> mCompileCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileOperationsToolbar)
};
