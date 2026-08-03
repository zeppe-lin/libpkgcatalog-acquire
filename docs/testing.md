# Testing

The suite separates adapter behavior from repository proof.

- `tests/integration/` exercises real filesystem/YAML/source/catalog handoff.
- `tests/acquire/` exercises the reference scanner.
- `tests/public/` compiles each installed header independently.
- `tests/contracts/` binds acquisition ownership, layout protocol, provider
  privacy, ABI, pkg-config, release metadata, documentation, style, and CI.

Shared and static builds are qualified with GCC and Clang. Sanitizer jobs run
ASan and UBSan. Shared builds compare the exact ELF surface and inspect direct
`NEEDED` dependencies. Installed-consumer tests use generated pkg-config
metadata rather than in-tree dependency objects.

Filesystem tests use temporary roots and vary one observation rule at a time:
request continuity, duplicate roots or names, hidden entries, missing recipes,
symlinks, document limits, global profiles, candidate shadowing, and owner
refusals.
