% PKGCATALOG_ACQUIRE(3) libpkgcatalog-acquire 4.0.0 | libpkgcatalog-acquire

# NAME

pkgcatalog_acquire - acquire explicit native collections into catalog authority

# SYNOPSIS

```cpp
#include <libpkgcatalog-acquire/libpkgcatalog-acquire.h>

pkgcatalog::catalog_snapshot
pkgcatalog::acquire::acquire_catalog(
    std::vector<pkgcatalog::acquire::collection_specification> collections,
    pkgcatalog::acquire::limits limits = {});
```

# DESCRIPTION

`acquire_catalog()` observes caller-selected collection roots and returns one
sealed `pkgcatalog::catalog_snapshot`.

Acquisition is global and two-pass. The adapter first reads every optional root
`profiles.yml`, parses all profile declarations through `libpkgsource-yaml`,
and seals one `pkgsource::profile_catalog`. It then discovers immediate visible
package directories, parses each `recipe.yml`, seals source snapshots against
the global profiles, seals collection revisions, and invokes catalog sealing.

Collection names, roots, precedence, external revision labels, and declaration
provenance are supplied explicitly. External revision labels are optional
diagnostic provenance and must be non-empty and line-safe when present. The library does not discover collections
or read package-manager configuration.

# LIMITS

`pkgcatalog::acquire::limits` sets the positive maximum number of bytes read for
each YAML document. The default is 1048576 bytes. The same ceiling is supplied
to the YAML parser as its document-byte limit.

# ERRORS

Request, filesystem, layout, and document-read failures throw
`pkgcatalog::acquire::error`. YAML grammar failures remain
`pkgsource::yaml::yaml_error`. Source and catalog semantic refusals remain
`pkgsource::error` and `pkgcatalog::error`.

# AUTHORITY

The returned catalog snapshot is authoritative. Filesystem paths, package
directory names, traversal order, and external revision labels are diagnostic
observations and provenance, not semantic identity.

# ABI

The public ABI is `libpkgcatalog-acquire.so.4`. Public metadata requires
`libpkgcatalog >= 4.0.0, < 5.0.0`; the YAML adapter is private metadata.

# SEE ALSO

`pkgcatalog_collection_layout(5)`, `pkgcatalog-scan(1)`, `libpkgcatalog(3)`,
`pkgsource_yaml(3)`
