#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <exception>

#include "Scanner.h"
#include "Compiler.h"
#include "ErrorReporting.h"

using namespace ORchestra;

static std::filesystem::path gCrashDir;
static std::filesystem::path gLastInputPath;
static size_t gCrashIndex = 0;

static void SaveInput(const uint8_t* data, size_t size, const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
}

static void SaveCrashInput(const uint8_t* data, size_t size)
{
    if (!std::filesystem::exists(gCrashDir))
        std::filesystem::create_directories(gCrashDir);

    std::string filename = "crash_" + std::to_string(gCrashIndex++) + ".txt";
    SaveInput(data, size, gCrashDir / filename);
    std::cerr << "CRASH INPUT SAVED: " << (gCrashDir / filename).string() << "\n";
}

static void RunInput(const uint8_t* data, size_t size)
{
    if (size == 0)
        return;

    std::string input(reinterpret_cast<const char*>(data), size);

    std::ostringstream nullStream;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(nullStream.rdbuf());

    try
    {
        ErrorReporting logger;
        Scanner scanner(logger);

        if (scanner.ScanFile(input))
        {
            std::vector<Instruction> instructions;
            Compiler compiler(scanner.GetTokens(), logger);
            compiler.Compile(instructions);
        }
    }
    catch (const std::exception& e)
    {
        std::cout.rdbuf(oldCoutBuffer);
        std::cerr << "EXCEPTION: " << e.what() << "\n";
        SaveCrashInput(data, size);
        return;
    }

    std::cout.rdbuf(oldCoutBuffer);
}

static void RunWithCorpus(const std::string& corpusDir, std::mt19937& rng, size_t iterations)
{
    std::vector<std::string> seeds;
    if (std::filesystem::exists(corpusDir))
    {
        for (const auto& entry : std::filesystem::directory_iterator(corpusDir))
        {
            if (entry.is_regular_file())
            {
                std::ifstream file(entry.path(), std::ios::binary);
                std::string content((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
                if (!content.empty())
                    seeds.push_back(std::move(content));
            }
        }
    }

    std::uniform_int_distribution<size_t> seedDist(0, seeds.empty() ? 0 : seeds.size() - 1);
    std::uniform_int_distribution<int> byteDist(0, 255);
    std::uniform_int_distribution<int> actionDist(0, 3);

    auto mutate = [&](const std::string& base) -> std::vector<uint8_t> {
        std::vector<uint8_t> result(base.begin(), base.end());
        if (result.empty())
            return result;

        int mutations = 1 + (static_cast<int>(rng()) % 5);
        for (int m = 0; m < mutations; ++m)
        {
            int action = actionDist(rng);
            size_t pos = rng() % result.size();

            switch (action)
            {
                case 0: // bit flip
                    result[pos] ^= static_cast<uint8_t>(1 << (rng() % 8));
                    break;
                case 1: // byte replacement
                    result[pos] = static_cast<uint8_t>(byteDist(rng));
                    break;
                case 2: // byte insertion
                    result.insert(result.begin() + static_cast<std::ptrdiff_t>(pos), static_cast<uint8_t>(byteDist(rng)));
                    break;
                case 3: // byte deletion
                    if (result.size() > 1)
                        result.erase(result.begin() + static_cast<std::ptrdiff_t>(pos));
                    break;
            }
        }
        return result;
    };

    std::cout << "Corpus seeds loaded: " << seeds.size() << "\n";

    for (size_t i = 0; i < iterations; ++i)
    {
        if (seeds.empty())
            break;

        auto input = mutate(seeds[seedDist(rng)]);
        SaveInput(input.data(), input.size(), gLastInputPath);
        RunInput(input.data(), input.size());

        if ((i + 1) % 10000 == 0)
            std::cout << "Scanner fuzz progress: " << (i + 1) << "/" << iterations << "\n";
    }
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

    if (argc > 1)
        seed = static_cast<unsigned int>(std::stoul(argv[1]));

    std::mt19937 rng(seed);
    std::cout << "Scanner/Compiler fuzzer seed: " << seed << "\n";

    gCrashDir = std::filesystem::path(argv[0]).parent_path() / "crashes";
    if (argc > 3)
        gCrashDir = argv[3];

    gLastInputPath = std::filesystem::path(argv[0]).parent_path() / "_last_input.txt";

    std::string corpusDir = std::filesystem::path(argv[0]).parent_path() / "corpus";
    if (argc > 2)
        corpusDir = argv[2];

    size_t iterations = 100000;
    if (argc > 4)
        iterations = std::stoull(argv[4]);

    if (std::filesystem::exists(corpusDir))
    {
        RunWithCorpus(corpusDir, rng, iterations);
    }
    else
    {
        std::cout << "No corpus found at " << corpusDir << ", falling back to random bytes.\n";
        std::uniform_int_distribution<int> lenDist(1, 4096);
        std::uniform_int_distribution<int> byteDist(0, 255);

        for (size_t i = 0; i < iterations; ++i)
        {
            int len = lenDist(rng);
            std::vector<uint8_t> data(static_cast<size_t>(len));
            for (int j = 0; j < len; ++j)
                data[j] = static_cast<uint8_t>(byteDist(rng));

            SaveInput(data.data(), data.size(), gLastInputPath);
            RunInput(data.data(), data.size());

            if ((i + 1) % 10000 == 0)
                std::cout << "Scanner fuzz progress: " << (i + 1) << "/" << iterations << "\n";
        }
    }

    std::cout << "Scanner/Compiler fuzzing completed successfully.\n";
    return 0;
}
#endif
