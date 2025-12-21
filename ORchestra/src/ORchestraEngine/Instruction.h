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
        END,
#if _DEBUG
        PRINT
#endif
    };

    class Instruction
    {
    public:
        Instruction() : opCode(OpCode::CONSTANT) {}
        explicit Instruction(OpCode code) : opCode(code) {}
        explicit Instruction(OpCode code, DataUnit value) : opCode(code), mDataValue(value) {}
        explicit Instruction(OpCode code, uint16_t id) : opCode(code), mId(id) {}

        OpCode opCode;
        DataUnit mDataValue;
        uint16_t mId;
    };

} // namespace ORchestra
