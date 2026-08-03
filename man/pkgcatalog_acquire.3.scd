pkgcatalog_acquire(3)

# NAME

pkgcatalog_acquire - explicit native collection filesystem acquisition

# SYNOPSIS

*#include <libpkgcatalog-acquire/libpkgcatalog-acquire.h>*

# DESCRIPTION

*pkgcatalog::acquire::acquire_catalog()* consumes explicit collection
specifications and returns a sealed *pkgcatalog::catalog_snapshot*.

The adapter performs two-pass acquisition. It first parses every existing root
*profiles.yml* and seals one global profile catalog. It then discovers immediate
non-hidden package directories, parses each regular *recipe.yml*, seals source
snapshots against the global profiles, seals collection revisions, and invokes
the core catalog sealer.

Collection roots, package directory labels, external revision labels, and
filesystem enumeration order are diagnostic observations. They do not replace
package, profile, collection, source, or catalog identities.

# LAYOUT

A collection root may contain one optional *profiles.yml*. Every immediate
non-hidden directory is a package acquisition entry and must contain a regular,
non-symlink *recipe.yml*. Hidden root entries and root regular files other than
*profiles.yml* are ignored. Non-hidden symbolic links are rejected.

The caller must provide quiescent collection roots for one acquisition. The
adapter does not create a filesystem snapshot or run Git.

# LIMITS

*pkgcatalog::acquire::limits* sets the positive maximum number of bytes read for
each YAML document. The default is 1 MiB.

# ERRORS

Filesystem and layout failures throw *pkgcatalog::acquire::error*. YAML syntax
failures remain *pkgsource::yaml_adapter::yaml_error*. Native source and catalog
validation failures remain *pkgsource::error* and *pkgcatalog::error*.

# SEE ALSO

*libpkgcatalog*(3), *pkgcatalog_collection_layout*(5), *pkgcatalog-scan*(1),
*pkgsource_yaml*(3)
