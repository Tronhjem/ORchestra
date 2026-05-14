#!/bin/bash
set -e

# Usage: ./run-fuzzer.sh [target] [seed] [corpus_dir] [crash_dir] [iterations]
#   target:     'scanner' or 'vm' (default: scanner)
#   seed:       Random seed for the fuzzer (default: current time)
#   corpus_dir: Path to corpus directory (default: Fuzzing/corpus)
#   crash_dir:  Path to crash output directory (default: <build>/crashes)
#   iterations: Number of fuzz iterations (default: 100000)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-fuzz"

TARGET="${1:-scanner}"
SEED="${2:-$(date +%s)}"
CORPUS_DIR="${3:-${SCRIPT_DIR}/Fuzzing/corpus}"
CRASH_DIR="${4:-}"
ITERATIONS="${5:-100000}"

if [ "$TARGET" = "vm" ]; then
    EXECUTABLE="ORchestraFuzzVM"
    echo "=== ORchestra VM Fuzzer ==="
else
    EXECUTABLE="ORchestraFuzz"
    echo "=== ORchestra Scanner/Compiler Fuzzer ==="
fi

echo "Build dir:  ${BUILD_DIR}"
echo "Seed:       ${SEED}"
echo "Corpus:     ${CORPUS_DIR}"
echo "Iterations: ${ITERATIONS}"
echo ""

echo "Configuring..."
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=OFF -DBUILD_FUZZING=ON -B "${BUILD_DIR}" "${SCRIPT_DIR}"

echo ""
echo "Building ${EXECUTABLE}..."
cmake --build "${BUILD_DIR}" --target "${EXECUTABLE}"

echo ""
echo "Running fuzzer..."
EXTRA_ARGS="$SEED $CORPUS_DIR"
if [ -n "$CRASH_DIR" ]; then
    EXTRA_ARGS="$EXTRA_ARGS $CRASH_DIR"
fi
EXTRA_ARGS="$EXTRA_ARGS $ITERATIONS"
"${BUILD_DIR}/Fuzzing/${EXECUTABLE}" ${EXTRA_ARGS}
