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

class GeneralLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override
    {
        return juce::Typeface::createSystemTypefaceFor(mFont);
    }

    void drawMenuBarBackground(juce::Graphics& g, int w, int h, bool, juce::MenuBarComponent&) override
    {
        UNUSED(w);
        UNUSED(h);

        g.fillAll(BackgroundColor); // or your palette color
    }

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

        // No rounded corners: cornerSize = 0
        g.setColour(fillColour);
        g.fillRoundedRectangle(bounds, 6.f);

        // Optional: draw border
        // g.setColour(fillColour.darker(0.3f));
        // g.drawRoundedRectangle(bounds, 6.0f, 2.f);
    }


    void drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                                            const bool isSeparator, const bool isActive,
                                            const bool isHighlighted, const bool isTicked,
                                            const bool hasSubMenu, const String& text,
                                            const String& shortcutKeyText,
                                            const Drawable* icon, const Colour* const textColourToUse) override
    {
        if (isSeparator)
        {
            auto r  = area.reduced (5, 0);
            r.removeFromTop (roundToInt (((float) r.getHeight() * 0.5f) - 0.5f));

            g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.3f));
            g.fillRect (r.removeFromTop (1));
        }
        else
        {
            auto textColour = (textColourToUse == nullptr ? findColour (PopupMenu::textColourId)
                                                          : *textColourToUse);

            auto r  = area.reduced (1);

            if (isHighlighted && isActive)
            {
                g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
                g.fillRect (r);

                g.setColour (findColour (PopupMenu::highlightedTextColourId));
            }
            else
            {
                g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
            }

            r.reduce (jmin (5, area.getWidth() / 20), 0);

            auto font = getPopupMenuFont();

            auto maxFontHeight = (float) r.getHeight() / 1.3f;

            if (font.getHeight() > maxFontHeight)
                font.setHeight (maxFontHeight);

            g.setFont (font);

            auto iconArea = r.removeFromLeft (roundToInt (maxFontHeight)).toFloat();

            if (icon != nullptr)
            {
                icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
                r.removeFromLeft (roundToInt (maxFontHeight * 0.5f));
            }
            else if (isTicked)
            {
                auto tick = getTickShape (1.0f);
                g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
            }

            if (hasSubMenu)
            {
                auto arrowH = 0.6f * getPopupMenuFont().getAscent();

                auto x = static_cast<float> (r.removeFromRight ((int) arrowH).getX());
                auto halfH = static_cast<float> (r.getCentreY());

                Path path;
                path.startNewSubPath (x, halfH - arrowH * 0.5f);
                path.lineTo (x + arrowH * 0.6f, halfH);
                path.lineTo (x, halfH + arrowH * 0.5f);

                g.strokePath (path, PathStrokeType (2.0f));
            }

            r.removeFromRight (3);
            g.drawFittedText (text, r, Justification::centredLeft, 1);

            if (shortcutKeyText.isNotEmpty())
            {
                auto f2 = font;
                f2.setHeight (f2.getHeight() * 0.75f);
                f2.setHorizontalScale (0.95f);
                g.setFont (f2);

                g.drawText (shortcutKeyText, r, Justification::centredRight, true);
            }
        }
    }

    void drawComboBox(Graphics& g, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        ComboBox& box) override
    {
        UNUSED(isButtonDown);
        UNUSED(buttonY);
        UNUSED(buttonH);
        UNUSED(buttonX);
        UNUSED(buttonW);

        // auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(ButtonBackgroundColor);
        g.fillRoundedRectangle(boxBounds.toFloat(), ROUNDED_CORNER_SIZE);

        g.setColour(ButtonBackgroundColor);
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), ROUNDED_CORNER_SIZE, 1.0f);

        Rectangle<int> arrowZone(width - 30, 0, 20, height);
        Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, PathStrokeType(2.0f));
    }

    void drawPopupMenuBackground(Graphics& g, int width, int height) override
    {
        juce::Rectangle bounds{ (float)width, (float)height };

        juce::Colour fillColour = ButtonBackgroundColor;

        g.setColour(fillColour);
        g.fillRoundedRectangle(bounds, ROUNDED_CORNER_SIZE);
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        UNUSED(sliderPos);
        UNUSED(minSliderPos);
        UNUSED(maxSliderPos);
        UNUSED(style);
        UNUSED(slider);

        g.setColour(ButtonBackgroundColor);
        g.fillRoundedRectangle(static_cast<float>(x), 
                                static_cast<float>(y), 
                                static_cast<float>(width), 
                                static_cast<float>(height), 
                                ROUNDED_CORNER_SIZE);
    }

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        UNUSED(buttonHeight);
        return mFont;
    }

private:
    const Font mFont{ MONOSPACE_FONT_OPTIONS };
};
