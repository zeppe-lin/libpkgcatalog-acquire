# Native collection layout, version 1

Version one defines the filesystem observation protocol accepted by
`libpkgcatalog-acquire 3.0.0`. It is not catalog authority and does not define
package identity.

## Layout

```text
collection-root/
├── profiles.yml                 optional
├── package-directory/
│   ├── recipe.yml               required
│   └── ...                      package-local material not inspected here
└── another-package/
    └── recipe.yml
```

Every collection is named, rooted, and positioned by an explicit caller
specification. The root must be a real directory with no symbolic-link
component.

Immediate hidden entries are ignored. Every immediate visible directory is a
package entry and must contain a regular, non-symlink `recipe.yml`. Visible
symbolic links and unsupported file types are refused. Visible regular root
files other than `profiles.yml` are ignored.

The package directory name is diagnostic only. `package.name` inside the sealed
recipe is package identity. Nested package directories are not traversed and
recipe-declared local source inputs are not verified.

## Global two-pass rule

All existing `profiles.yml` documents are parsed before any recipe. Their
profile declarations are sealed into one global namespace. Recipes are then
parsed and sealed against that complete profile catalog. Collection precedence
never shadows profile declarations.

## Documents

Documents are exact byte sequences subject to a positive caller-selected
per-document ceiling. They must be regular non-symlink files. The syntax and
grammar protocols are owned by `libpkgsource-yaml`.

## Exclusions

Version one does not define collection discovery, package-manager
configuration, Git state, branches, timestamps, caches, Pkgfiles, architecture
fallback, dependency resolution, installed state, or source-file verification.
