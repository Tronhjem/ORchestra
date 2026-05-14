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
#include "Colors.h"
#include "LookAndFeelConstants.h"
#include "Utility.h"

using namespace ORchestra;

class TextEditorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& editor) override
    {
        UNUSED(width);
        UNUSED(height);
        UNUSED(editor);

        g.fillAll(TextEditorBackgroundColor);
    }

    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar,
        int x, int y, int width, int height,
        bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
        bool isMouseOver, bool isMouseDown) override
    {
        UNUSED(scrollbar);
        UNUSED(isMouseOver);
        UNUSED(isMouseDown);
        UNUSED(thumbStartPosition);

        g.setColour(TextEditorBackgroundColor);
        g.fillRect(x, y, width, height);

        g.setColour(ButtonBackgroundColor);
        if (isScrollbarVertical)
            g.fillRoundedRectangle(static_cast<float>(x), static_cast<float>(thumbStartPosition), static_cast<float>(width), static_cast<float>(thumbSize), ROUNDED_CORNER_SIZE);
        else
            g.fillRoundedRectangle(static_cast<float>(thumbStartPosition), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), ROUNDED_CORNER_SIZE);
    }

    int getDefaultScrollbarWidth() override 
    {
        return 4;
    }

    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& ed) override
    {
        UNUSED(g);
        UNUSED(width);
        UNUSED(height);
        UNUSED(ed);
        
        g.setColour(ORchestra::ComponentOutlineColor);
        g.drawLine(0.f, 0.f, 0.f, (float)height, VERTICAL_SEPARATOR_THICKNESS);
    }
};
