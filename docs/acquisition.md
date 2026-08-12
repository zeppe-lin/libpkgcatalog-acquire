# Acquisition pipeline

## Request admission

`acquire_catalog()` requires at least one explicit specification. Specifications
are sorted by their numeric precedence and must then occupy every position from
zero. Canonical collection names and normalized absolute roots must be unique. Optional
external revision provenance must be non-empty and line-safe, including no
embedded NUL byte.

Each root is lexically normalized and compared with its canonical path. A root
containing a symbolic-link component, naming a symbolic link, or not naming a
directory is refused. Directory-open and iteration failures are translated into
the adapter filesystem failure domain.

## Pass one: global profiles

For every collection in precedence order, the adapter checks only the optional
root `profiles.yml`. When present it must be a regular non-symlink file within
the configured byte ceiling. Its exact bytes are parsed through
`pkgsource::yaml::parse_profiles_yaml()`.

All declarations from all configured collections are then submitted together to
`pkgsource::profile_catalog::seal()`. Profiles therefore form one global
namespace; collection precedence does not shadow profile declarations.

## Pass two: recipes

Immediate hidden root entries are ignored. Every visible directory is a package
acquisition entry and must contain `recipe.yml`. Visible symbolic links and
unsupported file types are refused. Visible regular root files other than
`profiles.yml` are ignored.

Package directories are sorted by filename for deterministic observation. Their
directory names are not package identity. Each exact recipe byte sequence is
parsed through `pkgsource::yaml::parse_recipe_yaml()` and passed with its source
origin to `pkgsource::seal_source()` against the already sealed global profile
catalog.

## Catalog handoff

Sealed source snapshots are placed in a `collection_declaration` with caller
name and diagnostic provenance, then submitted to `seal_collection()`. The
resulting collection revisions retain caller precedence and are submitted to
`catalog_snapshot::seal()`.

The adapter does not recreate package, collection, candidate, or catalog
identity algorithms. Every semantic decision after observation is delegated to
the owning library.

## Read stability and limits

A document is first observed without following its final component. Linked or
non-regular documents are refused before stream opening; regular documents are
then sized, opened in binary mode, read to the observed size, and checked for a
trailing byte. Oversize, short, or failed reads are refused. This detects common concurrent mutation but is not a
filesystem snapshot guarantee. Callers must provide quiescent roots for one
acquisition.

The adapter maps its per-document byte limit into the YAML parser byte ceiling.
Other parser limits remain owned by `libpkgsource-yaml` defaults.
