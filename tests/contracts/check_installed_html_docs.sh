#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
prefix=$1 version=$2 root=$1/share/htmldocs/libpkgcatalog-acquire/$2
fail(){ echo "installed-html-docs: $*" >&2; exit 1; }
for path in index.html history.html architecture.html acquisition.html integration.html \
  abi.html testing.html code-style.html manpage-markdown.html html.html \
  protocols/collection-layout-v1.html history/catalog-2.1-extraction.html \
  man/pkgcatalog-scan.1.html man/pkgcatalog_acquire.3.html \
  man/pkgcatalog_collection_layout.5.html api/index.html assets/house.css; do
  [ -e "$root/$path" ] || fail "missing $path"
done
