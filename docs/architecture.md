# Architecture

## Role

`libpkgcatalog-acquire.so.3` is a destination-owned adapter between mutable
native collection trees and immutable catalog authority.

```text
explicit collection specifications
          |
          v
filesystem observation and bounded reads       libpkgcatalog-acquire
          |
          v
strict profiles/recipe parsing                  libpkgsource-yaml
          |
          v
profile and source sealing                      libpkgsource
          |
          v
collection and catalog sealing                  libpkgcatalog
```

The returned `catalog_snapshot` is authoritative. Paths, directory names,
external revision labels, and traversal order are observations and provenance,
not semantic identity.

## Why acquisition is not in libpkgcatalog

The catalog owner can validate and seal already constructed declarations without
knowing how bytes were found. Acquisition adds filesystem APIs, symlink and file
type policy, document limits, YAML, and libyaml. Those dependencies have
independent failure modes and must not contaminate catalog semantics or ABI.

A different adapter may later acquire declarations from another syntax or
transport while using the same catalog owner. Repository separation prevents
one adapter from becoming the definition of a catalog.

## Public authority

The public API owns:

- explicit `collection_specification` values;
- positive per-document byte `limits`;
- typed request, filesystem, layout, and document-read failures; and
- `acquire_catalog()`, which performs the complete adapter pipeline.

It does not publish partially acquired state, filesystem snapshots, parsed YAML
trees, or a durable acquisition record.

## Internal bodies

```text
include/libpkgcatalog-acquire/  installed adapter API
src/error.cpp                   typed acquisition failures
src/model.cpp                   request and resource-limit values
src/acquire.cpp                 filesystem observation and two-pass handoff
tools/pkgcatalog_scan.cpp       reference client
abi/                            reviewed ELF export manifest
docs/protocols/                 layout protocol authority
tests/                          unit, integration, mechanism, CLI, header,
                                installed, support, contracts
```

## Failure ownership

`pkgcatalog::acquire::error` covers only adapter-owned request and observation
failures. YAML grammar failures remain `pkgsource::yaml::yaml_error`. Source
semantic refusal remains `pkgsource::error`. Catalog semantic refusal remains
`pkgcatalog::error`. The adapter does not flatten these authorities into one
stringly failure channel.

## Negative capabilities

The adapter cannot:

- discover enabled collections;
- read `pkgman.conf` or historical ports configuration;
- clone, fetch, or inspect Git repositories;
- verify local recipe source paths;
- cache or serialize catalog snapshots;
- resolve package requirements;
- inspect installed state;
- construct package operation or transaction plans;
- apply files, execute lifecycle actions, or publish state.
