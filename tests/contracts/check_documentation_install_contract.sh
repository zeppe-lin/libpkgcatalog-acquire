#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1 file=$1/docs/meson.build
fail(){ echo "documentation-install-contract: $*" >&2; exit 1; }
for token in "'../HISTORY.md'" "'../DESIGN.md'" "'../TESTING.md'" "'acquisition.md'" \
  "'integration.md'" "'protocols/collection-layout-v1.md'" \
  "'history/catalog-2.1-extraction.md'" "'man/pkgcatalog-scan.1.md'" \
  "install_tag: 'doc'" "install_tag: 'html-docs'"; do
  grep -F "$token" "$file" >/dev/null || fail "missing: $token"
done
