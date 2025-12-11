
#pragma once

#include <JuceHeader.h>

/// Inspired by the LuaTokeniser in JUCE 
namespace ORchestra
{
    class ORchestraCodeEditorTokenizer   : public juce::CodeTokeniser
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
