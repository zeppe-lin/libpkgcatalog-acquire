# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  acquire_test = executable(
    'acquire-test',
    'acquire_test.cpp',
    dependencies: libpkgcatalog_acquire_dep,
    install: false,
  )
  test('acquire', acquire_test)

  acquire_headers = executable(
    'acquire-headers',
    'acquire_header_test.cpp',
    dependencies: libpkgcatalog_acquire_dep,
    install: false,
  )
  test('acquire-headers', acquire_headers)

  acquisition_contract = find_program('check_acquisition_contract.sh')
  test(
    'acquisition-contract',
    acquisition_contract,
    args: [meson.project_source_root()],
  )

  if get_option('scan_tool').enabled()
    scan_tool_test = find_program('scan_tool_test.sh')
    test(
      'scan-tool',
      scan_tool_test,
      args: [pkgcatalog_scan, meson.project_version()],
    )
  endif
endif
