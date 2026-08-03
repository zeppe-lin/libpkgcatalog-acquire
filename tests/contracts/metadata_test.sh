#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build_root=$1 version=$2
fail(){ echo "metadata: $*" >&2; exit 1; }
file=$build_root/meson-private/libpkgcatalog-acquire.pc
[ -s "$file" ] || file=$(find "$build_root" -type f -name libpkgcatalog-acquire.pc -print | sed -n '1p')
[ -n "${file:-}" ] && [ -s "$file" ] || fail 'generated .pc not found'
count_requirement(){
  field=$1 module=$2
  awk -v field="$field" -v module="$module" '
    $1 == field {
      line=$0; sub(/^[^:]+:[[:space:]]*/, "", line)
      n=split(line, items, /,[[:space:]]*/)
      for (i=1; i<=n; ++i) {
        item=items[i]; sub(/^[[:space:]]*/, "", item)
        if (item ~ ("^" module "([[:space:]]|$)")) ++count
      }
    }
    END { print count + 0 }
  ' "$file"
}
grep -F 'Name: libpkgcatalog-acquire' "$file" >/dev/null
grep -F "Version: $version" "$file" >/dev/null
grep -E 'Libs:.*-lpkgcatalog-acquire([[:space:]]|$)' "$file" >/dev/null
[ "$(count_requirement Requires: libpkgcatalog)" -eq 1 ] || fail 'catalog owner must be public exactly once'
grep -E '^Requires:.*libpkgcatalog[[:space:]]*>=[[:space:]]*3\.0\.0([,[:space:]]|$)' "$file" >/dev/null ||
  fail 'catalog floor is missing'
[ "$(count_requirement Requires.private: libpkgsource-yaml)" -eq 1 ] ||
  fail 'YAML adapter must be private exactly once'
grep -E '^Requires\.private:.*libpkgsource-yaml[[:space:]]*>=[[:space:]]*1\.0\.0([,[:space:]]|$)' "$file" >/dev/null ||
  fail 'YAML adapter floor is missing'
! grep -E '^Requires:.*libpkgsource-yaml|^Requires\.private:.*libpkgcatalog([,[:space:]]|$)' "$file" >/dev/null ||
  fail 'dependency appears in the wrong metadata field'
