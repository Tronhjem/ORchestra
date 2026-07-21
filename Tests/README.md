# Testing

All test code lives in this folder.

## Unit Tests

Catch2-based tests of the core engine. No JUCE dependency.

```bash
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=ON -B build
cmake --build build
./build/Tests/UnitTests/ORchestraTests

# or
ctest --test-dir build --output-on-failure
```

Tests are in `Tests/UnitTests/src/Test_*.h` and included by `Tests/UnitTests/src/Main.cpp`.

## Fuzzing

Two fuzz targets covering the engine with random input.

- **ORchestraFuzz** — Scanner and Compiler only.
- **ORchestraFuzzVM** — End-to-end: scan, compile, VM::Prepare, and VM::Tick.

```bash
# Scanner/Compiler fuzzer
./scripts/run-fuzzer.sh scanner
./scripts/run-fuzzer.sh scanner 42 500000

# VM fuzzer (uses corpus seeds)
./scripts/run-fuzzer.sh vm
./scripts/run-fuzzer.sh vm 42 Tests/Fuzzing/corpus
```

Manual build:

```bash
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=OFF -DBUILD_FUZZING=ON -B build-fuzz
cmake --build build-fuzz --target ORchestraFuzz --target ORchestraFuzzVM

./build-fuzz/Tests/Fuzzing/ORchestraFuzz 42 500000
./build-fuzz/Tests/Fuzzing/ORchestraFuzzVM 42 Tests/Fuzzing/corpus
```

The VM fuzzer writes crash reproducers to `build-fuzz/Tests/Fuzzing/crashes/`.

## Stress Harness

A 3-thread race detector for the engine: audio (`Tick`), compile (`Compile`), and UI (`GetStepDataSlotCopy` / `GetErrors`).

```bash
./scripts/run-stress.sh 30 both
./scripts/run-stress.sh 60 tsan calm
```

Modes:
- `assert` — `_DEBUG` tripwires + widened race windows. Aborts on catch.
- `tsan` — ThreadSanitizer build; reports races without relying on timing.
- `calm` — valid scripts only, so deeper races surface under TSan.

Manual build:

```bash
# Assert build
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=OFF -DBUILD_STRESS=ON \
      -DSTRESS_ASSERTS=ON -DCMAKE_BUILD_TYPE=Debug -B build-stress-assert
cmake --build build-stress-assert --target ORchestraStress

# TSan build
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=OFF -DBUILD_STRESS=ON \
      -DSTRESS_ASSERTS=OFF -DENABLE_TSAN=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -B build-stress-tsan
cmake --build build-stress-tsan --target ORchestraStress
```
