#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "acquisition-contract: $*" >&2; exit 1; }
for token in \
  'pkgsource::yaml::parse_profiles_yaml' \
  'pkgsource::profile_catalog::seal' \
  'pkgsource::yaml::parse_recipe_yaml' \
  'pkgsource::seal_source' \
  'seal_collection' \
  'catalog_snapshot::seal'; do
  grep -F "$token" "$root/src/acquire.cpp" >/dev/null || fail "missing handoff: $token"
done
for token in \
  'std::filesystem::canonical' \
  'std::filesystem::symlink_status' \
  'document changed while it was read' \
  'collection precedence must be contiguous from zero' \
  'duplicate normalized collection root'; do
  grep -F "$token" "$root/src/acquire.cpp" >/dev/null || fail "missing observation rule: $token"
done
for forbidden in 'pkgman\.conf' 'Pkgfile' 'git clone' 'libpkgstate' 'libpkgplan' \
                 'libpkgresolve' 'fork\(' 'exec[lvpe]*\(' 'system\('; do
  ! grep -R -n -E -i "$forbidden" "$root/include" "$root/src" "$root/tools" >/dev/null ||
    fail "forbidden downstream or compatibility coupling: $forbidden"
done
