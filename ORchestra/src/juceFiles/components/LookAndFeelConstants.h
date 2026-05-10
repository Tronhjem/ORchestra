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

constexpr float NORMAL_FONT_SIZE = 15.f;
#if JUCE_MAC
    const juce::FontOptions MONOSPACE_FONT_OPTIONS{ "Menlo", NORMAL_FONT_SIZE, juce::Font::bold };
#elif JUCE_WINDOWS
    const juce::FontOptions MONOSPACE_FONT_OPTIONS{ "Consolas", NORMAL_FONT_SIZE, juce::Font::bold };
#else
    const juce::FontOptions MONOSPACE_FONT_OPTIONS{ juce::Font::findDefaultMonospaceFont().getTypefaceName(), NORMAL_FONT_SIZE, juce::Font::bold };
#endif
constexpr float ROUNDED_CORNER_SIZE = 4.f;
constexpr float BAR_LINE_THICKNESS = 3.f;
constexpr float QAURTER_BAR_LINE_THICKNESS = BAR_LINE_THICKNESS / 4.f;
constexpr float COMPONENT_LINE_THICKNESS = 2.f;
constexpr float OUTLINE_THICKNESS = 2.f;
constexpr float VERTICAL_SEPARATOR_THICKNESS = 4.f;

