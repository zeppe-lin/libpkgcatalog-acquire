#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
prefix=$1 root=$1/share/doc/libpkgcatalog-acquire
fail(){ echo "installed-docs: $*" >&2; exit 1; }
for path in README.md DESIGN.md TESTING.md HISTORY.md CONTRIBUTING.md MAINTAINING.md COPYING COPYRIGHT \
  acquisition.md integration.md abi.md code-style.md \
  manpage-markdown.md html.md protocols/collection-layout-v1.md \
  history/catalog-2.1-extraction.md man/pkgcatalog-scan.1.md \
  man/pkgcatalog_acquire.3.md man/pkgcatalog_collection_layout.5.md \
  assets/house.css assets/doxygen-extra.css; do
  [ -s "$root/$path" ] || fail "missing $path"
done
[ -s "$prefix/share/man/man1/pkgcatalog-scan.1" ] || fail 'missing scanner manual'
[ -s "$prefix/share/man/man3/pkgcatalog_acquire.3" ] || fail 'missing API manual'
[ -s "$prefix/share/man/man5/pkgcatalog_collection_layout.5" ] || fail 'missing layout manual'
