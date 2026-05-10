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

#include "catch.hpp"

#include "Instruction.h"

using namespace ORchestra;

TEST_CASE("Instruction: Constructions with OpCode only (no operand)", "[Instruction]")
{
    Instruction instr(OpCode::END);
    REQUIRE(instr.GetOpCode() == OpCode::END);
    REQUIRE(instr.GetOperand() == 0);
}

TEST_CASE("Instruction: Construction with OpCode and operand packs correctly", "[Instruction]")
{
    Instruction instr(OpCode::CONSTANT, static_cast<DataUnit>(42));
    REQUIRE(instr.GetOpCode() == OpCode::CONSTANT);
    REQUIRE(instr.GetOperand() == 42);
}

TEST_CASE("Instruction: Construction with maximum operand value 255", "[Instruction]")
{
    Instruction instr(OpCode::CONSTANT, static_cast<DataUnit>(255));
    REQUIRE(instr.GetOpCode() == OpCode::CONSTANT);
    REQUIRE(instr.GetOperand() == 255);
}

TEST_CASE("Instruction: Construction with minimum operand value 0", "[Instruction]")
{
    Instruction instr(OpCode::CONSTANT, static_cast<DataUnit>(0));
    REQUIRE(instr.GetOpCode() == OpCode::CONSTANT);
    REQUIRE(instr.GetOperand() == 0);
}

TEST_CASE("Instruction: Different OpCodes preserve correct operand", "[Instruction]")
{
    Instruction instr(OpCode::ADD, static_cast<DataUnit>(10));
    REQUIRE(instr.GetOpCode() == OpCode::ADD);
    REQUIRE(instr.GetOperand() == 10);
}

TEST_CASE("Instruction: MAX OpCode enum value packs correctly", "[Instruction]")
{
    Instruction instr(OpCode::END, static_cast<DataUnit>(127));
    REQUIRE(instr.GetOpCode() == OpCode::END);
    REQUIRE(instr.GetOperand() == 127);
}

TEST_CASE("Instruction: Default constructor zeros the encoded value", "[Instruction]")
{
    Instruction instr;
    REQUIRE(instr.GetOpCode() == OpCode::CONSTANT); // CONSTANT=0
    REQUIRE(instr.GetOperand() == 0);
}

TEST_CASE("Instruction: SET_IDENTIFIER_VALUE packs operand correctly", "[Instruction]")
{
    Instruction instr(OpCode::SET_IDENTIFIER_VALUE, static_cast<DataUnit>(7));
    REQUIRE(instr.GetOpCode() == OpCode::SET_IDENTIFIER_VALUE);
    REQUIRE(instr.GetOperand() == 7);
}

TEST_CASE("Instruction: GET_IDENTIFIER_VALUE packs operand correctly", "[Instruction]")
{
    Instruction instr(OpCode::GET_IDENTIFIER_VALUE, static_cast<DataUnit>(3));
    REQUIRE(instr.GetOpCode() == OpCode::GET_IDENTIFIER_VALUE);
    REQUIRE(instr.GetOperand() == 3);
}

TEST_CASE("Instruction: EXEC_FUNC_ARRAY packs array ID operand correctly", "[Instruction]")
{
    Instruction instr(OpCode::EXEC_FUNC_ARRAY, static_cast<DataUnit>(1));
    REQUIRE(instr.GetOpCode() == OpCode::EXEC_FUNC_ARRAY);
    REQUIRE(instr.GetOperand() == 1);
}

TEST_CASE("Instruction: NOTE opcode without operand", "[Instruction]")
{
    Instruction instr(OpCode::NOTE);
    REQUIRE(instr.GetOpCode() == OpCode::NOTE);
    REQUIRE(instr.GetOperand() == 0);
}

TEST_CASE("Instruction: CC opcode without operand", "[Instruction]")
{
    Instruction instr(OpCode::CC);
    REQUIRE(instr.GetOpCode() == OpCode::CC);
    REQUIRE(instr.GetOperand() == 0);
}
