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

#include <cstdint>
#include "Defines.h"

namespace ORchestra 
{
    enum class OpCode : DataUnit
    {
        CONSTANT = 0,
        GET_IDENTIFIER_VALUE,
        GET_IDENTIFIER_WITH_INDEX,
        SET_IDENTIFIER_VALUE,
        SET_IDENTIFIER_ARRAY,
        SET_IDENTIFIER_WITH_INDEX,

        SET_SUBSTEP_ARRAY,
        GENERATE_EUCLID_SEQUENCE,

        // Math
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        MODULO,

        // Logical
        AND,
        OR,
        XOR,

        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        EQUAL,
        NOT_EQUAL,

        // Specific
        NOTE,
        CC,

        // Built in Functions
        CALL_FUNCTION,
        GET_RANDOM_IN_RANGE,
        GET_GLOBAL_COUNT,
        PRINT,
        SET_BPM,
        SET_BPM_DIVISION,
        NEGATE,
        SET_TRANSPOSE,

        UPDATE_IDENTIFIER_VALUE,
        EXEC_FUNC_ARRAY,

        //end
        END,
    };

    class Instruction
    {
    public:
        // This is tested to be faster, and not just to be fancy.
        // We pack OpCode and operand together to save some space. 
        Instruction() : mEncoded(0) {}
        explicit Instruction(OpCode code) 
            : mEncoded(static_cast<uint16_t>(static_cast<uint16_t>(static_cast<DataUnit>(code)) << 8)) {}
        explicit Instruction(OpCode code, DataUnit operand) 
            : mEncoded(static_cast<uint16_t>((static_cast<uint16_t>(static_cast<DataUnit>(code)) << 8) | operand)) {}

        OpCode GetOpCode() const { return static_cast<OpCode>(mEncoded >> 8); }
        DataUnit GetOperand() const { return static_cast<DataUnit>(mEncoded & 0xFF); }

    private:
        uint16_t mEncoded;  // Upper 8 bits = OpCode, Lower 8 bits = operand
    };

} // namespace ORchestra
