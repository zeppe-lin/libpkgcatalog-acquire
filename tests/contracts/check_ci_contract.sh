#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
file=$1/.github/workflows/ci.yml
fail(){ echo "ci-contract: $*" >&2; exit 1; }
for token in \
  'repository: zeppe-lin/libpkgsource' \
  'repository: zeppe-lin/libpkgsource-yaml' \
  'repository: zeppe-lin/libpkgcatalog' \
  'ref: v3.0.0' \
  'ref: v1.0.0' \
  'mode: shared' \
  'mode: static' \
  'tests/installed/consumer.cpp' \
  'pkg-config "${flags[@]}" --cflags --libs libpkgcatalog-acquire' \
  'Library soname: [libpkgcatalog-acquire.so.3]' \
  '-Db_sanitize=address,undefined'; do
  grep -F -- "$token" "$file" >/dev/null || fail "missing: $token"
done
for token in \
  'tests/contracts/check_installed_html_docs.sh "$stage$PREFIX" 3.0.2' \
  'test "$(pkg-config --modversion libpkgcatalog-acquire)" = 3.0.2' \
  'tests/cli/scan_tool_test.sh "$PREFIX/bin/pkgcatalog-scan" 3.0.2'; do
  grep -F -- "$token" "$file" >/dev/null ||
    fail "installed release qualification drift: $token"
done
! grep -F 'check_installed_html_docs.sh "$stage$PREFIX" 3.0.1' "$file" >/dev/null ||
  fail 'installed HTML qualification still expects libpkgcatalog-acquire 3.0.1'
! grep -F 'pkg-config --modversion libpkgcatalog-acquire)" = 3.0.1' "$file" >/dev/null ||
  fail 'installed pkg-config qualification still expects libpkgcatalog-acquire 3.0.1'
! grep -F 'scan_tool_test.sh "$PREFIX/bin/pkgcatalog-scan" 3.0.1' "$file" >/dev/null ||
  fail 'installed scanner qualification still expects libpkgcatalog-acquire 3.0.1'
catalog_refs=$(awk '
  /repository: zeppe-lin\/libpkgcatalog$/ {
    getline
    if ($0 ~ /ref: v3\.0\.1$/) ++count
  }
  END { print count + 0 }
' "$file")
[ "$catalog_refs" -eq 2 ] ||
  fail 'both CI matrices must qualify against libpkgcatalog v3.0.1'
! awk '
  /repository: zeppe-lin\/libpkgcatalog$/ {
    getline
    if ($0 ~ /ref: v3\.0\.0$/) found=1
  }
  END { exit found ? 0 : 1 }
' "$file" || fail 'stale libpkgcatalog v3.0.0 qualification remains'
