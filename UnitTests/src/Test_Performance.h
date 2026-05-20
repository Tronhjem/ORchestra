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

#include <chrono>
#include <iostream>

#include "catch.hpp"

#include "VM.h"
#include "ErrorReporting.h"
#include "SequenceStep.h"

using namespace ORchestra;

// ============================================================================
// Benchmark helpers
// ============================================================================

struct Benchmark
{
    Benchmark(std::string n) : name(std::move(n)), start(std::chrono::high_resolution_clock::now()) {}

    void stop()
    {
        if (stopped) return;
        auto end = std::chrono::high_resolution_clock::now();
        elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        stopped = true;
    }

    ~Benchmark()
    {
        stop();
        std::cout << "[BENCH] " << name << ": " << elapsedUs << " us";
        if (iterations > 0)
            std::cout << "  (" << iterations << " iters, "
                      << (elapsedUs / static_cast<double>(iterations)) << " us/iter)";
        std::cout << std::endl;
    }

    std::string name;
    std::chrono::high_resolution_clock::time_point start;
    long long elapsedUs = 0;
    int iterations = 0;
    bool stopped = false;
};

// ============================================================================
// Script generators for stress testing
// ============================================================================

static std::string makeSimpleScript()
{
    return "a = [60, 62, 64, 65, 67, 69, 71, 72]\n"
           "note(1, C4, 100, n4)\n";
}

static std::string makeMediumScript()
{
    return
        "scale = [C4, D4, E4, F4, G4, A4, B4, C5]\n"
        "bass = euc(3, 8)\n"
        "hihat = euc(5, 8, 2)\n"
        "kick = [1, 0, 0, 0, 1, 0, 0, 0]\n"
        "chords = [1, 0, 0, 0, 1, 0, 0, 1]\n"
        "vel = 100\n"
        "note(kick[$ % 8] & 1, C3, vel, n4, 1)\n"
        "note(hihat[$ % 8] & 1, F#4, vel + 10, n8, 10)\n"
        "note(chords[$ % 8] & 1, E3, vel + 5, n2, 1)\n"
        "note(chords[$ % 8] & 1, G3, vel + 5, n2, 1)\n"
        "note(chords[$ % 8] & 1, B3, vel + 5, n2, 1)\n"
        "note(bass[$ % 8] & 1, scale[$ % 8], vel, n16, 1)\n"
        "bpm(128)\n"
        "beat(n8)\n"
        "transpose(0)\n";
}

static std::string makeComplexScript()
{
    return
        "fn doubleNote(n, v)\n"
        "  return n + 12\n"
        "end\n"
        "\n"
        "fn accent(vel)\n"
        "  return vel + 20\n"
        "end\n"
        "\n"
        "fn chooseVel(x)\n"
        "  vel = 100\n"
        "  return vel + x\n"
        "end\n"
        "\n"
        "ptn verse\n"
        "  note(kick[$ % 8] & 1, C3, 127, n4, 1)\n"
        "  note(hihat[$ % 8] & 1, F#4, accent(100), n8, 10)\n"
        "  note(hats[$ % 8] & 1, A#3, 60, n16, 10)\n"
        "  note(snare[$ % 8] & 1, D3, 80, n4, 10)\n"
        "  note(bass[$ % 16] & 1, bassline[$ % 16], 127, n8, 1)\n"
        "  note(arp[$ % 16] & 1, arpNotes[$ % 16], 80, n32, 1)\n"
        "end\n"
        "\n"
        "ptn chorus\n"
        "  note(kick[$ % 8] & 1, C3, 127, n4, 1)\n"
        "  note(kick[$ % 8] & 1, E3, 127, n4, 1)\n"
        "  note(hihat[$ % 4] & 1, F#4, 110, n8, 10)\n"
        "  note(snare[$ % 8] & 1, D3, 100, n4, 10)\n"
        "  note(bass[$ % 16] & 1, bassline[$ % 16] + 12, 127, n8, 1)\n"
        "  note(chords[$ % 8] & 1, C4, chooseVel(8), n4, 1)\n"
        "  note(chords[$ % 8] & 1, E4, 127, n4, 1)\n"
        "  note(chords[$ % 8] & 1, G4, 127, n4, 1)\n"
        "end\n"
        "\n"
        "kick = [1, 0, 0, 0, 1, 0, 0, 0]\n"
        "snare = [0, 0, 1, 0, 0, 0, 1, 0]\n"
        "hihat = euc(5, 8, 2)\n"
        "hats = euc(7, 16)\n"
        "bass = euc(3, 16)\n"
        "arp = [1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0]\n"
        "chords = euc(3, 8)\n"
        "bassline = [36, 36, 48, 48, 43, 43, 41, 41, 36, 36, 48, 48, 43, 43, 41, 41]\n"
        "arpNotes = [C4, E4, G4, C5, D4, F4, A4, D5, E4, G4, B4, E5, F4, A4, C5, F5]\n"
        "scale = [C4, D4, E4, F4, G4, A4, B4, C5]\n"
        "song = [verse, chorus]\n"
        "vel = 100\n"
        "song($ / 16 % 2)\n"
        "bpm(128)\n"
        "beat(n8)\n"
        "transpose(0)\n";
}

// ============================================================================
// Performance Tests
// ============================================================================

TEST_CASE("Performance: Baseline trivial script (Prepare + Tick x1000)", "[Performance]")
{
    std::string script = makeSimpleScript();

    {
        Benchmark b("Prepare simple");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.stop();
    }

    {
        Benchmark b("Tick x1000 simple");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.iterations = 1000;
        for (int i = 0; i < 1000; ++i)
        {
            std::vector<SequenceStep> steps;
            REQUIRE(vm.Tick(steps, i));
        }
    }
}

TEST_CASE("Performance: Medium script (Prepare + Tick x1000)", "[Performance]")
{
    std::string script = makeMediumScript();

    {
        Benchmark b("Prepare medium");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.stop();
    }

    {
        Benchmark b("Tick x1000 medium");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.iterations = 1000;
        for (int i = 0; i < 1000; ++i)
        {
            std::vector<SequenceStep> steps;
            REQUIRE(vm.Tick(steps, i));
        }
    }
}

TEST_CASE("Performance: Complex script with patterns and functions (Prepare + Tick x1000)", "[Performance]")
{
    std::string script = makeComplexScript();

    {
        Benchmark b("Prepare complex");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.stop();
    }

    {
        Benchmark b("Tick x1000 complex");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.iterations = 1000;
        for (int i = 0; i < 1000; ++i)
        {
            std::vector<SequenceStep> steps;
            REQUIRE(vm.Tick(steps, i));
        }
    }
}

TEST_CASE("Performance: Complex script Tick x10000 stress", "[Performance]")
{
    std::string script = makeComplexScript();

    ErrorReporting errorReporter;
    VM vm(errorReporter);
    REQUIRE(vm.Prepare(script));

    Benchmark b("Tick x10000 complex stress");
    b.iterations = 10000;
    for (int i = 0; i < 10000; ++i)
    {
        std::vector<SequenceStep> steps;
        REQUIRE(vm.Tick(steps, i));
    }
}

TEST_CASE("Performance: Multiple Prepare cycles (10x complex script)", "[Performance]")
{
    std::string script = makeComplexScript();

    Benchmark b("10x Prepare complex");
    b.iterations = 10;
    for (int n = 0; n < 10; ++n)
    {
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
    }
}

TEST_CASE("Performance: Max-sized arrays with euclidean (Prepare + Tick x100)", "[Performance]")
{
    // 32-element arrays are the max
    std::string script =
        "a = euc(15, 32)\n"
        "b = euc(7, 32, 4)\n"
        "c = euc(3, 32, 8)\n"
        "d = euc(19, 32, 12)\n"
        "e = euc(11, 32, 16)\n"
        "f = euc(23, 32)\n"
        "note(a[$ % 32] & b[$ % 32] & c[$ % 32], C4, d[$ % 32] * 127, n16)\n"
        "note(e[$ % 32] & 1, E4, f[$ % 32] * 127, n8)\n"
        "bpm(140)\n"
        "beat(n16)\n";

    {
        Benchmark b("Prepare max arrays");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.stop();
    }

    {
        Benchmark b("Tick x100 max arrays");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.iterations = 100;
        for (int i = 0; i < 100; ++i)
        {
            std::vector<SequenceStep> steps;
            REQUIRE(vm.Tick(steps, i));
        }
    }
}

TEST_CASE("Performance: Deeply nested arithmetic in arrays (Prepare + Tick x100)", "[Performance]")
{
    std::string script =
        "a = 10\n"
        "b = 5\n"
        "c = 2\n"
        "d = 3\n"
        "e = 4\n"
        "result = (a + b) * (c + d) - (a / c) + (b % d) * e\n"
        "arr = [1+2*3, 4+5*6+7, 8-3+9*2, 7/2+3%2, a+b*c-d/e, 10+20*30-15]\n"
        "note(result > 50 & 1, C4, arr[$ % 6], n4)\n"
        "note(arr[0] > 5 & 1, D4, arr[1], n8)\n";

    {
        Benchmark b("Prepare nested arith");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.stop();
    }

    {
        Benchmark b("Tick x100 nested arith");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.iterations = 100;
        for (int i = 0; i < 100; ++i)
        {
            std::vector<SequenceStep> steps;
            REQUIRE(vm.Tick(steps, i));
        }
    }
}

TEST_CASE("Performance: Many chained function calls (Prepare + Tick x100)", "[Performance]")
{
    std::string script =
        "fn add2(x)\n  return x + 2\nend\n"
        "fn mul3(x)\n  return x * 3\nend\n"
        "fn sub5(x)\n  return x - 5\nend\n"
        "fn div2(x)\n  return x / 2\nend\n"
        "fn add10(x)\n  return x + 10\nend\n"
        "fn mul4(x)\n  return x * 4\nend\n"
        "a = add2(mul3(sub5(div2(add10(mul4(1))))))\n"
        "note(a > 0 & 1, C4, 100, n4)\n";

    {
        Benchmark b("Prepare chained funcs");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.stop();
    }

    {
        Benchmark b("Tick x100 chained funcs");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.iterations = 100;
        for (int i = 0; i < 100; ++i)
        {
            std::vector<SequenceStep> steps;
            REQUIRE(vm.Tick(steps, i));
        }
    }
}

TEST_CASE("Performance: All operators in single expression (Prepare + Tick x100)", "[Performance]")
{
    std::string script =
        "x = (10 + 5) * 2 - 8 / 4 + 7 % 3\n"
        "y = x > 20 & x < 50\n"
        "z = (x == 29) | (x != 30)\n"
        "w = y ^ z\n"
        "test x + y + z + w\n";

    {
        Benchmark b("Prepare all ops");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.stop();
    }

    {
        Benchmark b("Tick x100 all ops");
        ErrorReporting errorReporter;
        VM vm(errorReporter);
        REQUIRE(vm.Prepare(script));
        b.iterations = 100;
        for (int i = 0; i < 100; ++i)
        {
            std::vector<SequenceStep> steps;
            REQUIRE(vm.Tick(steps, i));
        }
    }
}
