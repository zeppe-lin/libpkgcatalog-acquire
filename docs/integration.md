# Ecosystem integration

## Authority map

| Edge | Producer | Consumer | Contract |
| --- | --- | --- | --- |
| collection request | orchestration/configuration adapter | libpkgcatalog-acquire | explicit name, precedence, root, provenance |
| document bytes | libpkgcatalog-acquire | libpkgsource-yaml | bounded caller-owned bytes and source origin |
| source declarations | libpkgsource-yaml | libpkgsource | parser-neutral values |
| source snapshots | libpkgsource | libpkgcatalog | sealed package-source authority |
| catalog snapshot | libpkgcatalog | resolver/planner orchestration | immutable available-package authority |

`pkgcatalog-scan` is a reference client of the first four edges. It is not the
configuration authority for production orchestration.

## Placement test

Acquisition belongs here while all of the following remain true:

1. callers select collections and precedence explicitly;
2. the library observes native collection paths and reads their documents;
3. syntax parsing is delegated to an independent adapter;
4. source and catalog semantics are delegated to their owners;
5. the output is one catalog snapshot, not a resolver or transaction object.

The component is misplaced if it begins discovering configuration, mutating or
fetching repositories, resolving dependencies, inspecting installed state,
constructing operation plans, or publishing execution evidence. Such a change
creates a new authority edge and requires its owning body, repository, tests,
and documentation.

## Future audit procedure

An ecosystem audit records exact tags or commits for the requester,
`libpkgcatalog-acquire`, `libpkgsource-yaml`, `libpkgsource`, `libpkgcatalog`, and
the first downstream consumer. For each request field, identify the issuing
authority. For each output field, identify the consuming authority. Verify that
no neighbor duplicates root normalization, YAML grammar, source sealing,
collection sealing, precedence, or catalog identity.

A missing neighboring body is an unverified edge. Do not fill it from component
names, roadmaps, or architectural memory.
