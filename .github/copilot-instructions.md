# ORchestra Copilot Instructions

## Project Overview

ORchestra is a JUCE-based MIDI sequencer plugin with a custom scripting language for generating euclidean rhythms and combining sequences using logical operations. The project compiles to AU/VST3 plugin formats and a standalone application.

## Build System

### Building the Plugin

```bash
# Quick setup (initialize JUCE submodule + configure)
./setup.sh

# Build everything (plugin + tests)
cd build
cmake --build .

# Build plugin only (requires JUCE)
cmake -DBUILD_PLUGIN=ON -DBUILD_TESTS=OFF ..
cmake --build .

# Build tests only (fast, no JUCE dependency)
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=ON ..
cmake --build .
```

### Running Tests

The test suite uses Catch2 and is independent from JUCE for fast iteration:

```bash
# Build and run all tests
cd build
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=ON ..
cmake --build .
./UnitTests/ORchestraTests

# Run with CTest
ctest --output-on-failure -V
```

Tests are organized by feature area in `UnitTests/include/Test_*.h` and all included in `UnitTests/src/Main.cpp`.

## Architecture

### Core Components

**Scanner → Compiler → VM → Engine**

1. **Scanner** (`Scanner.h/cpp`) - Tokenizes the scripting language input into `ORchestraToken` objects
2. **Compiler** (`Compiler.h/cpp`) - Compiles tokens into stack-based `Instruction` opcodes
3. **VM** (`VM.h/cpp`) - Stack-based virtual machine that executes instructions in two phases:
   - **Prepare phase**: Pre-processes script at `globalCount = 0`, creates runtime instructions
   - **Tick phase**: Executes runtime instructions at each step, populating `SequenceStep` data
4. **ORchestraEngine** (`ORchestraEngine.h/cpp`) - Main engine coordinating VM, MIDI scheduling, and transport
   - Runs VM in background worker thread
   - Ring buffer (`STEP_BUFFER_SIZE` = 4) for pre-processing upcoming steps
   - Thread-safe communication with audio thread via atomics

### Data Flow

```
Script Text → Scanner → Tokens → Compiler → OpCodes → VM.Prepare()
                                                           ↓
                                           Runtime Instructions → VM.Tick()
                                                                      ↓
                                                            SequenceStep[]
                                                                      ↓
                                            MidiScheduler → JUCE MidiBuffer
```

### Key Data Structures

- **DataSequence** (`DataSequence.h/cpp`) - Stores arrays of values with substep support
- **StepData** (`StepData.h/cpp`) - Wraps `DataUnit` (uint8_t) with substep array capability
- **SequenceStep** (`SequenceStep.h`) - MIDI output data (note/CC, channel, velocity/value)
- **Instruction** (`Instruction.h`) - OpCode enum + operands for VM execution
- **CustomStack** (`CustomStack.h`) - Fixed-size stack for VM operations

### Plugin Structure

- `PluginProcessor.cpp/.h` - JUCE audio plugin processor (handles MIDI output)
- `PluginEditor.cpp/.h` - UI with code editor and timeline visualization
- `ORchestraCodeEditorComponent` - Custom JUCE code editor with syntax highlighting
- `Timeline.cpp/.h` - Visual representation of trigger patterns
- `MidiScheduler.cpp/.h` - Converts `SequenceStep` data to timed MIDI messages

## Key Conventions

### Scripting Language Execution Model

- **Two-phase execution**: Scripts are processed in `Prepare` (preprocessing at global step 0) and `Tick` (runtime at each step)
- **Global count variable (`$`)**: Special variable that evaluates to 0 during Prepare, current step during Tick
- **Sequence wrapping**: All array accesses wrap around using modulo of array length
- **Substeps**: Nested arrays subdivide steps (max 6 subdivisions per step)
- **8-bit values**: All values are `DataUnit` (uint8_t), clamped to 0-127 for MIDI

### Code Organization

- **ORchestra engine code**: `ORchestra/src/ORchestraEngine/` (no JUCE dependency)
- **JUCE-specific code**: `ORchestra/src/juceFiles/` (plugin UI, processor)
- **Utility headers**: `ORchestra/src/utils/` (defines, types, helpers)
- **Test files**: `UnitTests/include/Test_*.h` (Catch2-based tests)

### Memory Management

- VM uses fixed-size `CustomStack` (256 elements) to avoid dynamic allocation in audio thread
- `mStepRingBuffer` in ORchestraEngine pre-processes steps ahead of playback
- Worker thread pattern: audio thread reads, worker thread writes to ring buffer

### Error Handling

- `ErrorReporting` class collects compilation/runtime errors
- Errors stored as `LogEntry` with line/column info
- Access via `VM::GetErrors()` or `ORchestraEngine::GetErrors()`
- Test mode enabled with `_TEST=1` and `_DEBUG=1` compile definitions

### Reserved Keywords

Cannot be used as variable names: `note`, `cc`, `ran`, `euc`, `print`, `test`

### MIDI Output

- `note(trigger, noteValue, velocity, channel)` - Note on/off messages
- `cc(trigger, ccNumber, ccValue, channel)` - Control Change messages
- Trigger parameter: value > 0 sends MIDI, value == 0 skips

## Building for Different Platforms

The project supports Windows, macOS (Intel & Apple Silicon), and Ubuntu via GitHub Actions:

- **macOS**: Builds AU, VST3, and Standalone
- **Windows**: Builds VST3 and Standalone
- **Ubuntu**: Tests only (no plugin build in CI)

## Testing Strategy

- Unit tests focus on engine logic (Scanner, Compiler, VM, DataSequence)
- Tests are independent from JUCE for fast iteration
- Test data uses simple scripts to verify language features
- See `Test_*.h` files for examples of testing specific features

## Common Tasks

### Adding a New Language Feature

1. Add token type to `ORchestraTokenType` enum (`ORchestraToken.h`)
2. Update `Scanner` to recognize new token
3. Add compilation logic in `Compiler`
4. Add opcode to `OpCode` enum (`Instruction.h`)
5. Implement execution in `VM::ProcessInstruction()`
6. Add tests in `UnitTests/include/`

### Debugging Script Execution

- Use `_DEBUG=1` compile flag to enable debug features
- `print` and `test` keywords available in debug builds
- Check `ErrorReporting` log entries for compilation errors
- VM stores `mTopStackValue` in test builds for inspection

## File Import/Export

- `FileLoader` (`FileLoader.h/cpp`) handles reading/writing script files
- Files saved to/loaded from `InstructionFiles/` directory by default
- `ORchestraEngine::ImportFromFile()` / `ExportToFile()` for persistence
