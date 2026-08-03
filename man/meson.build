# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if not get_option('man_pages').disabled()
  scdoc = find_program('scdoc', required: get_option('man_pages'))
  if scdoc.found()
    foreach manual : [
      ['pkgcatalog-scan.1.scd', 'pkgcatalog-scan.1', 'man1'],
      ['pkgcatalog_acquire.3.scd', 'pkgcatalog_acquire.3', 'man3'],
      ['pkgcatalog_collection_layout.5.scd', 'pkgcatalog_collection_layout.5', 'man5'],
    ]
      output = custom_target(
        manual[1],
        input: manual[0],
        output: manual[1],
        command: [scdoc],
        feed: true,
        capture: true,
      )
      install_man(output)
    endforeach
  endif
endif
