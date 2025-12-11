#pragma once

#include "StepData.h"
#include <string>

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
        // TRACK,
        NOTE,
        CC,

        // Built in Functions
        CALL_FUNCTION,
        GET_RANDOM_IN_RANGE,
        GET_GLOBAL_COUNT,
        SET_BPM,
        SET_NOTE_DIVISION,
        END,
#if _DEBUG
        PRINT
#endif
    };

    class Instruction
    {
    public:
        explicit Instruction() : opCode(OpCode::CONSTANT), mDataValue(0), mNameValue("") {}
        explicit Instruction(OpCode code) : opCode(code), mDataValue(0), mNameValue("") {}
        explicit Instruction(OpCode code, StepData value) : opCode(code), mDataValue(value), mNameValue("") {}
        explicit Instruction(OpCode code, std::string name) : opCode(code), mDataValue(0), mNameValue(name) {}

        OpCode opCode;
        StepData mDataValue;
        std::string mNameValue;
    };
} // namespace ORchestra
