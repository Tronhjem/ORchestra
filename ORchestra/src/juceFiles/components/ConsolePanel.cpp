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

#include "ConsolePanel.h"
#include "Colors.h"
#include "LookAndFeelConstants.h"

using namespace ORchestra;

ConsolePanel::ConsolePanel()
{
    mClearButton.onClick = [this]() { if (mClearCallback) mClearCallback(); };
    addAndMakeVisible(mLogBox);
    addAndMakeVisible(mClearButton);
}

ConsolePanel::~ConsolePanel()
{
    mLogBox.setLookAndFeel(nullptr);
    mClearButton.setLookAndFeel(nullptr);
}

void ConsolePanel::resized()
{
    auto bounds = getLocalBounds();
    auto header = bounds.removeFromTop(HEADER_HEIGHT);

    mClearButton.setBounds(header.getRight() - CLEAR_BTN_W - 10,
                           header.getY() + (HEADER_HEIGHT - CLEAR_BTN_H) / 2,
                           CLEAR_BTN_W, CLEAR_BTN_H);

    mLogBox.setBounds(bounds);
}

void ConsolePanel::paint(juce::Graphics& g)
{
    // Log area background
    g.setColour(ConsoleBackgroundColor);
    g.fillRect(0, HEADER_HEIGHT, getWidth(), getHeight() - HEADER_HEIGHT);

    // Header background (Mantle - same as title bar)
    g.setColour(juce::Colour(ColorPalette::Mantle));
    g.fillRect(0, 0, getWidth(), HEADER_HEIGHT);

    // Header/log divider
    g.setColour(ComponentOutlineColor);
    g.drawLine(0.f, float(HEADER_HEIGHT), float(getWidth()), float(HEADER_HEIGHT), 1.f);
    
    // Side of full panel divider
    g.drawLine(0.f, 0.f, 0.f, float(getHeight()), VERTICAL_SEPARATOR_THICKNESS);
    // Top
    g.drawLine(0.f, 0.f, (float)getWidth(), 0.f, OUTLINE_THICKNESS);

    // "CONSOLE" label
    g.setFont(juce::Font(juce::FontOptions{ MONOSPACE_FONT_OPTIONS }.withHeight(12.f)));
    g.setColour(juce::Colour(ColorPalette::Subtext1));
    g.drawText("CONSOLE", 8, 0, 80, HEADER_HEIGHT, juce::Justification::centredLeft, false);

    // Message count
    const juce::String countText = juce::String(mMessageCount) + " message" + (mMessageCount == 1 ? "" : "s");
    g.drawText(countText,
               0, 0, getWidth() - CLEAR_BTN_W - 24, HEADER_HEIGHT,
               juce::Justification::centredRight, false);
    

}

void ConsolePanel::setText(const juce::String& text)
{
    mLogBox.setText(text);
}

void ConsolePanel::setMessageCount(int count)
{
    mMessageCount = count;
    repaint(0, 0, getWidth(), HEADER_HEIGHT);
}

void ConsolePanel::setClearCallback(std::function<void()> callback)
{
    mClearCallback = std::move(callback);
}

void ConsolePanel::setButtonLookAndFeel(juce::LookAndFeel* laf)
{
    mClearButton.setLookAndFeel(laf);
}

void ConsolePanel::setTextEditorLookAndFeel(juce::LookAndFeel* laf)
{
    mLogBox.setLookAndFeel(laf);
}

void ConsolePanel::applyDefaultStyling()
{
    mLogBox.setFont(MONOSPACE_FONT_OPTIONS);
    mLogBox.setColour(juce::TextEditor::textColourId, TextColor);
    mLogBox.setColour(juce::TextEditor::backgroundColourId, ConsoleBackgroundColor);
    mLogBox.setMultiLine(true);
    mLogBox.setEnabled(false);
}
