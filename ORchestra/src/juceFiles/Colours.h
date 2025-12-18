#pragma once
#include <JuceHeader.h>

using namespace juce;

namespace ORchestra
{
    struct CatppuccinMocha
    {
        static constexpr uint32_t Rosewater = 0xfff5e0dc;
        static constexpr uint32_t Flamingo = 0xfff2cdcd;
        static constexpr uint32_t Pink = 0xfff5c2e7;
        static constexpr uint32_t Mauve = 0xffcba6f7;
        static constexpr uint32_t Red = 0xfff38ba8;
        static constexpr uint32_t Maroon = 0xffeba0ac;
        static constexpr uint32_t Peach = 0xfffab387;
        static constexpr uint32_t Yellow = 0xfff9e2af;
        static constexpr uint32_t Green = 0xffa6e3a1;
        static constexpr uint32_t Teal = 0xff94e2d5;
        static constexpr uint32_t Sky = 0xff89dceb;
        static constexpr uint32_t Sapphire = 0xff74c7ec;
        static constexpr uint32_t Blue = 0xff89b4fa;
        static constexpr uint32_t Lavender = 0xffb4befe;
        static constexpr uint32_t Text = 0xffcdd6f4;
        static constexpr uint32_t Subtext1 = 0xffbac2de;
        static constexpr uint32_t Subtext0 = 0xffa6adc8;
        static constexpr uint32_t Overlay2 = 0xff9399b2;
        static constexpr uint32_t Overlay1 = 0xff7f849c;
        static constexpr uint32_t Overlay0 = 0xff6c7086;
        static constexpr uint32_t Surface2 = 0xff585b70;
        static constexpr uint32_t Surface1 = 0xff45475a;
        static constexpr uint32_t Surface0 = 0xff313244;
        static constexpr uint32_t Base = 0xff1e1e2e;
        static constexpr uint32_t Mantle = 0xff181825;
        static constexpr uint32_t Crust = 0xff11111b;
    };

    struct CatppuccinFrappe 
    {
        static constexpr uint32_t Rosewater = 0xfff2d5cf;
        static constexpr uint32_t Flamingo = 0xffeebebe;
        static constexpr uint32_t Pink = 0xfff4b8e4;
        static constexpr uint32_t Mauve = 0xffca9ee6;
        static constexpr uint32_t Red = 0xffe78284;
        static constexpr uint32_t Maroon = 0xffea999c;
        static constexpr uint32_t Peach = 0xffef9f76;
        static constexpr uint32_t Yellow = 0xffe5c890;
        static constexpr uint32_t Green = 0xffa6d189;
        static constexpr uint32_t Teal = 0xff81c8be;
        static constexpr uint32_t Sky = 0xff99d1db;
        static constexpr uint32_t Sapphire = 0xff85c1dc;
        static constexpr uint32_t Blue = 0xff8caaee;
        static constexpr uint32_t Lavender = 0xffbabbf1;
        static constexpr uint32_t Text = 0xffc6d0f5;
        static constexpr uint32_t Subtext1 = 0xffb5bfe2;
        static constexpr uint32_t Subtext0 = 0xffa5adce;
        static constexpr uint32_t Overlay2 = 0xff949cbb;
        static constexpr uint32_t Overlay1 = 0xff838ba7;
        static constexpr uint32_t Overlay0 = 0xff737994;
        static constexpr uint32_t Surface2 = 0xff626880;
        static constexpr uint32_t Surface1 = 0xff51576d;
        static constexpr uint32_t Surface0 = 0xff414559;
        static constexpr uint32_t Base = 0xff303446;
        static constexpr uint32_t Mantle = 0xff292c3c;
        static constexpr uint32_t Crust = 0xff232634;
    };

    using ColorPalette = CatppuccinMocha;
    struct CodeColor
    {
        static constexpr uint32_t Error = ColorPalette::Red;
        static constexpr uint32_t Comment = ColorPalette::Overlay0;
        static constexpr uint32_t Keyword = ColorPalette::Teal;
        static constexpr uint32_t Operator = ColorPalette::Rosewater;
        static constexpr uint32_t Identifier = ColorPalette::Mauve;
        static constexpr uint32_t Integer = ColorPalette::Text;
        static constexpr uint32_t Float = ColorPalette::Text;
        static constexpr uint32_t Bracket = ColorPalette::Peach;
        static constexpr uint32_t Punctuation = ColorPalette::Text;
    };

    const Colour BackgroundColor = Colour(ColorPalette::Surface0);
    const Colour ButtonBackgroundColor = Colour(ColorPalette::Blue);
    const Colour TextColor = Colour(ColorPalette::Text);

    static const Colour MinVelocityColor = Colour::fromString("#275DAD");
    const Colour MaxVelocityColor = Colour::fromString("#D64933");
    const Colour InactiveStepColor = Colour::fromString("#5B616A");
    
} // namespace ORchestra
