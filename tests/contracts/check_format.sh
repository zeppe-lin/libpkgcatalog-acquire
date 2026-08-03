#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
formatter=$1 root=$2
files=$(find "$root/include" "$root/src" "$root/tools" "$root/tests" -type f \
  \( -name '*.h' -o -name '*.cpp' \) -print | LC_ALL=C sort)
[ -n "$files" ] || exit 0
# shellcheck disable=SC2086
exec "$formatter" --dry-run --Werror $files
