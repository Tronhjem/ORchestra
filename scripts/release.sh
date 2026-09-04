#!/usr/bin/env bash
# Re-publish a GitHub Release for an existing tag without bumping the version.
#
# Deletes any existing (draft) release for the tag so the re-uploaded assets
# don't collide, then triggers the Release workflow for that tag via
# workflow_dispatch.
#
# Usage:
#   ./scripts/release.sh            # uses the latest tag (git describe)
#   ./scripts/release.sh v0.2.5     # use an explicit tag
#
# Requires the GitHub CLI (https://cli.github.com) and an authenticated session.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT_DIR"

command -v gh >/dev/null 2>&1 || {
    echo "Error: github.com CLI 'gh' is not installed or not on PATH."
    exit 1
}

# Resolve the tag: explicit argument, otherwise the latest reachable tag.
if [[ $# -ge 1 ]]; then
    TAG="$1"
else
    TAG="$(git describe --tags --abbrev=0)"
fi

# Validate the tag format and that it exists.
if [[ ! "$TAG" =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Error: '$TAG' does not look like a version tag (vX.Y.Z)."
    exit 1
fi
if ! git rev-parse "$TAG" >/dev/null 2>&1; then
    echo "Error: tag '$TAG' does not exist in this repository."
    exit 1
fi

echo "Re-publishing release for tag: $TAG"

# Best-effort delete of any existing release (draft or published) so the
# re-uploaded assets don't collide. The git tag itself is left untouched.
if gh release view "$TAG" >/dev/null 2>&1; then
    gh release delete "$TAG" --yes --cleanup-tag=false
    echo "Deleted existing release for $TAG"
else
    echo "No existing release for $TAG (ok)"
fi

# Trigger the release workflow for this tag.
gh workflow run release.yml --ref "$TAG" -f tag="$TAG"

echo "Triggered release.yml for $TAG."
echo "Watch progress with: gh run watch"
