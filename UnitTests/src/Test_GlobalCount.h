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

#include "VM.h"
#include "Scanner.h"
#include "Compiler.h"
#include "ErrorReporting.h"
#include "ORchestraToken.h"

using namespace ORchestra;

TEST_CASE("Scanner: Tokenizes '$' as DOLLAR token", "[GlobalCount]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "$";
    
    REQUIRE(scanner.ScanFile(input.c_str()));
    
    auto& tokens = scanner.GetTokens();
    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::DOLLAR);
}

TEST_CASE("Scanner: Tokenizes expression with $ correctly", "[GlobalCount]")
{
    ErrorReporting errorReporting;
    Scanner scanner(errorReporting);
    std::string input = "x = $ + 5";
    
    REQUIRE(scanner.ScanFile(input.c_str()));
    
    auto& tokens = scanner.GetTokens();
    // Should have: x, =, $, +, 5, EOL, END
    REQUIRE(tokens.size() >= 6);
    REQUIRE(tokens[0].GetType() == ORchestraTokenType::IDENTIFIER);
    REQUIRE(tokens[1].GetType() == ORchestraTokenType::EQUAL);
    REQUIRE(tokens[2].GetType() == ORchestraTokenType::DOLLAR);
    REQUIRE(tokens[3].GetType() == ORchestraTokenType::PLUS);
    REQUIRE(tokens[4].GetType() == ORchestraTokenType::NUMBER);
}

TEST_CASE("VM: $ returns 0 in PreprocessOpCodes (Prepare)", "[GlobalCount]")
{
    VM vm;
    std::string script = "x = $";
    
    // This should succeed - $ evaluates to 0 during Prepare
    REQUIRE(vm.Prepare(script));
}

TEST_CASE("VM: $ returns globalCount value in Tick", "[GlobalCount]")
{
    VM vm;
    std::string script = "x = $\ntest x";
    
    REQUIRE(vm.Prepare(script));
    
    std::vector<SequenceStep> steps;
    
    // Call Tick with globalCount = 5
    REQUIRE(vm.Tick(steps, 5));
    
    // The variable x should now be 5
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 5);
}

TEST_CASE("VM: $ can be used in arithmetic expressions during Tick", "[GlobalCount]")
{
    VM vm;
    std::string script = "x = $ * 2\ntest x";
    
    REQUIRE(vm.Prepare(script));
    
    std::vector<SequenceStep> steps;
    
    // Call Tick with globalCount = 3
    REQUIRE(vm.Tick(steps, 3));
    
    // The variable x should now be 6 (3 * 2)
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 6);
}

TEST_CASE("VM: $ can be used in comparison expressions", "[GlobalCount]")
{
    VM vm;
    std::string script = "x = $ > 5\ntest x";
    
    REQUIRE(vm.Prepare(script));
    
    std::vector<SequenceStep> steps;
    
    // Call Tick with globalCount = 10
    REQUIRE(vm.Tick(steps, 10));
    
    // The variable x should now be 1 (true)
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 1);
}

TEST_CASE("VM: $ evaluates to different values on different Tick calls", "[GlobalCount]")
{
    VM vm;
    std::string script = "x = $\ntest x";
    
    REQUIRE(vm.Prepare(script));
    
    std::vector<SequenceStep> steps1;
    REQUIRE(vm.Tick(steps1, 0));
    StepData result1 = vm.GetTopStackValue();
    REQUIRE(result1.GetValue(0) == 0);
    
    std::vector<SequenceStep> steps2;
    REQUIRE(vm.Tick(steps2, 10));
    StepData result2 = vm.GetTopStackValue();
    REQUIRE(result2.GetValue(0) == 10);
    
    std::vector<SequenceStep> steps3;
    REQUIRE(vm.Tick(steps3, 100));
    StepData result3 = vm.GetTopStackValue();
    REQUIRE(result3.GetValue(0) == 100);
}

TEST_CASE("VM: $ can be used with modulo to create patterns", "[GlobalCount]")
{
    VM vm;
    std::string script = "x = $ % 4\ntest x";
    
    REQUIRE(vm.Prepare(script));
    
    std::vector<SequenceStep> steps;
    
    // Call Tick with globalCount = 7
    REQUIRE(vm.Tick(steps, 7));
    
    // The variable x should now be 3 (7 % 4)
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 3);
}

#if _TEST
TEST_CASE("VM: $ resolves to globalCount value and can be retrieved", "[GlobalCount]")
{
    VM vm;
    std::string script = "test $";
    
    REQUIRE(vm.Prepare(script));
    
    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 42));
    
    // Check that the top stack value is 42
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 42);
}

TEST_CASE("VM: $ in expression evaluates correctly", "[GlobalCount]")
{
    VM vm;
    std::string script = "test $ + 10";
    
    REQUIRE(vm.Prepare(script));
    
    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 15));
    
    // Check that the top stack value is 25 (15 + 10)
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 25);
}

TEST_CASE("VM: $ in complex expression", "[GlobalCount]")
{
    VM vm;
    std::string script = "test $ * 2 + 5";
    
    REQUIRE(vm.Prepare(script));
    
    std::vector<SequenceStep> steps;
    REQUIRE(vm.Tick(steps, 10));
    
    // Check that the top stack value is 25 (10 * 2 + 5)
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 25);
}

TEST_CASE("VM: $ evaluates to 0 during Prepare (ProcessOpCodes)", "[GlobalCount]")
{
    VM vm;
    std::string script = "test $";
    
    REQUIRE(vm.Prepare(script));
    
    // During Prepare, $ should evaluate to 0
    StepData result = vm.GetTopStackValue();
    REQUIRE(result.GetValue(0) == 0);
}
#endif
