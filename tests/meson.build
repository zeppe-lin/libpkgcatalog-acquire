# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  integration_test = executable(
    'acquire-integration-test',
    'integration/acquire_test.cpp',
    dependencies: libpkgcatalog_acquire_dep,
    install: false,
  )
  test('acquire-integration', integration_test)

  foreach test_spec : [
    ['public-umbrella', 'public/umbrella_header_test.cpp'],
    ['public-acquire', 'public/acquire_header_test.cpp'],
    ['public-export', 'public/export_header_test.cpp'],
  ]
    public_test = executable(
      test_spec[0] + '-test',
      test_spec[1],
      dependencies: libpkgcatalog_acquire_dep,
      install: false,
    )
    test(test_spec[0], public_test)
  endforeach

  sh = find_program('sh')
  if library_kind == 'shared'
    nm = find_program('nm')
    test(
      'abi-surface',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_abi_surface.sh'),
        nm.full_path(),
        libpkgcatalog_acquire.full_path(),
        meson.project_source_root() / 'abi/libpkgcatalog-acquire.exports',
      ],
    )
  endif

  foreach contract : [
    'acquisition-contract',
    'provider-boundary',
    'architecture-contract',
    'release-contract',
    'documentation-contract',
    'documentation-install-contract',
    'doxygen-contract',
    'html-docs-contract',
    'manpage-source',
    'style-contract',
    'ci-contract',
  ]
    test(
      contract,
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_' + contract.replace('-', '_') + '.sh'),
        meson.project_source_root(),
      ],
    )
  endforeach

  pandoc = find_program('pandoc', required: false)
  if pandoc.found()
    test(
      'manpage-generated',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_manpage_generated.sh'),
        meson.project_source_root(),
        pandoc.full_path(),
      ],
    )
  endif

  test(
    'metadata',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/metadata_test.sh'),
      meson.project_build_root(),
      meson.project_version(),
    ],
  )

  clang_format = find_program('clang-format-17', required: false)
  if clang_format.found()
    test(
      'format',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_format.sh'),
        clang_format.full_path(),
        meson.project_source_root(),
      ],
    )
  endif

  if get_option('scan_tool').enabled()
    test(
      'scan-tool',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'acquire/scan_tool_test.sh'),
        pkgcatalog_scan.full_path(),
        meson.project_version(),
      ],
    )
  endif
endif
