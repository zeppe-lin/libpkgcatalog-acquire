# History

## 2.1.0

Durable acquired-catalog authority.

- Added one canonical bounded encoding for complete sealed catalog snapshots.
- Raised the core and YAML acquisition dependency floors to `libpkgsource 2.1`.
- Embedded the `libpkgsource` 2.1 profile-catalog and source-snapshot records
  instead of duplicating package-source schemas.
- Reconstructed collection revisions, candidate precedence, shadow relations,
  and catalog identity only through the ordinary sealers.
- Retained collection and source diagnostic provenance without admitting it to
  semantic identity.
- Added whole-record checksum, stored-identity, non-canonical, and nested-record
  refusal tests.
- Kept filesystem acquisition, YAML parsing, Git operations, resolution,
  transaction construction, and orchestration evidence outside the codec.

## 2.0.0

ABI rebuild for libpkgsource 2.

- Rebuilt `source_snapshot`-retaining public values against `libpkgsource.so.2`.
- Raised the core dependency floor to `libpkgsource >= 2.0.0`.
- Raised the acquisition dependency floor to `libpkgsource-yaml >= 2.0.0`.
- Advanced `libpkgcatalog` and `libpkgcatalog-acquire` to SONAME 2.
- Preserved collection, candidate, catalog, and acquisition semantics and identity domains.

## 1.1.0

Native collection acquisition frontend.

- Added explicit collection root specifications and resource limits.
- Added two-pass global profile and recipe acquisition.
- Added strict symlink and non-hidden directory handling.
- Added deterministic filesystem enumeration independent of semantic identity.
- Added `libpkgcatalog-acquire.so.1`.
- Added the `pkgcatalog-scan` reference validation and inspection tool.
- Kept the core `libpkgcatalog.so.1` authority and identity domains unchanged.
- Deliberately excluded Git operations, historical configuration, caching,
  dependency resolution, architecture fallback, and local-source verification.

## 1.0.0

Initial native catalog authority.

- Added canonical collection references and diagnostic acquisition provenance.
- Added deterministic sealed collection revisions.
- Added explicit contiguous collection precedence.
- Added retained effective and shadowed package candidates.
- Added global sealed profile-authority validation.
- Added domain-separated collection, revision, candidate, and snapshot
  identities.
- Deliberately excluded collection discovery, YAML parsing, dependency
  resolution, architecture fallback, installed-state access, and CRUX-era
  compatibility.
