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


using namespace ORchestra;
TEST_CASE("NoteNumbers: Converts 'C#0' to MIDI note 13 correctly", "[NoteNumbers]")
{
    std::string file{"a = C#0 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    DataUnit value = result.GetValue(0);
    REQUIRE(value == 13 /*C#0 == 13 */);
}

TEST_CASE("NoteNumbers: Converts 'Db3' to MIDI note 49 correctly", "[NoteNumbers]")
{
    std::string file{"a = Db3 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    DataUnit value = result.GetValue(0);
    REQUIRE(value == 49 /*Db3 == 49 */);
}

TEST_CASE("NoteNumbers: Parses note in array '[G#5, G#5]' as MIDI 80 correctly", "[NoteNumbers]")
{
    std::string file{"a = [G#5, G#5] \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    DataUnit value = result.GetValue(0);
    REQUIRE(value == 80 /*G#5 == 80*/);
}

TEST_CASE("NoteNumbers: Evaluates 'C0 + 1' in array correctly (result=13)", "[NoteNumbers]")
{
    std::string file{"a = [C0 + 1, 0] \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]));

    StepData result = vm.GetTopStackValue();
    DataUnit value = result.GetValue(0);
    REQUIRE(value == 13);
}

TEST_CASE("NoteNumbers: Rejects invalid note 'Dw21312' (compilation fails)", "[NoteNumbers]")
{
    std::string file{"a = Dw21312 \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}

TEST_CASE("NoteNumbers: Rejects incomplete note 'C' without octave (compilation fails)", "[NoteNumbers]")
{
    std::string file{"a = C \n test a"};
    VM vm;
    REQUIRE(vm.Prepare(&file[0]) == false);
}
