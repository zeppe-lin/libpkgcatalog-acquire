<!-- SPDX-FileCopyrightText: 2026 Alexandr Savca -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Native collection layout and acquisition contract

Version one defines an explicit filesystem acquisition protocol for producing
`libpkgcatalog` declarations. The layout is an observation interface, not
catalog authority. Only the returned sealed `pkgcatalog::catalog_snapshot` is
authoritative.

## Explicit collection specifications

Every collection is supplied by the caller with:

- a canonical collection reference;
- an explicit precedence position;
- a filesystem root;
- optional external revision provenance;
- declaration provenance for the configuration that selected it.

Positions are contiguous from zero. Argument or configuration order does not
replace the explicit position in the library API. The reference
`pkgcatalog-scan` command assigns positions from repeated `--collection`
arguments in command-line order.

Collection roots are normalized to absolute diagnostic paths during one
acquisition. Paths and external revision labels do not enter catalog semantic
identities. A root must be a real directory rather than a symbolic link.

## Layout

The native version-one layout is:

```text
collection-root/
├── profiles.yml                 optional
├── package-directory/
│   ├── recipe.yml               required
│   └── ...                      package-local source material
└── another-package/
    └── recipe.yml
```

Immediate non-hidden directories are package acquisition entries. Every such
directory must contain a regular, non-symlink `recipe.yml`. The directory name
is an acquisition label only. `package.name` inside the sealed recipe is the
package identity, and multiple directories sealing to the same package name are
rejected by collection authority.

Hidden root entries are ignored so repository metadata such as `.git` is not
mistaken for a package. Root regular files other than `profiles.yml` are also
ignored. Non-hidden symbolic links are rejected. Acquisition does not traverse
nested package directories, follow links, or validate recipe-declared local
source inputs.

## Two-pass sealing

Acquisition is deliberately global and two-pass:

1. Read every existing root `profiles.yml`.
2. Parse all profile declarations through `libpkgsource-yaml`.
3. Seal one global `pkgsource::profile_catalog`.
4. Discover and parse every package `recipe.yml`.
5. Seal each recipe against that global profile catalog.
6. Seal each collection revision.
7. Seal the final catalog snapshot under explicit precedence.

Profiles therefore form one global namespace. They do not shadow through
collection precedence. A profile may reference another profile declared in a
different configured collection because the complete declaration set is sealed
before any recipe.

Absent `profiles.yml` means that collection contributes no profile
declarations. Duplicate profile names, unknown nested profiles, and profile
cycles are rejected by `libpkgsource` authority.

## Reads and limits

Documents are read as exact bytes and passed to the strict version-one YAML
adapter. The default maximum is 1 MiB per document and callers may select a
different positive limit. Oversized, short-read, non-regular, or symlinked
documents are rejected before parsing.

Filesystem and layout failures are `pkgcatalog::acquire::error` values. YAML
syntax failures remain structured `pkgsource::yaml_adapter::yaml_error` values.
Native profile, recipe, collection, and catalog failures retain their original
`pkgsource::error` or `pkgcatalog::error` domains.

## Excluded behavior

Version one does not read `pkgman.conf`, infer collection names from paths,
invoke Git, choose branches, fetch repositories, inspect timestamps, cache
catalog snapshots, parse Pkgfiles, apply architecture fallback, resolve
requirements, or inspect installed state. Historical collection discovery
belongs in a separate migration frontend.
