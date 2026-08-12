# libpkgcatalog-acquire

`libpkgcatalog-acquire` is the explicit filesystem and YAML acquisition adapter
for `libpkgcatalog`.

It observes caller-selected native collection roots, parses their
`profiles.yml` and `recipe.yml` documents through `libpkgsource-yaml`, seals
source authority through `libpkgsource`, and submits sealed collection
revisions to `libpkgcatalog`. The result is one authoritative
`pkgcatalog::catalog_snapshot`.

The library does not discover collections, read package-manager configuration,
run Git, assign precedence from repository layout, resolve dependencies, inspect
installed state, fetch source material, construct operation plans, or execute
package effects.

## Why this is a separate repository

Acquisition owns mutable filesystem observation, native layout policy, bounded
document reads, and one syntax adapter. `libpkgcatalog` owns immutable catalog
semantics and identities. Keeping those bodies separate allows the catalog
owner to remain independent of filesystems, YAML, and libyaml while permitting
other acquisition adapters to produce the same catalog declarations.

The repository also contains `pkgcatalog-scan`, a reference client that assigns
precedence from explicit command-line order and prints diagnostic catalog
contents. Its text output is not a storage or resolver protocol.

## Products

- `libpkgcatalog-acquire.so.3` — acquisition request values, filesystem
  observation, two-pass parsing and sealing, and typed acquisition failures.
- `pkgcatalog-scan` — reference validation and inspection frontend.

## Read next

- `DESIGN.md` — owned boundary and construction pipeline.
- `TESTING.md` — qualification topology and release proof.
- `docs/acquisition.md` — exact observations, passes, and refusal points.
- `docs/integration.md` — ecosystem placement and future audit procedure.
- `docs/protocols/collection-layout-v1.md` — native layout protocol.
- `docs/history/catalog-2.1-extraction.md` — extraction and source-3 migration.
- `pkgcatalog_acquire(3)`, `pkgcatalog_collection_layout(5)`, and
  `pkgcatalog-scan(1)` — installed operational documentation.

## Build

```sh
meson setup build \
  -Ddefault_library=shared \
  -Dlink_mode=shared \
  -Dwerror=true
meson compile -C build
meson test -C build --print-errorlogs
```

Build shared and static closures separately. The public metadata requires
`libpkgcatalog >= 3.0.0`; `libpkgsource-yaml >= 1.0.0` and its YAML provider
remain private implementation dependencies.

## License

GPL-3.0-or-later. See `COPYING` and `COPYRIGHT`.
