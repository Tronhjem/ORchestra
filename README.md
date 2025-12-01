# ORchestra

[![Build](https://github.com/Tronhjem/ORchestra/actions/workflows/Build.yml/badge.svg)](https://github.com/Tronhjem/ORchestra/actions/workflows/Build.yml)
[![Run Tests](https://github.com/Tronhjem/ORchestra/actions/workflows/RunTests.yml/badge.svg)](https://github.com/Tronhjem/ORchestra/actions/workflows/RunTests.yml)

> **Note:** Project is still work in progress.

## Overview

ORchestra is a powerful MIDI sequencer plugin that generates and combines sequences using euclidean algorithms or manual input. It features a custom scripting language for creating complex rhythmic patterns through logical operations.

### Key Features

- **Euclidean Rhythm Generation**: Create rhythmic patterns using the euclidean algorithm
- **Sequence Combination**: Use logic operations (`&`, `^`, `|`) to combine sequences
- **Phasing Patterns**: Sequences of different lengths phase and evolve over time
- **Custom Scripting Language**: Powerful yet simple syntax for defining musical patterns
- **MIDI Output**: Generates MIDI notes and CC messages
- **Mathematical Operations**: Full arithmetic support with standard precedence
- **Comparison Operators**: Compare values and create conditional patterns

Original prototype that sparked the idea can be found here: <https://github.com/Tronhjem/EuclidsCombinator>

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Build Instructions](#build-instructions)
- [Running Tests](#running-tests)
- [Syntax and Language Reference](#syntax-and-language-reference)
  - [Operators](#operators)
  - [Variables](#variables)
  - [Reserved Keywords](#reserved-keywords)
  - [Data Sequences](#data-sequences)
  - [Note Values](#note-values)
  - [Tracks](#tracks)
  - [Built-in Functions](#built-in-functions)
- [Examples](#examples)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

Before building ORchestra, ensure you have the following installed:

- **CMake** (version 3.22 or higher)
- **C++ Compiler** with C++17 support (GCC, Clang, or MSVC)
- **Git** (for cloning and managing submodules)
- **JUCE Framework** (automatically fetched as a git submodule)

For Ubuntu/Debian:
```bash
sudo apt-get install cmake build-essential git
```

For macOS:
```bash
brew install cmake git
```

---

## Quick Start

The fastest way to get started is using the provided setup script:

```bash
./setup.sh
```

This will:
1. Initialize and update the JUCE submodule
2. Create a build directory
3. Configure CMake with default settings

Then build the project:
```bash
cd build
cmake --build .
```

---

## Build Instructions

### Manual Build Process

**Step 1: Clone and Initialize Submodules**

```bash
git clone https://github.com/Tronhjem/ORchestra.git
cd ORchestra
git submodule update --init --recursive
```

**Step 2: Create Build Directory**

```bash
mkdir build
cd build
```

### Build Options

The CMake build supports separate compilation of the plugin and tests for faster development:

**Build tests only (no JUCE dependency, faster):**
```bash
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=ON ..
cmake --build .
./UnitTests/ORchestraTests
```

**Build plugin only (requires JUCE):**
```bash
cmake -DBUILD_PLUGIN=ON -DBUILD_TESTS=OFF ..
cmake --build .
```

**Build both (default):**
```bash
cmake ..
cmake --build .
```

---

## Running Tests

Tests use the Catch2 framework and can be built independently from the JUCE plugin.
This allows for quick test iterations without compiling the entire JUCE framework.

```bash
cd build
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=ON ..
cmake --build .
./UnitTests/ORchestraTests
```

---

## Syntax and Language Reference

The ORchestra scripting language is evaluated from top to bottom.
You must declare variables before using them later in the script.

**Best Practice:** Define data sequences first, then create tracks that use them.

### General Rules

- Each new line is a new instruction (uses `\n` as delimiter)
- All whitespace within a line is ignored
- Use `//` for single-line comments
- All values are 8-bit integers, limited to 0-127 for MIDI compatibility

### Operators

**Arithmetic Operators**

`+` `-` `*` `/` operate on numbers with standard mathematical precedence.
Parentheses can be used to override precedence, just like in regular programming.
All arithmetic operators take precedence over logical and comparison operators.

**Logical Operators**

`|` `^` `&` evaluate logic operations on triggers (values > 0 are true).
These operators always return 0 (false) or 1 (true).

- `|` - OR operation
- `^` - XOR operation  
- `&` - AND operation

**Comparison Operators**

`>` `<` `>=` `<=` `==` `!=` compare two values and return 0 (false) or 1 (true).

### Variables

The `=` operator declares and assigns a variable.

**Declaration Syntax:**
- Variable names can be any length
- Must start with an alphabetic character or `_`
- Cannot start with a number

**Assignment Examples:**

**Simple value:**
```cpp
a = 64
```

**Expressions:**
```cpp
a = 64 + 2
c = a | 1
z = a * (2 + 4)
```

**Data sequence:**
```cpp
a = [127, 0, 64]
```

**Variable reference:**

References another variable. If it's a sequence, it evaluates at the current global step.
```cpp
c = [64, 64, 64]
a = c  // References the sequence c
```

**Array indexing:**

Access specific values in a data sequence (zero-indexed):
```cpp
c = [64, 65, 70]
a = c[0]  // value is 64
x = c[1]  // value is 65
```

**Boolean operations:**

Note that logical and comparison operators always return 0 or 1:
```cpp
a = [64, 63]
c = a[0] > 0  // value is 1

z = [1, 0]
x = [0, 1]
y = z[0] & x[0]  // value is 0 (AND operation: 1 & 0 = 0)
```

### Reserved Keywords

The following words are reserved and cannot be used as variable names:

- `note` - Creates a MIDI note track
- `cc` - Creates a MIDI control change track
- `ran` - Random number generator function
- `euc` - Euclidean sequence generator function

**Note:** The keywords `print` and `test` are reserved for debugging purposes but are only available in debug builds.

### Data Sequences

Data sequences are arrays of numeric values used to create musical patterns.

**Key Points:**
- Values can be used anywhere - they're always just numbers
- For triggers (first argument of `note` or `cc`), values > 0 are treated as true
- For notes, velocities, and CC values, the values are sent as defined
- Sequences are defined using C-style array syntax with `[]` and comma separators
- Each index represents a value to be used in a step

**Example:**
```cpp
a = [64, 64, 65]  // Simple 3-step sequence
```

### Note Values

Notes can be represented in two ways:

**1. Raw MIDI values (0-127):**
```cpp
a = [60, 62, 64]  // C4, D4, E4
```

**2. Musical notation:**
- Capital letter for the note (C, D, E, F, G, A, B)
- Optional `#` (sharp) or `b` (flat)
- Octave number (0-10)

**Example:**
```cpp
a = [C4, C#4, Db2]
```

When compiled, note names are converted to MIDI values, allowing them to be combined with other values and used in expressions.

### Tracks

Tracks output MIDI messages and are created using the `note` or `cc` keywords.

**Important:**
- Tracks are not variables - they execute immediately
- Both require exactly 4 arguments
- Arguments can be values, expressions, or variables
- The trigger argument checks if value > 0 to determine when to send MIDI

**Note Track Syntax:**
```cpp
note(trigger, note, velocity, channel)
```

**CC Track Syntax:**
```cpp
cc(trigger, controlNumber, controlValue, channel)
```

**Example:**
```cpp
a = [1, 0, 1, 0]      // Trigger pattern
b = [64, 64, 65, 67]  // Note sequence

note(a, b, 100, 1)    // Trigger: a, Notes: b, Velocity: 100, Channel: 1
```

### Built-in Functions

#### Euclidean Sequence Generator

The `euc(hits, length)` function generates euclidean rhythm patterns.

**Syntax:**
```cpp
euc(hits, length)
```

**Parameters:**
- `hits` - Number of beats to distribute
- `length` - Total length of the sequence

**Returns:** A data sequence containing 0s and 1s (designed for triggers)

**Note:** Can only be used for variable assignment, not as a direct parameter.

**Example:**
```cpp
// Euclidean sequence with 4 hits divided across 8 steps
a = euc(4, 8)
note(a, 64, 100, 1)  // Use the euclidean pattern as a trigger
```

#### Random Number Generator

The `ran(low, high)` function generates random values at runtime.

**Syntax:**
```cpp
ran(low, high)
```

**Parameters:**
- `low` - Minimum value (inclusive)
- `high` - Maximum value (inclusive)

**Returns:** A random integer between low and high

**Note:** Evaluated at every tick, providing new random values each time.

**Example:**
```cpp
vel = ran(50, 100)   // Random velocity between 50-100
note(1, 64, vel, 1)  // Play C4 with random velocity
```

---

## Examples

### Basic Pattern
```cpp
// Simple kick drum pattern
kick = [1, 0, 0, 0]
note(kick, 36, 100, 1)  // C1 on MIDI channel 1
```

### Euclidean Rhythm
```cpp
// Create a euclidean pattern
pattern = euc(5, 8)
note(pattern, C4, 100, 1)
```

### Combining Sequences with Logic
```cpp
// Create two patterns
a = euc(3, 8)
b = euc(5, 8)

// Combine with XOR - triggers when only one is active
combined = a ^ b
note(combined, D4, 100, 1)
```

### Phasing Patterns
```cpp
// Different length sequences phase over time
pattern1 = euc(3, 8)
pattern2 = euc(5, 13)

// Combine with AND - both must be active
both = pattern1 & pattern2
note(both, E4, 120, 1)
```

### Random Velocity and Notes
```cpp
// Random velocity for each triggered note
trigger = euc(4, 8)
velocity = ran(80, 127)
note(trigger, C4, velocity, 1)

// Random note selection
notes = [C4, D4, E4, G4, A4]
randomNote = notes[ran(0, 4)]
note(1, randomNote, 100, 1)
```

### Using CC Messages
```cpp
// Control filter cutoff with sequence
cutoff = [64, 80, 100, 120]
cc(1, 74, cutoff, 1)  // Always trigger, CC#74 (filter cutoff)
```

### Complex Rhythm
```cpp
// Kick on 1 and 3
kick = [1, 0, 1, 0]
// Snare on 2 and 4
snare = [0, 1, 0, 1]
// Hi-hat euclidean pattern
hihat = euc(7, 8)

note(kick, 36, 100, 10)   // Kick on channel 10
note(snare, 38, 100, 10)  // Snare on channel 10
note(hihat, 42, 80, 10)   // Hi-hat on channel 10
```

---

## Troubleshooting

### Build Issues

**Problem:** `JUCE not found` error
```
Solution: Ensure you've initialized the submodule:
git submodule update --init --recursive
```

**Problem:** CMake version too old
```
Solution: Install CMake 3.22 or higher:
- Ubuntu: sudo apt-get install cmake
- macOS: brew install cmake
```

**Problem:** Build fails with missing compiler
```
Solution: Install build essentials:
- Ubuntu: sudo apt-get install build-essential
- macOS: Install Xcode Command Line Tools
```

### Runtime Issues

**Problem:** Reserved keyword error
```
Solution: Check that you're not using: note, cc, ran, euc, print, or test as variable names
```

**Problem:** Note name parsing error
```
Solution: Ensure note names follow the format: [A-G][#/b]?[0-10]
Examples: C4, F#5, Db3
```

**Problem:** Index out of bounds
```
Solution: Verify array indices are within range (0 to array length - 1)
```

### Getting Help

- Open an issue on [GitHub](https://github.com/Tronhjem/ORchestra/issues)
- Check existing issues for similar problems
- Include your ORchestra script and error messages when reporting bugs

---

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
