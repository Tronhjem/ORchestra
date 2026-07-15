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

#include <string>
#include <functional>

#if defined(_DEBUG)
    #include <cassert>
#endif

namespace ORchestra
{
    #if defined(_DEBUG)
    // Debug-only sink that ORCHESTRA_ASSERT forwards failure messages + backtraces to.
    // The plugin wrapper installs a juce::Logger-backed implementation; tests leave it
    // null so the macros fall back to plain assert(). MUST be set before any assert can
    // fire, and is global (one sink per process).
    struct AssertSink
    {
        using LogFn = std::function<void(const std::string& condition,
                                         const std::string& message,
                                         const std::string& backtrace)>;
        static LogFn& Get() { static LogSinkFnInstance inst; return inst.fn; }
        static void Set(LogFn fn) { Get() = std::move(fn); }

    private:
        struct LogSinkFnInstance { LogFn fn; };
    };
    #endif
}

// Example with message:
//   ORCHESTRA_ASSERT(mReadySteps > 0, "mReadySteps underflow at step " << currentStep);
// Example without:
//   ORCHESTRA_ASSERT_SIMPLE(ptr != nullptr);
#if defined(_DEBUG)
    #include <sstream>
    #if __has_include(<execinfo.h>)
        #include <execinfo.h>
        #include <unistd.h>
        #define ORCHESTRA_CAPTURE_BACKTRACE_() \
            [&]() -> std::string { \
                void* frames[64]; \
                int n = ::backtrace(frames, 64); \
                char** syms = ::backtrace_symbols(frames, n); \
                std::string out; \
                if (syms) { \
                    for (int i = 0; i < n; ++i) { \
                        out += syms[i]; out += '\n'; \
                    } \
                    ::free(syms); \
                } \
                return out; \
            }()
    #else
        #define ORCHESTRA_CAPTURE_BACKTRACE_() std::string{}
    #endif

    #define ORCHESTRA_ASSERT(condition, message)                                    \
        do                                                                         \
        {                                                                          \
            if (!(condition))                                                      \
            {                                                                      \
                std::ostringstream _oss; _oss << message;                          \
                const std::string _cond = #condition;                                \
                const std::string _msg = _oss.str();                                 \
                const std::string _bt  = ORCHESTRA_CAPTURE_BACKTRACE_();             \
                auto& _fn = ::ORchestra::AssertSink::Get();                          \
                if (_fn) _fn(_cond, _msg, _bt);                                     \
                assert(condition);                                                  \
            }                                                                      \
        } while (false)

    #define ORCHESTRA_ASSERT_SIMPLE(condition)                                      \
        do                                                                         \
        {                                                                          \
            if (!(condition))                                                      \
            {                                                                      \
                const std::string _cond = #condition;                                \
                const std::string _bt  = ORCHESTRA_CAPTURE_BACKTRACE_();             \
                auto& _fn = ::ORchestra::AssertSink::Get();                          \
                if (_fn) _fn(_cond, std::string{}, _bt);                             \
                assert(condition);                                                  \
            }                                                                      \
        } while (false)
#else
    #define ORCHESTRA_ASSERT(condition, message) do { } while (false)
    #define ORCHESTRA_ASSERT_SIMPLE(condition) do { } while (false)
#endif
