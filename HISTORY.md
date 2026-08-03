# Project history

## 3.0.0

First independent `libpkgcatalog-acquire` release.

- Extracted filesystem acquisition and `pkgcatalog-scan` from
  `libpkgcatalog 2.1.0`.
- Migrated to `libpkgcatalog 3.0.0`, `libpkgsource 3.0.0`, and standalone
  `libpkgsource-yaml 1.0.0` authority.
- Preserved explicit collection specifications, bounded document reads,
  symlink refusal, deterministic directory ordering, and global two-pass
  profile/recipe acquisition.
- Established `libpkgcatalog-acquire.so.3`, exact ELF exports, strict public API
  documentation, canonical Markdown manuals, versioned HTML documentation, and
  installed-consumer metadata tests.
- Kept collection discovery, Git operations, caching, dependency resolution,
  local-source verification, planning, execution, and state publication out of
  the adapter.
