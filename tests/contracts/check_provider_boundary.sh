#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "provider-boundary: $*" >&2; exit 1; }
! grep -R -n -E '#[[:space:]]*include[[:space:]]*[<"](libpkgsource-yaml|yaml\.h)' "$root/include" >/dev/null ||
  fail 'YAML syntax or provider headers escaped the installed API'
grep -F '#include <libpkgsource-yaml/parser.h>' "$root/src/acquire.cpp" >/dev/null ||
  fail 'qualified YAML adapter is not explicit in the implementation'
for token in 'pkgsource::yaml::parse_profiles_yaml' 'pkgsource::yaml::parse_recipe_yaml'; do
  grep -F "$token" "$root/src/acquire.cpp" >/dev/null || fail "missing handoff: $token"
done
! grep -R -n -E '#[[:space:]]*include[[:space:]]*[<"]yaml\.h' "$root/src" >/dev/null ||
  fail 'libyaml provider contaminated acquisition implementation'
