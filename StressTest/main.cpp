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

// Stress harness for the ORchestra engine's threading model.
//
// Simulates the three threads that race in the plugin:
//   audio thread  -> engine.Tick() at buffer cadence, with periodic loop-back seeks
//   compile       -> engine.Compile() fired frequently, incl. an invalid script
//   UI thread     -> GetStepDataSlotCopy()/GetErrors() like Timeline/ConsolePanel do
//
// Build with STRESS_ASSERTS=ON (default) for AssertMutex tripwires, or with
// -DENABLE_TSAN=ON -DSTRESS_ASSERTS=OFF to run under ThreadSanitizer.

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>

#include "ORchestraEngine.h"
#include "TransportData.h"

#if defined(_DEBUG)
#include "ORchestraAssert.h"
#endif

using namespace ORchestra;
using namespace std::chrono_literals;

namespace
{
    constexpr const char* kScripts[] =
    {
        // valid, matches the plugin default
        "beat(n8)\n\ntrig = euc(3, 8)\n\nnote(trig, C4, 100, n8)\n",
        // valid, denser
        "beat(n16)\n\na = euc(5, 8)\nb = ran(40, 90)\n\nnote(a, b, 100, n16)\ncc(a, 20, b)\n",
        // valid, script-set bpm + transpose
        "bpm(90)\nbeat(n8)\ntranspose(2)\n\nt = euc(3, 8)\n\nnote(t, C3, 110, n4)\n",
        // invalid on purpose: drives worker-side error logging (kept last so
        // "calm" mode can skip it)
        "this is not a valid script @@@\n",
    };

    constexpr int kNumScripts = sizeof(kScripts) / sizeof(kScripts[0]);
}

int main(int argc, char** argv)
{
    const int secondsToRun = (argc > 1) ? std::atoi(argv[1]) : 20;
    // "calm" mode: no invalid scripts and no error clearing, so the hot
    // ErrorReporting race doesn't crash the run before deeper races surface.
    const bool calmMode = (argc > 2) && (std::string(argv[2]) == "calm");
    const int scriptCount = calmMode ? kNumScripts - 1 : kNumScripts;

#if defined(_DEBUG)
    ORchestra::AssertSink::Set([](const std::string& condition,
                                  const std::string& message,
                                  const std::string& backtrace)
    {
        std::fprintf(stderr, "\n[STRESS ASSERT] %s %s\n%s\n",
                     condition.c_str(), message.c_str(), backtrace.c_str());
        std::fflush(stderr);
    });
#endif

    std::printf("Stress run: %d s, 3 threads (audio / compile / ui)\n", secondsToRun);
    std::fflush(stdout);

    ORchestraEngine engine;
    std::atomic<bool> stop{ false };

    std::thread audioThread([&]()
    {
        TransportData transport;
        transport.isPlaying = true;
        transport.sampleRate = 44100.0;
        transport.bpm = 120.0;
        transport.bpmDivision = 1.0f;

        const int bufferLength = 128;
        const int64_t loopLengthSamples = 44100 * 2; // seek back every ~2 s
        juce::MidiBuffer midi;

        while (!stop.load(std::memory_order_relaxed))
        {
            transport.timeInSamples += bufferLength;
            if (transport.timeInSamples >= loopLengthSamples)
                transport.timeInSamples = 0; // exercises the seek path

            engine.Tick(transport, bufferLength, midi);
            midi.clear();

            std::this_thread::sleep_for(1ms);
        }
    });

    std::thread compileThread([&]()
    {
        int i = 0;
        while (!stop.load(std::memory_order_relaxed))
        {
            engine.Compile(kScripts[i % scriptCount]);
            ++i;
            std::this_thread::sleep_for(3ms);
        }
    });

    std::thread uiThread([&]()
    {
        std::string sink;
        sink.reserve(4096);

        while (!stop.load(std::memory_order_relaxed))
        {
            sink.clear();

            for (size_t s = 0; s < STEP_BUFFER_SIZE; ++s)
            {
                const std::vector<SequenceStep> slot = engine.GetStepDataSlotCopy(s);
                for (const auto& step : slot)
                    sink += static_cast<char>(step.mType);
            }

            for (const auto& entry : engine.GetErrors())
                sink += entry.mMessage;

            sink += engine.GetInstructionData();

            if (!calmMode)
                engine.RequestClearErrors();

            std::this_thread::sleep_for(8ms);
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(secondsToRun));
    stop.store(true, std::memory_order_relaxed);

    audioThread.join();
    compileThread.join();
    uiThread.join();

    std::printf("Stress run completed (%d s), no assert fired\n", secondsToRun);
    return 0;
}
