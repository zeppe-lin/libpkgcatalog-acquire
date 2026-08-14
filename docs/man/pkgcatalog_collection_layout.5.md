% PKGCATALOG_COLLECTION_LAYOUT(5) libpkgcatalog-acquire 4.0.0 | libpkgcatalog-acquire

# NAME

pkgcatalog_collection_layout - native collection acquisition layout version 1

# SYNOPSIS

```text
collection-root/
├── profiles.yml
└── package-directory/
    └── recipe.yml
```

# DESCRIPTION

A version-one collection root contains one optional `profiles.yml` and immediate
visible package directories. Every such directory contains one required regular,
non-symlink `recipe.yml`.

Immediate hidden entries are ignored. Visible symbolic links and unsupported
file types are refused. Visible regular root files other than `profiles.yml` are
ignored. The root itself must contain no symbolic-link component.

The package directory name is diagnostic only. The sealed `package.name` in the
recipe is package identity. Package-local files are not traversed or verified by
catalog acquisition.

# TWO-PASS RULE

All configured `profiles.yml` documents are parsed before any recipe is sealed.
Their declarations form one global profile namespace. Profiles do not shadow by
collection precedence.

# STABILITY

Callers must provide quiescent roots for one acquisition. The adapter detects
common size-changing reads but does not create a filesystem snapshot.

# COMPATIBILITY

The layout does not define Pkgfiles, `pkgman.conf`, Git branches, collection
discovery, architecture fallback, requirement resolution, or installed state.
Translation belongs in a separate migration or configuration frontend.

# SEE ALSO

`pkgcatalog_acquire(3)`, `pkgcatalog-scan(1)`, `pkgsource_profiles_yaml(5)`,
`pkgsource_recipe_yaml(5)`
