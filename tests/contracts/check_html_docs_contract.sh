#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
file=$root/tools/build-html-docs.py
fail(){ echo "html-docs-contract: $*" >&2; exit 1; }
for token in 'libpkgcatalog-acquire' 'history.html' 'acquisition.html' \
  'integration.html' 'protocols/collection-layout-v1.html' \
  'man/pkgcatalog_acquire.3.html' 'api/index.html'; do
  grep -F "$token" "$file" >/dev/null || fail "missing: $token"
done
