#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
file=$root/tools/build-html-docs.py
policy=$root/docs/html.md
fail(){ echo "html-docs-contract: $*" >&2; exit 1; }
for token in 'libpkgcatalog-acquire' 'history.html' 'acquisition.html' \
  'integration.html' 'protocols/collection-layout-v1.html' \
  'man/pkgcatalog_acquire.3.html' 'api/index.html'; do
  grep -F "$token" "$file" >/dev/null || fail "missing: $token"
done
grep -F 'libpkgcatalog-acquire/3.0.1/' "$policy" >/dev/null || fail 'published HTML version drift'
grep -F 'man/' "$policy" >/dev/null || fail 'published HTML manual path drift'
! grep -F 'manual/' "$policy" >/dev/null || fail 'obsolete HTML manual path remains'
