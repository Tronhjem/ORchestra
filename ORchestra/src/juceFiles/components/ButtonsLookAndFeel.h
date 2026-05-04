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
#include <juce_gui_basics/juce_gui_basics.h>
#include "LookAndFeelConstants.h"
#include "Colors.h"
#include "Utility.h"
#include "juce_graphics/juce_graphics.h"

using namespace ORchestra;

class ButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();

        const juce::Colour baseColour = backgroundColour.isTransparent() ? ButtonBackgroundColor : backgroundColour;
        juce::Colour fillColour = button.getToggleState() ? HighlightColor : baseColour;

        if (isButtonDown)
            fillColour = fillColour.darker(0.15f);
        else if (isMouseOverButton)
            fillColour = fillColour.brighter(0.2f);

        g.setColour(fillColour);

        // Square bounds -> draw as circle (used for traffic-light dots)
        if (std::abs(bounds.getWidth() - bounds.getHeight()) < 1.f)
            g.fillEllipse(bounds);
        else
            g.fillRoundedRectangle(bounds, ROUNDED_CORNER_SIZE);
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& toggleButton,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        UNUSED(shouldDrawButtonAsHighlighted);
        UNUSED(shouldDrawButtonAsDown);

        juce::Colour fillColour = ButtonBackgroundColor;
        g.setColour(fillColour);

        g.fillRoundedRectangle(toggleButton.getLocalBounds().toFloat(), ROUNDED_CORNER_SIZE);

        const bool isPressed = toggleButton.getToggleState();
        if (isPressed)
        {
            g.setColour(juce::Colours::white);

            Rectangle<float> bounds = toggleButton.getLocalBounds().toFloat() * 0.6f;
            bounds.setCentre(toggleButton.getLocalBounds().toFloat().getCentre());

            g.fillEllipse(bounds);
        }
    }

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        UNUSED(buttonHeight);
        return mFont;
    }

private:
    const Font mFont{ MONOSPACE_FONT_OPTIONS };
};
