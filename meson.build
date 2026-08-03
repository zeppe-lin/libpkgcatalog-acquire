# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
project(
  'libpkgcatalog-acquire',
  'cpp',
  version: '2.1.0',
  license: 'GPL-3.0-or-later',
  meson_version: '>=1.2.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=3',
    'default_library=shared',
    'b_ndebug=if-release',
  ],
)

library_kind = get_option('default_library')
if library_kind == 'both'
  error('libpkgcatalog-acquire requires separate shared and static builds')
endif
want_static = get_option('link_mode') == 'static'
if (library_kind == 'static') != want_static
  error('default_library and link_mode must both select shared or static')
endif

libpkgcatalog_dep = dependency(
  'libpkgcatalog',
  version: '>=2.1.0',
  required: true,
  static: want_static,
)
libpkgsource_yaml_dep = dependency(
  'libpkgsource-yaml',
  version: '>=2.1.0',
  required: true,
  static: want_static,
)
pkgconf = import('pkgconfig')

subdir('src')
if get_option('scan_tool').enabled()
  subdir('tools')
endif
subdir('man')
subdir('tests')
