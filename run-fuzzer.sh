#!/bin/bash
set -e

# Usage: ./run-fuzzer.sh [seed] [iterations]
#   seed:       Random seed for the fuzzer (default: current time)
#   iterations: Number of fuzz iterations (default: 100000)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-fuzz"
SEED="${1:-$(date +%s)}"
ITERATIONS="${2:-100000}"

echo "=== ORchestra Fuzzer ==="
echo "Build dir:  ${BUILD_DIR}"
echo "Seed:       ${SEED}"
echo "Iterations: ${ITERATIONS}"
echo ""

echo "Configuring..."
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=OFF -DBUILD_FUZZING=ON -B "${BUILD_DIR}" "${SCRIPT_DIR}"

echo ""
echo "Building..."
cmake --build "${BUILD_DIR}" --target ORchestraFuzz

echo ""
echo "Running fuzzer..."
"${BUILD_DIR}/Fuzzing/ORchestraFuzz" "${SEED}" "${ITERATIONS}"
