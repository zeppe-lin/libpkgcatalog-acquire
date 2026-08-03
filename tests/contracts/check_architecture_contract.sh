#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "architecture-contract: $*" >&2; exit 1; }
for path in docs/architecture.md docs/acquisition.md docs/integration.md; do
  [ -s "$root/$path" ] || fail "missing $path"
done
for token in \
  'destination-owned adapter' \
  'global and two-pass' \
  'libpkgsource-yaml' \
  'libpkgcatalog' \
  'unverified edge' \
  'does not publish partially acquired state'; do
  grep -R -F "$token" "$root/README.md" "$root/docs" >/dev/null || fail "missing: $token"
done
for forbidden in \
  'discovers enabled collections' \
  'runs Git repositories' \
  'serializes the catalog snapshot'; do
  ! grep -R -F "$forbidden" "$root/README.md" "$root/docs" >/dev/null || fail "false authority claim: $forbidden"
done
