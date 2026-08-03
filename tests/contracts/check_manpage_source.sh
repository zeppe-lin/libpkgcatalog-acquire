#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "manpage-source: $*" >&2; exit 1; }
for page in "$root"/docs/man/*.md; do
  grep -E '^% [A-Z0-9_-]+\([1-9]\)' "$page" >/dev/null || fail "missing title: ${page#$root/}"
  grep -F '# NAME' "$page" >/dev/null || fail "missing NAME: ${page#$root/}"
  grep -F '# SYNOPSIS' "$page" >/dev/null || fail "missing SYNOPSIS: ${page#$root/}"
done
! find "$root" -type f -name '*.scd' -print | grep . >/dev/null || fail 'retired scdoc remains'
