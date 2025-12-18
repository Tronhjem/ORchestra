#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Colours.h"
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

        g.fillAll(BackgroundColor);
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
        g.setColour(juce::Colours::lightgrey);
        g.fillRect(x, y, width, height);

        // Thumb (the draggable part)
        g.setColour(ButtonBackgroundColor); // Set your custom color here
        if (isScrollbarVertical)
            g.fillRoundedRectangle(static_cast<float>(x), static_cast<float>(thumbStartPosition), static_cast<float>(width), static_cast<float>(thumbSize), 20.4f);
        else
            g.fillRoundedRectangle(static_cast<float>(thumbStartPosition), static_cast<float>(y), static_cast<float>(thumbSize), static_cast<float>(height), 20.4f);
    }

    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& ed) override
    {
        UNUSED(ed);
        g.setColour(ButtonBackgroundColor); // Or use your palette
        g.drawRect(0, 0, width, height, 2);              // Last parameter is thickness
    }
};
