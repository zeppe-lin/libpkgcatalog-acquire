#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "documentation-contract: $*" >&2; exit 1; }
for path in README.md HISTORY.md CONTRIBUTING.md MAINTAINING.md Doxyfile \
  docs/architecture.md docs/acquisition.md docs/integration.md docs/abi.md \
  docs/testing.md docs/code-style.md docs/manpage-markdown.md docs/html.md \
  docs/protocols/collection-layout-v1.md \
  docs/history/catalog-2.1-extraction.md; do
  [ -s "$root/$path" ] || fail "missing $path"
done
for token in 'tests/unit/' 'tests/integration/' 'tests/mechanism/' 'tests/cli/' \
  'tests/header/' 'tests/installed/' 'tests/support/' 'tests/contracts/'; do
  grep -F "$token" "$root/docs/testing.md" >/dev/null || fail "testing topology drift: $token"
done

for page in pkgcatalog-scan.1 pkgcatalog_acquire.3 pkgcatalog_collection_layout.5; do
  [ -s "$root/docs/man/$page.md" ] || fail "missing source $page"
  [ -s "$root/docs/man/generated/$page" ] || fail "missing generated $page"
done
