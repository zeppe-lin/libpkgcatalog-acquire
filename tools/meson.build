# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
configuration = configuration_data()
configuration.set_quoted('PKGCATALOG_SCAN_VERSION', meson.project_version())
configure_file(
  output: 'pkgcatalog_scan_config.h',
  configuration: configuration,
)

pkgcatalog_scan = executable(
  'pkgcatalog-scan',
  'pkgcatalog_scan.cpp',
  dependencies: libpkgcatalog_acquire_dep,
  install: true,
  install_dir: get_option('bindir'),
)
