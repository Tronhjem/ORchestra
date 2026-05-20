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

#include <limits>
#include <cstdint>
namespace ORchestra 
{
    // DataSequence Operation Lambdas
#define OPERATION_BINARY_LAMBDA(op) [](const int a, const int b) { return (a > 0) op (b > 0); }
#define OPERATION_LAMBDA(op) [](const int a, const int b) { return a op b; }

    constexpr auto Greater = OPERATION_LAMBDA(> );
    constexpr auto GreaterEqual = OPERATION_LAMBDA(>= );
    constexpr auto Lesser = OPERATION_LAMBDA(< );
    constexpr auto LesserEqual = OPERATION_LAMBDA(<= );
    constexpr auto Equal = OPERATION_LAMBDA(== );
    constexpr auto NotEqual = OPERATION_LAMBDA(!= );

    constexpr auto BinaryAND = OPERATION_BINARY_LAMBDA(&);
    constexpr auto BinaryOR = OPERATION_BINARY_LAMBDA(| );
    constexpr auto BinaryXOR = OPERATION_BINARY_LAMBDA(^);

    constexpr auto Add = OPERATION_LAMBDA(+);
    constexpr auto Subtract = OPERATION_LAMBDA(-);
    constexpr auto Multiply = OPERATION_LAMBDA(*);

    constexpr auto Divide = [](const int a, const int b) {
        if (b == 0)
            return 0;
        return a / b;
        };

    constexpr auto Modulo = [](const int a, const int b) {
        if (b == 0)
            return 0;
        return a % b;
        };

    // ============================================================================================

    constexpr int MAX_DATASEQUENCE_LENGTH = 32;
    constexpr int DEFAULT_NOTE_DURATION = 11050;
    constexpr int MAX_SUB_DIVISION_LENGTH = 6;

    constexpr int MIDI_MAX_VALUE = 127;
    constexpr int MAX_MIDI_CHANNEL_NUMBER = 16;

    typedef int16_t DataUnit;
    static_assert(sizeof(DataUnit) == 2);

    constexpr int DATA_UNIT_MAX_VALUE = std::numeric_limits<DataUnit>::max();
    constexpr int DATA_UNIT_MIN_VALUE = std::numeric_limits<DataUnit>::min();

    constexpr int STEP_BUFFER_SIZE = 32;
    constexpr int STEP_BUFFER_SIZE_MASK = STEP_BUFFER_SIZE - 1;
    constexpr int HALF_STEP_BUFFER_SIZE = STEP_BUFFER_SIZE / 2;

    static_assert(STEP_BUFFER_SIZE > 0 && 
                  (STEP_BUFFER_SIZE & (STEP_BUFFER_SIZE - 1)) == 0, 
                  "STEP_BUFFER_SIZE must be a power of two");

    static_assert(STEP_BUFFER_SIZE > 0 && 
                  (STEP_BUFFER_SIZE & (STEP_BUFFER_SIZE_MASK)) == 0, 
                  "STEP_BUFFER_SIZE must be a power of two");

    enum class SequenceStepType : DataUnit
    {
        NoteOn = 0,
        NoteOff = 1,
        CC = 2,
        BPM = 3,
        BEAT = 4,
        PRINT = 5,
        TRANSPOSE = 6
    };
} // namespace ORchestra
