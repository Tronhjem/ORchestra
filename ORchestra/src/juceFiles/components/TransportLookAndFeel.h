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

using namespace ORchestra;

class TransportLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();

        const juce::Colour baseColour = backgroundColour.isTransparent() ? ButtonBackgroundColor : backgroundColour;
        juce::Colour outlineColour = button.getToggleState() ? HighlightColor : baseColour;
        juce::Colour fillColour = TransportButtonPanelBackground.interpolatedWith(outlineColour, 0.1f);

        if (isButtonDown)
        {
            fillColour = fillColour.brighter(0.1f);
            outlineColour = outlineColour.brighter(0.1f);
        }
        else if (isMouseOverButton)
        {
            fillColour = fillColour.brighter(0.05f);
            outlineColour = outlineColour.brighter(0.15f);
        }

        g.setColour(fillColour);
        g.fillRoundedRectangle(bounds, ROUNDED_CORNER_SIZE);
        g.setColour(outlineColour);
        g.drawRoundedRectangle(bounds.reduced(OUTLINE_THICKNESS * 0.5f), ROUNDED_CORNER_SIZE, OUTLINE_THICKNESS);
    }

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        UNUSED(buttonHeight);
        return mFont;
    }

private:
    const juce::Font mFont{ BUTTON_FONT_OPTIONS };
};
