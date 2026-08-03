# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')

elf_export_generator = find_program(
  meson.project_source_root() / 'tools/generate-elf-export-script.sh',
)
elf_export_script_name = 'libpkgcatalog-acquire.exports.map'
elf_export_script_path = meson.current_build_dir() / elf_export_script_name
elf_export_script = configure_file(
  input: '../abi/libpkgcatalog-acquire.exports',
  output: elf_export_script_name,
  command: [elf_export_generator, '@INPUT@'],
  capture: true,
)

libpkgcatalog_acquire = library(
  'pkgcatalog-acquire',
  [
    'error.cpp',
    'model.cpp',
    'acquire.cpp',
  ],
  include_directories: public_inc,
  dependencies: [libpkgcatalog_dep, libpkgsource_yaml_dep],
  cpp_args: visibility_cpp_args + ['-DPKGCATALOG_ACQUIRE_BUILDING_LIBRARY'],
  gnu_symbol_visibility: 'hidden',
  link_args: ['-Wl,--version-script=' + elf_export_script_path],
  link_depends: [elf_export_script],
  install: true,
  version: meson.project_version(),
  soversion: '3',
)

libpkgcatalog_acquire_dep = declare_dependency(
  link_with: libpkgcatalog_acquire,
  include_directories: public_inc,
  dependencies: [libpkgcatalog_dep],
  version: meson.project_version(),
)
meson.override_dependency('libpkgcatalog-acquire', libpkgcatalog_acquire_dep)

install_headers(
  '../include/libpkgcatalog-acquire/libpkgcatalog-acquire.h',
  '../include/libpkgcatalog-acquire/acquire.h',
  '../include/libpkgcatalog-acquire/export.h',
  subdir: 'libpkgcatalog-acquire',
)

pkgconf.generate(
  libpkgcatalog_acquire,
  name: 'libpkgcatalog-acquire',
  filebase: 'libpkgcatalog-acquire',
  description: 'Explicit filesystem acquisition adapter for libpkgcatalog',
  subdirs: '.',
  # The target contributes its full closure privately. Promote only the
  # catalog owner exposed by the installed public API; YAML remains private.
  requires: [libpkgcatalog_dep],
)
