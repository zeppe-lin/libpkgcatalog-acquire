#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
scanner=$1
version=$2
root=$(mktemp -d)
trap 'rm -rf "$root"' EXIT HUP INT TERM
mkdir -p "$root/core/alpha" "$root/system/alpha"
cat > "$root/core/profiles.yml" <<'YAML'
format: zeppe-lin.profiles/1
profiles: {}
YAML
cat > "$root/core/alpha/recipe.yml" <<'YAML'
format: zeppe-lin.recipe/1
package:
  name: alpha
  version: 1.0
  release: 1
  summary: Alpha
  licenses: [GPL-3.0-or-later]
requirements: {}
sources: []
build:
  language: posix-shell
  script: |
    true
YAML
sed 's/version: 1.0/version: 2.0/' \
  "$root/core/alpha/recipe.yml" > "$root/system/alpha/recipe.yml"
output=$($scanner \
  --collection "core=$root/core" \
  --collection "system=$root/system" \
  --external-revision core=deadbeef)
printf '%s\n' "$output" | grep '^catalog [0-9a-f]\{64\}$' >/dev/null
printf '%s\n' "$output" | grep '^collection 0 core [0-9a-f]\{64\}$' >/dev/null
printf '%s\n' "$output" | grep '^effective alpha 1.0-1 core ' >/dev/null
printf '%s\n' "$output" | grep '^shadowed alpha 2.0-1 system ' >/dev/null
test "$($scanner --version)" = "pkgcatalog-scan $version"
if $scanner --external-revision missing=rev \
     --collection "core=$root/core" >/dev/null 2>&1; then
  echo 'scan-tool-test: unknown revision collection accepted' >&2
  exit 1
fi
