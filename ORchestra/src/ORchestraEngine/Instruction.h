#pragma once

#include "StepData.h"

namespace ORchestra {


enum class OpCode : DataUnit
{
    CONSTANT = 0,
    GET_IDENTIFIER_VALUE,
    GET_IDENTIFIER_WITH_INDEX,
    SET_IDENTIFIER_VALUE,
    SET_IDENTIFIER_ARRAY,

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
    END,
#if _DEBUG
    PRINT
#endif
};

class Instruction
{
public:
    Instruction() : opCode(OpCode::CONSTANT) {}
    explicit Instruction(OpCode code) : opCode(code), mDataValue() {}
    explicit Instruction(OpCode code, StepData value) : opCode(code), mDataValue(value) {}
    explicit Instruction(OpCode code, std::string name) : opCode(code), mNameValue(name) {}

    OpCode opCode;
    StepData mDataValue;
    std::string mNameValue;
};


} // namespace ORchestra
