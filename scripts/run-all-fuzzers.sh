#!/bin/bash
# Builds and runs all fuzzers (with ASan + UBSan) and unit tests.
# No arguments needed. Exit code is non-zero if anything fails.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_FUZZ="${ROOT_DIR}/build-fuzz"
BUILD_TEST="${ROOT_DIR}/build-test"
CORPUS_DIR="${ROOT_DIR}/Fuzzing/corpus"
SEED="$(date +%s)"
ITERATIONS=100000

RED='\033[1;31m'
GREEN='\033[1;32m'
BOLD='\033[1m'
NC='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0

run_step() {
    local label="$1"
    shift

    local logfile
    logfile="$(mktemp)"

    if "$@" > "$logfile" 2>&1; then
        echo -e "  ${GREEN}[PASS]${NC} ${label}"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        local code=$?
        echo -e "  ${RED}[FAIL]${NC} ${label} (exit ${code})"
        tail -n 20 "$logfile"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi

    rm -f "$logfile"
}

echo -e "${BOLD}============================================${NC}"
echo -e "${BOLD} ORchestra - Full Fuzz + Test Suite${NC}"
echo -e "${BOLD}============================================${NC}"
echo "Seed:       ${SEED}"
echo "Iterations: ${ITERATIONS} per fuzzer"
echo "Corpus:     ${CORPUS_DIR}"
echo ""

# -- Build --
echo -e "${BOLD}[1/5] Building fuzzers (ASan + UBSan)...${NC}"
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=OFF -DBUILD_FUZZING=ON \
      -B "${BUILD_FUZZ}" "${ROOT_DIR}" > /dev/null 2>&1
cmake --build "${BUILD_FUZZ}" > /dev/null 2>&1
echo "  Done."

echo -e "${BOLD}[2/5] Building unit tests...${NC}"
cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=ON -DBUILD_FUZZING=OFF \
      -B "${BUILD_TEST}" "${ROOT_DIR}" > /dev/null 2>&1
cmake --build "${BUILD_TEST}" > /dev/null 2>&1
echo "  Done."
echo ""

# -- Run --
echo -e "${BOLD}[3/5] Unit tests${NC}"
run_step "Unit tests" "${BUILD_TEST}/UnitTests/ORchestraTests"

echo -e "${BOLD}[4/5] Scanner/Compiler fuzzer (${ITERATIONS} iterations)${NC}"
run_step "Scanner/Compiler fuzzer" \
    "${BUILD_FUZZ}/Fuzzing/ORchestraFuzz" "${SEED}" "${CORPUS_DIR}" "" "${ITERATIONS}"

echo -e "${BOLD}[5/5] VM fuzzer (${ITERATIONS} iterations)${NC}"
run_step "VM fuzzer" \
    "${BUILD_FUZZ}/Fuzzing/ORchestraFuzzVM" "${SEED}" "${CORPUS_DIR}" "" "${ITERATIONS}"

# -- Summary --
echo ""
echo -e "${BOLD}============================================${NC}"
echo -e "  Passed: ${GREEN}${PASS_COUNT}${NC}"
echo -e "  Failed: ${RED}${FAIL_COUNT}${NC}"
echo -e "${BOLD}============================================${NC}"

[ "${FAIL_COUNT}" -eq 0 ]
