% PKGCATALOG-SCAN(1) libpkgcatalog-acquire 3.0.0 | libpkgcatalog-acquire

# NAME

pkgcatalog-scan - validate and inspect explicit native package collections

# SYNOPSIS

```text
pkgcatalog-scan [--max-document-bytes N]
  --collection NAME=ROOT [--collection NAME=ROOT ...]
  [--external-revision NAME=REVISION ...]
```

# DESCRIPTION

`pkgcatalog-scan` is the reference client for `libpkgcatalog-acquire`. Repeated
`--collection` arguments receive contiguous precedence in command-line order.
The command acquires the native layout and prints the catalog identity,
collection revision identities, and effective and shadowed candidates.

The output is diagnostic text. It is not a durable catalog record, resolver
protocol, or package-manager configuration format.

# OPTIONS

`--collection NAME=ROOT`
: Add one explicit collection. Repetition order defines precedence from zero.

`--external-revision NAME=REVISION`
: Attach diagnostic revision provenance to one configured collection.

`--max-document-bytes N`
: Set a positive canonical decimal byte ceiling for each YAML document. The
  default is 1048576.

`--version`
: Print the program version.

`--help`
: Print usage.

# EXIT STATUS

Zero indicates a successfully sealed catalog. Non-zero indicates option,
filesystem, YAML, source-authority, or catalog-authority failure.

# SEE ALSO

`pkgcatalog_acquire(3)`, `pkgcatalog_collection_layout(5)`, `libpkgcatalog(3)`
