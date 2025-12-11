#pragma once

#include "catch.hpp"

#include "VM.h"
#include "TransportData.h"

using namespace ORchestra;

TEST_CASE("BPM: Sets BPM value correctly", "[BPM]")
{
    std::string file{"bpm(120)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpm == 120.0);
}

TEST_CASE("BPM: Rejects bpm() with no parameters (compilation fails)", "[BPM]")
{
    std::string file{"bpm()\n test 1"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("BPM: Sets different BPM values", "[BPM]")
{
    std::string file{"bpm(100)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpm == 100.0);
}

TEST_CASE("NoteDiv: Sets note division correctly for quarter note", "[NoteDiv]")
{
    std::string file{"noteDiv(3)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpmDivision == 1.0f);
}

TEST_CASE("NoteDiv: Rejects noteDiv() with no parameters (compilation fails)", "[NoteDiv]")
{
    std::string file{"noteDiv()\n test 1"};
    VM vm;
    REQUIRE(vm.Prepare(file) == false);
}

TEST_CASE("NoteDiv: Sets whole note division (1)", "[NoteDiv]")
{
    std::string file{"noteDiv(1)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpmDivision == 0.25f);
}

TEST_CASE("NoteDiv: Sets half note division (2)", "[NoteDiv]")
{
    std::string file{"noteDiv(2)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpmDivision == 0.5f);
}

TEST_CASE("NoteDiv: Sets 8th note division (4)", "[NoteDiv]")
{
    std::string file{"noteDiv(4)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpmDivision == 2.0f);
}

TEST_CASE("NoteDiv: Sets 16th note division (5)", "[NoteDiv]")
{
    std::string file{"noteDiv(5)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpmDivision == 4.0f);
}

TEST_CASE("NoteDiv: Sets 32nd note division (6)", "[NoteDiv]")
{
    std::string file{"noteDiv(6)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpmDivision == 8.0f);
}

TEST_CASE("NoteDiv: Sets 64th note division (7)", "[NoteDiv]")
{
    std::string file{"noteDiv(7)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpmDivision == 16.0f);
}

TEST_CASE("BPM and NoteDiv: Can be used together", "[BPM][NoteDiv]")
{
    std::string file{"bpm(80)\nnoteDiv(4)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpm == 80.0);
    REQUIRE(transportData.bpmDivision == 2.0f);
}

TEST_CASE("BPM: Can use variable values", "[BPM]")
{
    std::string file{"tempo = 110\nbpm(tempo)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpm == 110.0);
}

TEST_CASE("NoteDiv: Can use variable values", "[NoteDiv]")
{
    std::string file{"div = 5\nnoteDiv(div)\n test 1"};
    VM vm;
    TransportData transportData;
    vm.SetTransportData(&transportData);
    
    REQUIRE(vm.Prepare(file));
    REQUIRE(transportData.bpmDivision == 4.0f);
}
