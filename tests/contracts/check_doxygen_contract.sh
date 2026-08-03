#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "doxygen-contract: $*" >&2; exit 1; }
for setting in 'EXTRACT_ALL            = NO' 'WARN_IF_UNDOCUMENTED   = YES' \
  'WARN_IF_DOC_ERROR      = YES' 'WARN_NO_PARAMDOC       = YES' \
  'WARN_AS_ERROR          = YES'; do
  grep -F "$setting" "$root/Doxyfile" >/dev/null || fail "missing: $setting"
done
for header in "$root"/include/libpkgcatalog-acquire/*.h; do
  grep -F '\file' "$header" >/dev/null || fail "missing file docs: ${header#$root/}"
done
for token in '\param collections' '\param resource_limits' '\param precedence' \
  '\param max_document_bytes' '\return' '\throws error'; do
  grep -R -F "$token" "$root/include" >/dev/null || fail "missing: $token"
done
