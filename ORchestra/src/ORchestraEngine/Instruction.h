#pragma once

#include <cstdint>

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
<<<<<<< HEAD
        Instruction() : opCode(OpCode::CONSTANT) {}
        explicit Instruction(OpCode code) : opCode(code), mDataValue() {}
        explicit Instruction(OpCode code, StepData value) : opCode(code), mDataValue(value) {}
        explicit Instruction(OpCode code, uint16_t id) : opCode(code), mId(id) {}
=======
        explicit Instruction() : opCode(OpCode::CONSTANT), mDataValue(0), mNameValue("") {}
        explicit Instruction(OpCode code) : opCode(code), mDataValue(0), mNameValue("") {}
        explicit Instruction(OpCode code, StepData value) : opCode(code), mDataValue(value), mNameValue("") {}
        explicit Instruction(OpCode code, std::string name) : opCode(code), mDataValue(0), mNameValue(name) {}
>>>>>>> 66d386634bcd7cf55c0344103f5f7d9dc33291bd

        OpCode opCode;
        uint16_t mId;
        StepData mDataValue;
    };

} // namespace ORchestra
