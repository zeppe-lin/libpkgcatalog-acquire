# Contributing

## Boundary first

Changes must preserve acquisition-only ownership. The library may observe
explicit roots, enforce the native layout, read bounded documents, invoke the
qualified YAML adapter, and hand declarations to source and catalog sealers. It
must not discover collections, run Git, inspect package-manager configuration,
resolve requirements, fetch source material, construct plans, execute effects,
or publish state.

A layout change must identify the exact observation rule, refusal category,
protocol text, parser handoff, semantic sealer, tests, and compatibility
consequence.

## Engineering standard

Follow `docs/code-style.md`. C++ is formatted with clang-format 17. Public
headers require complete Doxygen. Filesystem helpers report exact diagnostic
paths and never silently follow symbolic links.

`libpkgsource-yaml` is a private syntax dependency. Public headers expose the
catalog owner and source provenance only through catalog/source authority types.

## Tests and documentation

Add focused tests for request validation, filesystem observation, document
limits, two-pass sealing, owner refusals, scanner behavior, ABI, metadata, and
installation. Update canonical Markdown and generated roff together. Generated
files are never edited directly.

## Patch discipline

Keep extraction, semantic changes, tests, generated material, and repository
policy separate when practical. Every patch must pass `git diff --check` and be
reviewable without reconstructing unrelated intent.
