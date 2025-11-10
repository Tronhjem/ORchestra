#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "LookAndFeelConstants.h"
#include "Colours.h"

class ButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();

        juce::Colour fillColour = ORchestraColours::ButtonBackground;

        if (isButtonDown)
            fillColour = fillColour.darker(0.15f);
        else if (isMouseOverButton)
            fillColour = fillColour.brighter(0.15f);

        // No rounded corners: cornerSize = 0
        g.setColour(fillColour);
        g.fillRoundedRectangle(bounds, 6.f);

        // Optional: draw border
        g.setColour(fillColour.darker(0.3f));
        g.drawRoundedRectangle(bounds, 6.0f, 2.f);
    }
    
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override
    {
        return mFont;
    }

private:
    const Font mFont {MONOSPACE_FONT_OPTIONS};
};
