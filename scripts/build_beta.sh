#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-beta"

echo "=== ORchestra Beta Build (Release + logging, no debug features) ==="
echo "Root: ${ROOT_DIR}"
echo "Build dir: ${BUILD_DIR}"
echo ""

echo "Cleaning build folder..."
rm -rf "${BUILD_DIR}"

echo "Configuring (Release + ORCHESTRA_ENABLE_LOGGING)..."
cmake \
    -DBUILD_PLUGIN=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_FUZZING=OFF \
    -DORCHESTRA_ENABLE_LOGGING=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -B "${BUILD_DIR}" \
    "${ROOT_DIR}"

echo ""
echo "Building all targets..."
cmake --build "${BUILD_DIR}" --parallel

echo ""
echo "=== Build complete ==="
echo "AU plugin:    ${BUILD_DIR}/ORchestra/ORchestra_artefacts/Release/AU/ORchestra.component"
echo "VST3 plugin:  ${BUILD_DIR}/ORchestra/ORchestra_artefacts/Release/VST3/ORchestra.vst3"
echo "Standalone:   ${BUILD_DIR}/ORchestra/ORchestra_Standalone_artefacts/Release/Standalone/ORchestra.app"
echo "Tests:        ${BUILD_DIR}/Tests/UnitTests/ORchestraTests"
