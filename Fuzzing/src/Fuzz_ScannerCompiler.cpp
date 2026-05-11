#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <sstream>
#include <chrono>

#include "Scanner.h"
#include "Compiler.h"
#include "ErrorReporting.h"

using namespace ORchestra;

static void RunInput(const uint8_t* data, size_t size)
{
    if (size == 0)
        return;

    std::string input(reinterpret_cast<const char*>(data), size);

    // Suppress console noise from ErrorReporting during fuzzing
    std::ostringstream nullStream;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(nullStream.rdbuf());

    ErrorReporting logger;
    Scanner scanner(logger);

    if (scanner.ScanFile(input))
    {
        std::vector<Instruction> instructions;
        Compiler compiler(scanner.GetTokens(), logger);
        compiler.Compile(instructions);
    }

    std::cout.rdbuf(oldCoutBuffer);
}

#ifdef FUZZING_BUILD_LIBFUZZER
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    RunInput(data, size);
    return 0;
}
#else
int main(int argc, char** argv)
{
    unsigned int seed = static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count());
    size_t iterations = 100000;

    if (argc > 1)
        seed = static_cast<unsigned int>(std::stoul(argv[1]));
    if (argc > 2)
        iterations = std::stoull(argv[2]);

    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> lenDist(1, 4096);
    std::uniform_int_distribution<int> byteDist(0, 255);

    std::cout << "Standalone fuzzer seed: " << seed << "\n";
    std::cout << "Iterations: " << iterations << "\n";

    for (size_t i = 0; i < iterations; ++i)
    {
        int len = lenDist(rng);
        std::vector<uint8_t> data(static_cast<size_t>(len));
        for (int j = 0; j < len; ++j)
            data[j] = static_cast<uint8_t>(byteDist(rng));

        RunInput(data.data(), data.size());

        if ((i + 1) % 10000 == 0)
            std::cout << "Progress: " << (i + 1) << "/" << iterations << "\n";
    }

    std::cout << "Fuzzing completed successfully.\n";
    return 0;
}
#endif
