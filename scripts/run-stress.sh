#!/bin/bash
set -e

# Usage: ./run-stress.sh [seconds] [mode] [harness_mode]
#   seconds:      length of each stress run (default: 20)
#   mode:         assert | tsan | both (default: both)
#   harness_mode: '' (default) or 'calm' (valid scripts only)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

DURATION="${1:-20}"
MODE="${2:-both}"
HARNESS_MODE="${3:-}"

find_binary() {
    find "$1" -name ORchestraStress -type f | head -n 1
}

run_assert_build() {
    local build_dir="${ROOT_DIR}/build-stress-assert"
    echo "=== Assert stress build ==="
    cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=OFF -DBUILD_STRESS=ON \
          -DSTRESS_ASSERTS=ON -DCMAKE_BUILD_TYPE=Debug \
          -B "${build_dir}" "${ROOT_DIR}"
    cmake --build "${build_dir}" --target ORchestraStress --parallel

    echo ""
    echo "Running assert stress (${DURATION}s${HARNESS_MODE:+, ${HARNESS_MODE}})..."
    if "$(find_binary "${build_dir}")" "${DURATION}" ${HARNESS_MODE}; then
        echo "=== Assert stress: completed, no assert fired ==="
    else
        echo "=== Assert stress: STOPPED (assert fired or crashed, see output above) ==="
    fi
}

run_tsan_build() {
    if ! echo 'int main(){return 0;}' | clang++ -fsanitize=thread -x c++ - -o /tmp/orchestra_tsan_probe 2>/dev/null; then
        echo "=== TSan not supported by this compiler/target, skipping ==="
        return 0
    fi

    local build_dir="${ROOT_DIR}/build-stress-tsan"
    echo "=== TSan stress build ==="
    cmake -DBUILD_PLUGIN=OFF -DBUILD_TESTS=OFF -DBUILD_STRESS=ON \
          -DSTRESS_ASSERTS=OFF -DENABLE_TSAN=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo \
          -B "${build_dir}" "${ROOT_DIR}"
    cmake --build "${build_dir}" --target ORchestraStress --parallel

    echo ""
    echo "Running TSan stress (${DURATION}s${HARNESS_MODE:+, ${HARNESS_MODE}})..."
    if TSAN_OPTIONS="halt_on_error=0" "$(find_binary "${build_dir}")" "${DURATION}" ${HARNESS_MODE}; then
        echo "=== TSan stress: completed, no races reported ==="
    else
        echo "=== TSan stress: STOPPED (races reported or crashed, see output above) ==="
    fi
}

case "${MODE}" in
    assert) run_assert_build ;;
    tsan)   run_tsan_build ;;
    both)   run_assert_build; echo ""; run_tsan_build ;;
    *)      echo "Unknown mode: ${MODE} (expected assert|tsan|both)"; exit 1 ;;
esac
