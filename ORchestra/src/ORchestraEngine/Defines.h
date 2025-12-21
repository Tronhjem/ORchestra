#pragma once

namespace ORchestra 
{
    // DataSequence Operation Lambdas
#define OPERATION_BINARY_LAMBDA(op) [](const int a, const int b) { return (a > 0) op (b > 0); }
#define OPERATION_LAMBDA(op) [](const int a, const int b) { return a op b; }

    const auto Greater = OPERATION_LAMBDA(> );
    const auto GreaterEqual = OPERATION_LAMBDA(>= );
    const auto Lesser = OPERATION_LAMBDA(< );
    const auto LesserEqual = OPERATION_LAMBDA(<= );
    const auto Equal = OPERATION_LAMBDA(== );
    const auto NotEqual = OPERATION_LAMBDA(!= );

    const auto BinaryAND = OPERATION_BINARY_LAMBDA(&);
    const auto BinaryOR = OPERATION_BINARY_LAMBDA(| );
    const auto BinaryXOR = OPERATION_BINARY_LAMBDA(^);

    const auto Add = OPERATION_LAMBDA(+);
    const auto Subtract = OPERATION_LAMBDA(-);
    const auto Multiply = OPERATION_LAMBDA(*);

    const auto Divide = [](const int a, const int b) {
        if (b == 0)
            return 0;
        return a / b;
        };

    const auto Modulo = [](const int a, const int b) {
        if (b == 0)
            return 0;
        return a % b;
        };

    // ============================================================================================

    constexpr int MAX_DATASEQUENCE_LENGTH = 32;
    constexpr int DEFAULT_NOTE_DURATION = 11050;
    constexpr int MAX_SUB_DIVISION_LENGTH = 6;
    constexpr int MAX_MIDI_CHANNEL_NUMBER = 16;

    typedef unsigned char DataUnit;
    static_assert(sizeof(DataUnit) == 1);
    constexpr int DATA_UNIT_MAX_VALUE = 127;
    constexpr int DATA_UNIT_MIN_VALUE = 0;

    enum class MidiType : DataUnit
    {
        NoteOn = 0,
        NoteOff = 1,
        CC = 2
    };
} // namespace ORchestra
