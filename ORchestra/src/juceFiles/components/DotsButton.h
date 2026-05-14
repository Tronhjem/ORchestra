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

class DotsButton : public juce::Component
{
public:
    std::function<void()> onClick;

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        const float dotRadius = 2.0f;
        const float gap = 3.5f;
        const float totalWidth = dotRadius * 6.0f + gap * 2.0f;
        const float startX = bounds.getCentreX() - totalWidth * 0.5f + dotRadius;
        const float centreY = bounds.getCentreY();

        juce::Colour dotColour = juce::Colour(ORchestra::ColorPalette::Overlay1);
        if (mIsDown)
            dotColour = dotColour.brighter(0.3f);
        else if (mIsOver)
            dotColour = dotColour.brighter(0.15f);

        g.setColour(dotColour);
        for (int i = 0; i < 3; ++i)
        {
            float cx = startX + static_cast<float>(i) * (dotRadius * 2.0f + gap);
            g.fillEllipse(cx - dotRadius, centreY - dotRadius,
                          dotRadius * 2.0f, dotRadius * 2.0f);
        }
    }

    void mouseEnter(const juce::MouseEvent&) override { mIsOver = true;  repaint(); }
    void mouseExit(const juce::MouseEvent&) override  { mIsOver = false; mIsDown = false; repaint(); }
    void mouseDown(const juce::MouseEvent&) override   { mIsDown = true;  repaint(); }
    void mouseUp(const juce::MouseEvent&) override
    {
        if (mIsDown && mIsOver && onClick)
            onClick();
        mIsDown = false;
        repaint();
    }

private:
    bool mIsOver = false;
    bool mIsDown = false;
};
