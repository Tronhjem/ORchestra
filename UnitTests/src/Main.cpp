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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// Include all test files
#include "Test_ArithmeticOperators.h"
#include "Test_LogicalOperators.h"
#include "Test_ComparisonOperators.h"
#include "Test_ArrayOperators.h"
#include "Test_EuclidAndRandom.h"
#include "Test_ErrorHandling.h"
#include "Test_DataSequence.h"
#include "Test_NoteNumbers.h"
#include "Test_CustomStack.h"
#include "Test_Token.h"
#include "Test_Scanner_BasicTokens.h"
#include "Test_Scanner_Operators.h"
#include "Test_Scanner_Expressions.h"
#include "Test_FileLoader.h"
#include "Test_Compiler.h"
#include "Test_GlobalCount.h"
#include "Test_UserFunctions.h"
#include "Test_NoteDivisionLiterals.h"
#include "Test_NoteAndCC.h"
#include "Test_Scanner_EdgeCases.h"
#include "Test_BuiltInFunctions.h"
#include "Test_ErrorReportingClass.h"
#include "Test_Instruction.h"
#include "Test_VM_EdgeCases.h"
#include "Test_Performance.h"

