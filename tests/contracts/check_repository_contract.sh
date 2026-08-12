#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=${1:?source root required}
fail() { echo "repository-contract: $*" >&2; exit 1; }

[ -s "$root/meson.options" ] || fail 'meson.options is absent'
[ ! -e "$root/meson_options.txt" ] || fail 'legacy meson_options.txt remains'
[ -s "$root/DESIGN.md" ] || fail 'root DESIGN.md is absent'
[ -s "$root/TESTING.md" ] || fail 'root TESTING.md is absent'
[ ! -e "$root/docs/architecture.md" ] || fail 'duplicate docs/architecture.md authority remains'
[ ! -e "$root/docs/testing.md" ] || fail 'duplicate docs/testing.md authority remains'
[ -d "$root/docs/man/generated" ] || fail 'generated manual directory is absent'
[ ! -e "$root/man" ] || fail 'legacy root man/ authority remains'

if find "$root" -type f \( -name '*.scd' -o -name '*.scdoc' \) | grep . >/dev/null; then
  fail 'scdoc manual authority remains'
fi
if grep -RInF 'meson_options.txt' \
    "$root/meson.build" "$root/.github" "$root/docs" "$root/tools" \
    >/dev/null 2>&1; then
  fail 'legacy Meson options name remains in active source'
fi
if grep -RInE 'docs/(architecture|testing)\.md' \
    "$root/README.md" "$root/CONTRIBUTING.md" "$root/MAINTAINING.md" \
    "$root/DESIGN.md" "$root/TESTING.md" "$root/docs" "$root/tools" \
    >/dev/null 2>&1; then
  fail 'retired nested design/testing path remains in active source'
fi
if grep -RInF 'architecture.html' \
    "$root/README.md" "$root/DESIGN.md" "$root/TESTING.md" "$root/docs" "$root/tools" \
    >/dev/null 2>&1; then
  fail 'retired architecture.html publication name remains'
fi

grep -F "input: 'generated/" "$root/docs/man/meson.build" >/dev/null ||
  fail 'ordinary man installation is not sourced from committed generated roff'
grep -F "'update-man-pages'" "$root/docs/man/meson.build" >/dev/null ||
  fail 'manual regeneration target is absent'
grep -F "'check-man-pages'" "$root/docs/man/meson.build" >/dev/null ||
  fail 'manual freshness target is absent'
