#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
CONFIG="Debug"
SIGN=0
[[ "${1:-}" == "--sign" ]] && SIGN=1

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

if [[ "$SIGN" -eq 1 ]]; then
    if [[ "$(uname)" != "Darwin" ]]; then
        echo "Error: --sign is only supported on macOS."
        exit 1
    fi
    IDENTITY="${MACOS_SIGNING_IDENTITY:-}"
    if [[ -z "$IDENTITY" ]]; then
        IDENTITY="$(security find-identity -v -p codesigning 2>/dev/null \
            | grep -o 'Developer ID Application: [^"]*' | head -n 1)"
    fi
    if [[ -z "$IDENTITY" ]]; then
        echo "Error: no 'Developer ID Application' identity found in Keychain."
        echo "       Set MACOS_SIGNING_IDENTITY or add your cert via Xcode."
        exit 1
    fi

    echo ""
    echo "Signing with identity: $IDENTITY"
    for BUNDLE in \
        "${BUILD_DIR}/ORchestra/ORchestra_AU_artefacts/${CONFIG}/AU/ORchestra.component" \
        "${BUILD_DIR}/ORchestra/ORchestra_VST_artefacts/${CONFIG}/VST3/ORchestra.vst3" \
        "${BUILD_DIR}/ORchestra/ORchestra_Standalone_artefacts/${CONFIG}/Standalone/ORchestra.app"
    do
        codesign --force --timestamp --sign "$IDENTITY" "$BUNDLE"
        codesign --verify --deep --strict --verbose=2 "$BUNDLE"
    done
    echo "Signed and verified."
fi

echo ""
echo "=== Build complete ==="
echo "AU plugin:    ${BUILD_DIR}/ORchestra/ORchestra_AU_artefacts/${CONFIG}/AU/ORchestra.component"
echo "VST3 plugin:  ${BUILD_DIR}/ORchestra/ORchestra_VST_artefacts/${CONFIG}/VST3/ORchestra.vst3"
echo "Standalone:   ${BUILD_DIR}/ORchestra/ORchestra_Standalone_artefacts/${CONFIG}/Standalone/ORchestra.app"
echo "Tests:        ${BUILD_DIR}/Tests/UnitTests/ORchestraTests"
