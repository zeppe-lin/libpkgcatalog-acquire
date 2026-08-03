#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
forbidden='pkgman\.conf|Pkgfile|git clone|libpkgstate|libpkgplan|libpkgresolve|system\(|yaml_adapter|parse_profiles_yaml_v1|seal_recipe_yaml_v1'
if grep -E -i "$forbidden" \
    "$root"/include/libpkgcatalog-acquire/*.h \
    "$root"/src/*.cpp \
    "$root"/tools/*.cpp; then
  echo 'acquisition-contract-test: forbidden compatibility or downstream coupling' >&2
  exit 1
fi
grep -F 'pkgsource::yaml::parse_profiles_yaml' "$root/src/acquire.cpp" >/dev/null
grep -F 'profile_catalog::seal' "$root/src/acquire.cpp" >/dev/null
grep -F 'pkgsource::yaml::parse_recipe_yaml' "$root/src/acquire.cpp" >/dev/null
grep -F 'pkgsource::seal_source' "$root/src/acquire.cpp" >/dev/null
grep -F 'seal_collection' "$root/src/acquire.cpp" >/dev/null
grep -F 'catalog_snapshot::seal' "$root/src/acquire.cpp" >/dev/null
