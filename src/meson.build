# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')

libpkgcatalog_acquire = library(
  'pkgcatalog-acquire',
  'acquire.cpp',
  include_directories: public_inc,
  dependencies: [libpkgcatalog_dep, libpkgsource_yaml_dep],
  install: true,
  version: meson.project_version(),
  soversion: '2',
)

libpkgcatalog_acquire_dep = declare_dependency(
  link_with: libpkgcatalog_acquire,
  include_directories: public_inc,
  dependencies: [libpkgcatalog_dep, libpkgsource_yaml_dep],
)
meson.override_dependency('libpkgcatalog-acquire', libpkgcatalog_acquire_dep)

install_headers(
  '../include/libpkgcatalog-acquire/libpkgcatalog-acquire.h',
  '../include/libpkgcatalog-acquire/acquire.h',
  subdir: 'libpkgcatalog-acquire',
)

pkgconf.generate(
  libpkgcatalog_acquire,
  name: 'libpkgcatalog-acquire',
  filebase: 'libpkgcatalog-acquire',
  description: 'Explicit native collection acquisition for libpkgcatalog',
  subdirs: '.',
  requires: [libpkgcatalog_dep, libpkgsource_yaml_dep],
)
