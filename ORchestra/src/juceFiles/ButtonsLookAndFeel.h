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
        UNUSED(backgroundColour);
        auto bounds = button.getLocalBounds().toFloat();

        juce::Colour fillColour = ButtonBackgroundColor;

        if (isButtonDown)
            fillColour = fillColour.darker(0.15f);
        else if (isMouseOverButton)
            fillColour = fillColour.brighter(0.15f);

        g.setColour(fillColour);
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
