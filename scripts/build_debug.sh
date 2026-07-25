#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"

echo "=== ORchestra Debug Build ==="
echo "Root: ${ROOT_DIR}"
echo "Build dir: ${BUILD_DIR}"
echo ""

echo "Cleaning build folder..."
rm -rf "${BUILD_DIR}"

echo "Configuring (Debug)..."
cmake \
    -DBUILD_PLUGIN=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_FUZZING=OFF \
    -DCMAKE_BUILD_TYPE=Debug \
    -B "${BUILD_DIR}" \
    "${ROOT_DIR}"

echo ""
echo "Building all targets..."
cmake --build "${BUILD_DIR}" --parallel

echo ""
echo "=== Build complete ==="
echo "AU plugin:    ${BUILD_DIR}/ORchestra/ORchestra_artefacts/Debug/AU/ORchestra.component"
echo "VST3 plugin:  ${BUILD_DIR}/ORchestra/ORchestra_artefacts/Debug/VST3/ORchestra.vst3"
echo "Standalone:   ${BUILD_DIR}/ORchestra/ORchestra_Standalone_artefacts/Debug/Standalone/ORchestra.app"
echo "Tests:        ${BUILD_DIR}/Tests/UnitTests/ORchestraTests"
