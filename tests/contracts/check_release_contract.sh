#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "release-contract: $*" >&2; exit 1; }
require(){ grep -F -- "$2" "$1" >/dev/null || fail "missing in ${1#$root/}: $2"; }
require "$root/meson.build" "version: '3.0.2'"
require "$root/meson.build" "version: '>=3.0.0'"
require "$root/meson.build" "version: '>=1.0.0'"
require "$root/src/meson.build" "soversion: '3'"
require "$root/HISTORY.md" '## 3.0.2'
require "$root/Doxyfile" 'PROJECT_NUMBER         = 3.0.2'
require "$root/README.md" '`libpkgcatalog-acquire.so.3`'
require "$root/docs/history/catalog-2.1-extraction.md" '# Extraction from libpkgcatalog 2.1'
require "$root/tests/meson.build" "test_cpp_args = ['-UNDEBUG']"
require "$root/tests/meson.build" 'cpp_args: test_cpp_args'
require "$root/.github/workflows/ci.yml" 'name: GCC release'
require "$root/.github/workflows/ci.yml" '--buildtype=${{ matrix.buildtype'
! find "$root" -path "$root/.git" -prune -o -type f \
  \( -name '*.scd' -o -name 'meson_options.txt' -o -name 'CHANGELOG.md' \) \
  -print | grep . >/dev/null || fail 'retired repository authority remains'
