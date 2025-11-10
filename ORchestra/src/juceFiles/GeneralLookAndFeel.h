#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "LookAndFeelConstants.h"
#include "Colours.h"

class GeneralLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Typeface::Ptr getTypefaceForFont (const juce::Font&) override
    {
        return juce::Typeface::createSystemTypefaceFor(mFont);
    }
    
    void drawMenuBarBackground (juce::Graphics& g, int w, int h, bool, juce::MenuBarComponent&) override
    {
        g.fillAll (ORchestraColours::Background); // or your palette color
    }

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

    void drawComboBox(Graphics& g, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        ComboBox& box) override
    {
		auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
		Rectangle<int> boxBounds (0, 0, width, height);

		g.setColour (ORchestraColours::ButtonBackground);
		g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

		g.setColour (ORchestraColours::ButtonBackground);
		g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

		Rectangle<int> arrowZone (width - 30, 0, 20, height);
		Path path;
		path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
		path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
		path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

		g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
		g.strokePath (path, PathStrokeType (2.0f));
    }

    void drawPopupMenuBackground (Graphics& g, int width, int height) override
    {
        //auto bounds = button.getLocalBounds().toFloat();
        juce::Rectangle bounds{ (float)width, (float)height };

        juce::Colour fillColour = ORchestraColours::ButtonBackground;

        //if (isButtonDown)
        //    fillColour = fillColour.darker(0.15f);
        //if (isMouseOverButton)
        //    fillColour = fillColour.brighter(0.15f);

        // No rounded corners: cornerSize = 0
        g.setColour(fillColour);
        g.fillRoundedRectangle(bounds, 6.f);

        // Optional: draw border
        g.setColour(fillColour.darker(0.3f));
        g.drawRoundedRectangle(bounds, 6.0f, 2.f);
    }

	void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
		float sliderPos, float minSliderPos, float maxSliderPos,
		const juce::Slider::SliderStyle style, juce::Slider& slider) override
	{
		// Draw the track background
		g.setColour(ORchestraColours::ButtonBackground);
		g.fillRect(x, y, width, height);

		// (Optional) Draw the original slider on top, or customize further
		juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height,
			sliderPos, minSliderPos, maxSliderPos, style, slider);
	}
    
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override
    {
        return mFont;
    }
    
private:
    const Font mFont {MONOSPACE_FONT_OPTIONS};
};
