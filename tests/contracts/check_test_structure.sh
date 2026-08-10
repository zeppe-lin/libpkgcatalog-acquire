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
