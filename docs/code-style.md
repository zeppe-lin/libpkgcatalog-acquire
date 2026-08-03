# Code style

## Purpose

The repository style exists to make syntax ownership, parser invariants, and
provider boundaries reviewable. Formatting is automated; semantic clarity is a
review obligation.

## C++ formatting

`clang-format 17` and the repository `.clang-format` are canonical. CI runs the
formatter in dry-run error mode over installed headers, implementation sources,
and C++ tests.

Control statements always use braces, including one-statement bodies. Braces
must not be omitted to save vertical space. A later comment or statement must
not be able to change control flow by indentation alone.

Includes are grouped in this order:

1. the corresponding public or private project header;
2. other private project headers;
3. owner-library headers;
4. provider headers, only inside provider implementations;
5. standard-library headers.

## C++ design

Public headers expose owner types and stable adapter types only. Provider types,
implementation helpers, and protocol writers remain under `src/internal`.

A translation unit should have one dominant responsibility. Public parser
entry points coordinate one document grammar. Generic document-tree handling,
recipe field decoding, and libyaml event acquisition belong to separate
internal units.

Keep libyaml types and event layouts inside the libyaml provider translation
unit. The grammar layer consumes provider-neutral events and owns all schema
paths, accepted keys, and construction of parser-neutral declarations.

Protocol names, accepted keys, and field ordering use named constants where
that improves review. Do not derive schema behavior from container iteration or
provider-specific enum representations.

Translate only `libpkgsource` construction failures that belong to one parsed
value. Allocation, logic, and unrelated runtime failures retain their original
types.

## Comments and Doxygen

Installed headers carry complete Doxygen for public types, parameters, return
values, ownership, and exceptions. Public Doxygen is the API source of truth.

Implementation comments explain facts that are not obvious from syntax:
canonical ordering, owner normalization, protocol coupling, defensive branches,
move timing, and exception translation. They do not narrate individual
statements or repeat the public manual.

Private headers may use concise Doxygen-style comments for IDE navigation, but
they are not included in the published public API documentation.

## Tests

Behavioral tests are organized by contract, not by implementation file.
Grammar tests use only the public API. Internal tests bind provider event
normalization and document-tree construction directly.

Shared fixtures expose semantic options rather than construction noise. Each accepted field, rejected feature, resource ceiling, and semantic handoff
is varied independently so one parser rule cannot mask another regression.

## Documentation

Markdown uses ATX headings only: `#`, `##`, `###`, and so on. Setext headings
and horizontal rules are not used. Repository Markdown does not carry SPDX HTML
comments; licensing authority is kept in `COPYING`, `COPYRIGHT`, and source-file
headers where tooling consumes it.

Documentation distinguishes owner facts, adapter behavior, implementation
choices, and exclusions. It must not invent future orchestration, persistence,
compatibility, or execution semantics.

Manual pages use the restricted profile in `docs/manpage-markdown.md`. Markdown
is the canonical source; committed roff is generated release material. Review
both forms together and never edit generated roff directly.

## Review discipline

Every patch should have one review purpose. Tree movement, semantic code, tests,
and documentation are separated when practical. A release candidate is
accepted only after clean shared and static builds, both supported compilers,
sanitizers, generated metadata checks, installed-consumer checks, manual-page
lint, and exact patch replay.
