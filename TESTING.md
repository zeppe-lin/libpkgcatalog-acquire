# Testing libpkgcatalog-acquire

The suite separates request semantics, adapter composition, filesystem
mechanism, reference-client behavior, installed surface, and repository proof.

- `tests/unit/` exercises acquisition request values and stable failure names.
- `tests/integration/` exercises the real YAML/source/catalog handoff and
  identity propagation across that seam.
- `tests/mechanism/` exercises native collection layout, document limits, and
  symbolic-link refusal through the public acquisition operation.
- `tests/cli/` exercises the `pkgcatalog-scan` reference frontend.
- `tests/header/` compiles each installed header independently.
- `tests/installed/` contains the staged pkg-config consumer used by CI. It
  calls `acquire_catalog()` so static qualification must resolve the private
  YAML adapter closure rather than succeeding through an unused library flag.
- `tests/support/` contains filesystem fixtures shared by behavioral tests.
- `tests/contracts/` binds acquisition ownership, layout protocol, provider
  privacy, ABI, pkg-config, release metadata, documentation, style, CI, and
  test topology.

Shared and static builds are qualified with GCC and Clang. Sanitizer jobs run
ASan and UBSan. Shared builds compare the exact ELF surface and inspect direct
`NEEDED` dependencies. Installed-consumer tests use generated pkg-config
metadata rather than in-tree dependency objects.

Filesystem tests use temporary roots and vary one observation rule at a time:
request continuity, duplicate roots or names, hidden entries, missing recipes,
symlinks, nonblocking special-file refusal, document limits, global profiles,
candidate shadowing, and owner refusals.
