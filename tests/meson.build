# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  test_cpp_args = ['-UNDEBUG']
  test_inc = include_directories('.')

  unit_test = executable(
    'model-unit-test',
    'unit/model_test.cpp',
    include_directories: test_inc,
    dependencies: libpkgcatalog_acquire_dep,
    cpp_args: test_cpp_args,
    install: false,
  )
  test('model-unit', unit_test, suite: 'unit')

  integration_test = executable(
    'acquire-integration-test',
    'integration/acquire_test.cpp',
    include_directories: test_inc,
    dependencies: libpkgcatalog_acquire_dep,
    cpp_args: test_cpp_args,
    install: false,
  )
  test('acquire-integration', integration_test, suite: 'integration')

  mechanism_test = executable(
    'filesystem-mechanism-test',
    'mechanism/filesystem_test.cpp',
    include_directories: test_inc,
    dependencies: libpkgcatalog_acquire_dep,
    cpp_args: test_cpp_args,
    install: false,
  )
  test('filesystem-mechanism', mechanism_test, suite: 'mechanism')

  foreach test_spec : [
    ['header-umbrella', 'header/umbrella_header_test.cpp'],
    ['header-acquire', 'header/acquire_header_test.cpp'],
    ['header-export', 'header/export_header_test.cpp'],
  ]
    header_test = executable(
      test_spec[0] + '-test',
      test_spec[1],
      dependencies: libpkgcatalog_acquire_dep,
      cpp_args: test_cpp_args,
      install: false,
    )
    test(test_spec[0], header_test, suite: 'header')
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
      suite: 'contracts',
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
    'test-structure',
  ]
    test(
      contract,
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_' + contract.replace('-', '_') + '.sh'),
        meson.project_source_root(),
      ] + (contract == 'style-contract' ? [meson.project_build_root()] : []),
      suite: 'contracts',
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
      suite: 'contracts',
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
    suite: 'contracts',
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
      suite: 'contracts',
    )
  endif

  if get_option('scan_tool').enabled()
    test(
      'scan-tool',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'cli/scan_tool_test.sh'),
        pkgcatalog_scan.full_path(),
        meson.project_version(),
      ],
      suite: 'cli',
    )
  endif
endif
