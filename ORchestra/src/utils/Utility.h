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

#include <JuceHeader.h>
namespace ORchestra
{

#define UNUSED(X) static_cast<void>(X)

    inline juce::Colour lerpColour(const juce::Colour& c1, const juce::Colour& c2, float t)
    {
        auto lerp = [t](uint8 a, uint8 b) -> uint8
            {
                return static_cast<uint8>(a + (b - a) * t);
            };
        return juce::Colour(
            lerp(c1.getRed(), c2.getRed()),
            lerp(c1.getGreen(), c2.getGreen()),
            lerp(c1.getBlue(), c2.getBlue()),
            lerp(c1.getAlpha(), c2.getAlpha()));
    }

    inline float smoothstep(float edge0, float edge1, float x)
    {
        x = (x - edge0) / (edge1 - edge0);
        return x * x * (3.f - 2.f * x);
    }

    inline float smootherstep(float edge0, float edge1, float x)
    {
        x = (x - edge0) / (edge1 - edge0);
        return x * x * x * (x * (x * 6.f - 15.f) + 10.f);
    }

    inline juce::Colour smoothstepColour(const juce::Colour& a, const juce::Colour& b, float t)
    {
        t = juce::jlimit(0.0f, 1.0f, t);
        float s = t * t * (3.0f - 2.0f * t);

        float r = juce::jmap(s, a.getFloatRed(), b.getFloatRed());
        float g = juce::jmap(s, a.getFloatGreen(), b.getFloatGreen());
        float b_ = juce::jmap(s, a.getFloatBlue(), b.getFloatBlue());
        float a_ = juce::jmap(s, a.getFloatAlpha(), b.getFloatAlpha());

        return juce::Colour::fromFloatRGBA(r, g, b_, a_);
    }

} //namespace ORchestra
