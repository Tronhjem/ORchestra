#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Colours.h"

class TextEditorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& editor) override
    {
        g.fillAll (ORchestraColours::Background);
    }
    
    void drawScrollbar (juce::Graphics& g, juce::ScrollBar& scrollbar,
                       int x, int y, int width, int height,
                       bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                       bool isMouseOver, bool isMouseDown) override
    {
        // Background
        g.setColour(juce::Colours::lightgrey);
        g.fillRect(x, y, width, height);

        // Thumb (the draggable part)
        g.setColour(ORchestraColours::ButtonBackground); // Set your custom color here
        if (isScrollbarVertical)
            g.fillRoundedRectangle(x, thumbStartPosition, width, thumbSize, 20.4f);
        else
            g.fillRoundedRectangle(thumbStartPosition, y, thumbSize, height, 20.4f);
    }
    
    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& ed) override
      {
          g.setColour(ORchestraColours::ButtonBackground); // Or use your palette
          g.drawRect(0, 0, width, height, 2); // Last parameter is thickness
      }
};
