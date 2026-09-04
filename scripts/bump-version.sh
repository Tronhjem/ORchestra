#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
CMAKE_FILE="$ROOT_DIR/CMakeLists.txt"

usage() {
    echo "Usage: $0 <major|minor|patch>"
    exit 1
}

[[ $# -eq 1 ]] || usage

# Refuse to run with uncommitted changes so the bump commit only
# contains the version edit and not unrelated work.
if ! git -C "$ROOT_DIR" diff --quiet || ! git -C "$ROOT_DIR" diff --cached --quiet; then
    echo "Error: working tree has uncommitted changes. Commit or stash them first."
    exit 1
fi

BUMP_TYPE="$1"

# Extract current version from root CMakeLists.txt
CURRENT=$(sed -n 's/^project(ORCHESTRA VERSION \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\)).*/\1/p' "$CMAKE_FILE")

if [[ -z "$CURRENT" ]]; then
    echo "Error: could not find version in $CMAKE_FILE"
    exit 1
fi

IFS='.' read -r MAJOR MINOR PATCH <<< "$CURRENT"

case "$BUMP_TYPE" in
    major) MAJOR=$((MAJOR + 1)); MINOR=0; PATCH=0 ;;
    minor) MINOR=$((MINOR + 1)); PATCH=0 ;;
    patch) PATCH=$((PATCH + 1)) ;;
    *) usage ;;
esac

NEW_VERSION="$MAJOR.$MINOR.$PATCH"

echo "Bumping version: $CURRENT -> $NEW_VERSION"

if git -C "$ROOT_DIR" rev-parse "v$NEW_VERSION" >/dev/null 2>&1; then
    echo "Error: tag v$NEW_VERSION already exists."
    exit 1
fi

# Update CMakeLists.txt
sed -i '' "s/project(ORCHESTRA VERSION $CURRENT)/project(ORCHESTRA VERSION $NEW_VERSION)/" "$CMAKE_FILE"

# Commit, tag, and push to trigger the release workflow.
cd "$ROOT_DIR"
git add CMakeLists.txt
git commit -m "Bump version to $NEW_VERSION"
git tag "v$NEW_VERSION"
git push origin HEAD
git push origin "v$NEW_VERSION"

echo "Done. Pushed v$NEW_VERSION. GitHub Actions will build the release draft."
