#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

LC_ALL=C
export LC_ALL
[ "$#" -eq 3 ] || { echo "usage: $0 NM LIBRARY MANIFEST" >&2; exit 2; }
nm=$1 library=$2 manifest=$3
tmp=${TMPDIR:-/tmp}/libpkgcatalog-acquire-abi.$$
trap 'rm -f "$tmp"' EXIT HUP INT TERM
"$nm" -D --defined-only "$library" | awk '{print $3}' | sed '/^$/d' | sort -u > "$tmp"
if ! cmp -s "$manifest" "$tmp"; then
  echo "abi-surface: exported ABI differs from $manifest" >&2
  diff -u "$manifest" "$tmp" >&2 || true
  exit 1
fi
