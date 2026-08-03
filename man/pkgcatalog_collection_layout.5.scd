pkgcatalog_collection_layout(5)

# NAME

pkgcatalog_collection_layout - native collection acquisition layout version 1

# DESCRIPTION

A version-one collection root contains an optional *profiles.yml* and immediate
non-hidden package directories. Every package directory contains a required
regular, non-symlink *recipe.yml*.

The package directory name is not package identity. The sealed
*package.name* value in *recipe.yml* is authoritative. Package-local files are
not traversed or verified by catalog acquisition.

All configured *profiles.yml* documents are parsed before any recipe is sealed.
Their declarations form one global profile namespace. Profiles do not shadow by
collection precedence.

Hidden root entries are ignored. Root regular files other than *profiles.yml*
are ignored. Non-hidden directories without *recipe.yml* and non-hidden
symbolic links are errors.

Paths and external revision labels are diagnostic provenance excluded from
semantic identity. Callers must provide quiescent roots for one acquisition.

# COMPATIBILITY

The layout does not describe Pkgfiles, historical ports trees, *pkgman.conf*,
Git branches, or legacy collection precedence. Translation belongs in a
separate migration frontend.

# SEE ALSO

*pkgcatalog_acquire*(3), *pkgcatalog-scan*(1), *pkgsource_profiles_yaml*(5),
*pkgsource_recipe_yaml*(5)
