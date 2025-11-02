#pragma once

#include <JuceHeader.h>
#include <iostream>

inline juce::Colour lerpColour(const juce::Colour& c1, const juce::Colour& c2, float t)
{
    auto lerp = [t](uint8 a, uint8 b) -> uint8
    {
        return static_cast<uint8>(a + (b - a) * t);
    };
    return juce::Colour(
        lerp(c1.getRed(),   c2.getRed()),
        lerp(c1.getGreen(), c2.getGreen()),
        lerp(c1.getBlue(),  c2.getBlue()),
        lerp(c1.getAlpha(), c2.getAlpha())
    );
}

inline float smoothstep(float edge0, float edge1, float x)
{
    x = (x - edge0) / (edge1 - edge0);
    return x * x * (3 - 2 * x);
}

inline float smootherstep(float edge0, float edge1, float x)
{
    x = (x - edge0) / (edge1 - edge0);
    return x * x * x * (x * (x * 6 - 15) + 10);
}

inline juce::Colour smoothstepColour(const juce::Colour& a, const juce::Colour& b, float t)
{
    t = juce::jlimit(0.0f, 1.0f, t);
    float s = t * t * (3.0f - 2.0f * t);

    float r = juce::jmap(s, a.getFloatRed(),   b.getFloatRed());
    float g = juce::jmap(s, a.getFloatGreen(), b.getFloatGreen());
    float b_ = juce::jmap(s, a.getFloatBlue(),  b.getFloatBlue());
    float a_ = juce::jmap(s, a.getFloatAlpha(), b.getFloatAlpha());

    return juce::Colour::fromFloatRGBA(r, g, b_, a_);
}

unsigned int WriteArrayToBytes(unsigned int* start, unsigned int length)
{
    int array = 0;
    for (int i = 0; i < length; i++)
    {
        array |= start[i] << i;
    }

    return array;
}

unsigned char GetByte(unsigned int number, int index)
{
    return ((1 << index) & number) >> index;
}

void PrintBinary(unsigned x)
{
    std::cout << "Binary : ";
    for (int i = 31; i >= 0; --i)
    {
        int mask = 1 << i;
        int number = (x & mask) >> i;
        std::cout << number;
    }
    std::cout << std::endl;
}

//class ManagedMemory
//{
//public:
//    ManagedMemory() : mFreeIndex(0) {}
//
//    int* Write(int* start, int length)
//    {
//        int* startIndex = &mFreeIndex;
//        for (int i = mFreeIndex; i < mFreeIndex + length; ++i)
//        {
//            mMemory[i] = *(start + i);
//        }
//        mFreeIndex += length;
//
//        return startIndex;
//    }
//
//    int* Get(int index)
//    {
//        return &mMemory[index];
//    }
//
//private:
//    int mFreeIndex;
//    std::array<int, 512> mMemory;
//};
