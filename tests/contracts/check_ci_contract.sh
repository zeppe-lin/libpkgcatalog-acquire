#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1 file=$1/.github/workflows/ci.yml
fail(){ echo "ci-contract: $*" >&2; exit 1; }
for token in 'repository: zeppe-lin/libpkgsource' 'repository: zeppe-lin/libpkgsource-yaml' \
  'repository: zeppe-lin/libpkgcatalog' 'ref: v3.0.0' 'ref: v1.0.0' \
  'mode: shared' 'mode: static' 'pkg-config "${flags[@]}" --cflags --libs libpkgcatalog-acquire' \
  'Library soname: [libpkgcatalog-acquire.so.3]' '-Db_sanitize=address,undefined'; do
  grep -F -- "$token" "$file" >/dev/null || fail "missing: $token"
done
