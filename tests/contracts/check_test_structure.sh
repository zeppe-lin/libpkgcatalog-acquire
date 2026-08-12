#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "test-structure: $*" >&2; exit 1; }
for directory in unit integration mechanism cli header installed support contracts; do
  [ -d "$root/tests/$directory" ] || fail "missing tests/$directory"
done
for legacy in acquire public; do
  [ ! -e "$root/tests/$legacy" ] || fail "legacy tests/$legacy remains"
done
for suite in unit integration mechanism cli header contracts; do
  grep -F "suite: '$suite'" "$root/tests/meson.build" >/dev/null ||
    fail "Meson does not register $suite suite"
done
grep -F 'tests/installed/consumer.cpp' "$root/.github/workflows/ci.yml" >/dev/null ||
  fail 'installed consumer is not compiled by CI'
grep -F 'pkgcatalog::acquire::acquire_catalog' "$root/tests/installed/consumer.cpp" >/dev/null ||
  fail 'installed consumer does not call the acquisition API'
grep -F '::mkdtemp(buffer.data())' "$root/tests/support/acquisition_fixture.h" >/dev/null ||
  fail 'temporary-tree fixture is not allocated atomically'
! grep -F 'steady_clock' "$root/tests/support/acquisition_fixture.h" >/dev/null ||
  fail 'timestamp-derived temporary-tree allocation returned'


meson=$root/tests/meson.build
for contract in "$root"/tests/contracts/check_*.sh; do
  [ -x "$contract" ] || fail "contract is not executable: ${contract#$root/}"
  name=$(basename "$contract")
  case $name in
    check_installed_docs.sh | check_installed_html_docs.sh)
      continue
      ;;
  esac
  grep -F "$name" "$meson" >/dev/null ||
    fail "shell contract is not registered in Meson: $name"
done
