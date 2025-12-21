#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Colors.h"
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

        // Background
        g.setColour(TextEditorBackgroundColor);
        g.fillRect(x, y, width, height);

        g.setColour(ButtonBackgroundColor);
        if (isScrollbarVertical)
            g.fillRoundedRectangle(static_cast<float>(x), static_cast<float>(thumbStartPosition), static_cast<float>(width), static_cast<float>(thumbSize), ROUNDED_CORNER_SIZE);
        else
            g.fillRoundedRectangle(static_cast<float>(thumbStartPosition), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), ROUNDED_CORNER_SIZE);
    }

    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& ed) override
    {
        UNUSED(g);
        UNUSED(width);
        UNUSED(height);
        UNUSED(ed);
//        g.setColour(ButtonBackgroundColor);
//        g.drawRect(0, 0, width, height, 2);
    }
};
