#!/usr/bin/env bash
# Validate packaging metadata consistency (used by CI and locally).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

fail=0

makefile_var() {
  sed -n "s/^${1}[[:space:]]*=[[:space:]]*//p" Makefile | head -1 | tr -d '[:space:]'
}

VERSION="$(makefile_var VERSION)"
DEB_VERSION="$(makefile_var DEB_VERSION)"
FULL="${VERSION}-${DEB_VERSION}"

echo "Makefile VERSION=${VERSION} DEB_VERSION=${DEB_VERSION}"

if [[ ! "$VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
  echo "FAIL: VERSION must look like X.Y.Z"
  fail=1
fi

changelog_version="$(sed -n '1s/^[^ ]* (\([^)]*\)).*/\1/p' debian/changelog)"
echo "debian/changelog version=${changelog_version}"

# Changelog may be FULL or FULL~series1; must start with FULL
if [[ "$changelog_version" != "$FULL" && "$changelog_version" != "${FULL}~"* ]]; then
  echo "FAIL: changelog version '${changelog_version}' must be '${FULL}' or '${FULL}~<series>1'"
  fail=1
fi

if ! grep -q "Maintainer: alanjmrt94 <alanjmartinez94@gmail.com>" debian/control; then
  echo "FAIL: unexpected Maintainer in debian/control"
  fail=1
fi

if ! grep -q "Build-Depends:.*debhelper" debian/control; then
  echo "FAIL: missing debhelper in Build-Depends"
  fail=1
fi

if [[ ! -f LICENSE ]]; then
  echo "FAIL: LICENSE missing"
  fail=1
fi

if ! grep -qi "MIT License" LICENSE; then
  echo "FAIL: LICENSE does not look like MIT"
  fail=1
fi

# Misma lectura que usa el script de PPA cuando no hay VERSION en el entorno
# shellcheck disable=SC2016 # comillas simples a propósito: expandir dentro del bash -c hijo
script_version="$(
  env -u VERSION -u DEB_VERSION bash -c '
    makefile_var() { sed -n "s/^${1}[[:space:]]*=[[:space:]]*//p" Makefile | head -1 | tr -d "[:space:]"; }
    v="$(makefile_var VERSION)"
    d="$(makefile_var DEB_VERSION)"
    printf "%s-%s\n" "$v" "$d"
  '
)"
if [[ "$script_version" != "$FULL" ]]; then
  echo "FAIL: script/Makefile version mismatch (${script_version} vs ${FULL})"
  fail=1
fi

if [[ "$fail" -ne 0 ]]; then
  echo "Metadata checks failed"
  exit 1
fi

echo "Metadata OK"
