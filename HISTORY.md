# Project history

## 3.0.2

Parallel-test fixture isolation repair.

- Replaced timestamp-derived temporary collection roots with atomic `mkdtemp()`
  allocation so independently running Meson tests cannot share and mutate the
  same fixture tree.
- Kept collection and recipe symbolic-link refusal unchanged; the observed
  failure was test-fixture cross-talk, not an acquisition-policy defect.
- Aligned installed HTML, pkg-config, and scanner CI qualification with the
  3.0.2 project version so the release workflow tests the artifact it builds.

## 3.0.1

Testing-foundation and acquisition-boundary hardening.

- Rejected NUL-bearing external revision provenance at acquisition request
  construction instead of leaking that refusal into catalog authority later.
- Added an integration seam proving that recipe semantic changes with an
  unchanged package release propagate through source, candidate, collection,
  and catalog identities.
- Made the staged installed consumer call `acquire_catalog()` so shared and
  static pkg-config qualification exercises the real private YAML dependency
  closure instead of header inclusion only.
- Reorganized tests by unit, integration, filesystem mechanism, CLI, header,
  installed-consumer, support, and contract ownership.
- Qualified against corrected `libpkgcatalog 3.0.1` while retaining the public
  compatibility floor at `libpkgcatalog >= 3.0.0`.
- Corrected stale HTML publication documentation that still named version
  1.0.0 and the nonexistent `manual/` output directory.

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
