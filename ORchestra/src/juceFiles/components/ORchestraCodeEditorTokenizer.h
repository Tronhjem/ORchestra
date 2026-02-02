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

/// Inspired by the juce_LuaTokeniser
namespace ORchestra
{
    class ORchestraCodeEditorTokenizer : public juce::CodeTokeniser
    {
    public:
        //==============================================================================
        ORchestraCodeEditorTokenizer() {}
        ~ORchestraCodeEditorTokenizer() override {}

        //==============================================================================
        int readNextToken (CodeDocument::Iterator&) override;
        CodeEditorComponent::ColourScheme getDefaultColourScheme() override;

        enum TokenType
        {
            tokenType_error = 0,
            tokenType_comment,
            tokenType_keyword,
            tokenType_operator,
            tokenType_identifier,
            tokenType_integer,
            tokenType_float,
            tokenType_string,
            tokenType_bracket,
            tokenType_punctuation
        };
    };
} // namespace ORchestra
