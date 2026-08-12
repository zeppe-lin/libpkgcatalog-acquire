#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=${1:?source root required}
fail() { echo "documentation-source-contract: $*" >&2; exit 1; }

for name in README.md DESIGN.md TESTING.md HISTORY.md MAINTAINING.md CONTRIBUTING.md; do
  file=$root/$name
  [ -s "$file" ] || fail "missing authored root document: $name"
  first=$(sed -n '1p' "$file")
  case $first in
    '# '*) ;;
    *) fail "root document does not begin with an ATX level-one heading: $name" ;;
  esac
  count=$(grep -c '^# ' "$file" || true)
  [ "$count" -eq 1 ] || fail "root document must contain exactly one level-one heading: $name"
done

if grep -RInE '^[=-]{3,}$|^~{3,}$' "$root"/*.md "$root/docs" --include='*.md' >/dev/null 2>&1; then
  fail 'Setext/underline Markdown heading remains'
fi
if grep -RIn "$(printf '\t')" "$root"/*.md "$root/docs" --include='*.md' >/dev/null 2>&1; then
  fail 'tab character remains in authored Markdown'
fi
