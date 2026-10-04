#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BIN="${ROOT_DIR}/build/apt-downgrade"
FIXTURE="${ROOT_DIR}/tests/fixtures/apt-list-sample.txt"
PASS=0
FAIL=0
OUT="/tmp/apt-downgrade-test-out.$$"
ERR="/tmp/apt-downgrade-test-err.$$"

if [[ ! -x "$BIN" ]]; then
  echo "Error: binario no encontrado. Ejecutá 'make build' antes."
  exit 1
fi

cleanup() {
  rm -f "$OUT" "$ERR"
}
trap cleanup EXIT

assert_exit() {
  local expected="$1"
  shift
  local actual=0
  set +e
  "$@" >"$OUT" 2>"$ERR"
  actual=$?
  set -e
  if [[ "$actual" -eq "$expected" ]]; then
    PASS=$((PASS + 1))
  else
    FAIL=$((FAIL + 1))
    echo "FAIL: esperado exit $expected, obtuvo $actual"
    echo "  cmd: $*"
    echo "  stdout:"; cat "$OUT"
    echo "  stderr:"; cat "$ERR"
  fi
}

assert_stdout_contains() {
  local needle="$1"
  if grep -Fq -- "$needle" "$OUT"; then
    PASS=$((PASS + 1))
  else
    FAIL=$((FAIL + 1))
    echo "FAIL: stdout no contiene: $needle"
    echo "  stdout:"; cat "$OUT"
  fi
}

assert_stdout_not_contains() {
  local needle="$1"
  if grep -Fq -- "$needle" "$OUT"; then
    FAIL=$((FAIL + 1))
    echo "FAIL: stdout no debería contener: $needle"
    echo "  stdout:"; cat "$OUT"
  else
    PASS=$((PASS + 1))
  fi
}

assert_stderr_contains() {
  local needle="$1"
  if grep -Fq -- "$needle" "$ERR"; then
    PASS=$((PASS + 1))
  else
    FAIL=$((FAIL + 1))
    echo "FAIL: stderr no contiene: $needle"
    echo "  stderr:"; cat "$ERR"
  fi
}

export APT_DOWNGRADE_LIST_CMD="cat ${FIXTURE}"

echo "== Tests de CLI =="

assert_exit 0 "$BIN" --help
assert_stdout_contains "Uso:"
assert_stdout_contains "--dry-run"

assert_exit 0 "$BIN" -h
assert_stdout_contains "Uso:"

assert_exit 1 "$BIN"
assert_stderr_contains "Uso:"

assert_exit 1 "$BIN" --current 1.0
assert_stderr_contains "Uso:"

assert_exit 1 "$BIN" --current 1.0 --unknown
assert_stderr_contains "Uso:"

echo "== Tests de parsing / dry-run =="

assert_exit 0 "$BIN" --current 1.2.3-1 --downgrade 1.0.0-1 --dry-run
assert_stdout_contains "Paquetes que coinciden"
assert_stdout_contains "demo-tool (noble)"
assert_stdout_contains "demo-lib (noble-updates,noble-security)"
assert_stdout_contains "sudo apt install"
assert_stdout_contains "demo-tool=1.0.0-1"
assert_stdout_contains "demo-lib=1.0.0-1"
assert_stdout_contains "Modo dry-run"
assert_stdout_contains "(2)"

assert_exit 0 "$BIN" --current 23.13.9-8ubuntu5.2 --downgrade 23.0.0 --dry-run
assert_stdout_contains "accountsservice (resolute-updates,resolute-security)"

assert_exit 0 "$BIN" --current 1:2.0.34-1ubuntu3 --downgrade 1:2.0.30-1 --dry-run
assert_stdout_contains "acpid (resolute)"

assert_exit 0 "$BIN" --current 0.0.0-not-installed --downgrade 0.0.0-1 --dry-run
assert_stdout_contains "No se encontraron paquetes"
assert_stdout_not_contains "Comando a ejecutar"

echo "== Tests de confirmación y privilegios =="

set +e
printf 'n\n' | env APT_DOWNGRADE_LIST_CMD="cat ${FIXTURE}" \
  "$BIN" --current 1.2.3-1 --downgrade 1.0.0-1 >"$OUT" 2>"$ERR"
actual=$?
set -e
if [[ "$actual" -eq 0 ]]; then
  PASS=$((PASS + 1))
else
  FAIL=$((FAIL + 1))
  echo "FAIL: cancelación esperaba exit 0, obtuvo $actual"
fi
assert_stdout_contains "Operación cancelada"

assert_exit 1 "$BIN" --current 1.2.3-1 --downgrade 1.0.0-1 --yes
assert_stderr_contains "requiere privilegios de root"

echo
echo "Resultado: ${PASS} ok, ${FAIL} fallos"
if [[ "$FAIL" -ne 0 ]]; then
  exit 1
fi
exit 0
