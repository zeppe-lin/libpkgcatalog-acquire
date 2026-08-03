#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
forbidden='pkgman\.conf|Pkgfile|git clone|libpkgstate|libpkgplan|libpkgresolve|system\('
if grep -E -i "$forbidden" \
    "$root"/include/libpkgcatalog-acquire/*.h \
    "$root"/acquire/*.cpp \
    "$root"/tools/*.cpp; then
  echo 'acquisition-contract-test: forbidden compatibility or downstream coupling' >&2
  exit 1
fi
grep -F 'parse_profiles_yaml_v1' "$root/acquire/acquire.cpp" >/dev/null
grep -F 'profile_catalog::seal' "$root/acquire/acquire.cpp" >/dev/null
grep -F 'seal_recipe_yaml_v1' "$root/acquire/acquire.cpp" >/dev/null
grep -F 'seal_collection' "$root/acquire/acquire.cpp" >/dev/null
grep -F 'catalog_snapshot::seal' "$root/acquire/acquire.cpp" >/dev/null
