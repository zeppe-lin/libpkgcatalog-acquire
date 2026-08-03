pkgcatalog-scan(1)

# NAME

pkgcatalog-scan - validate and inspect explicit native package collections

# SYNOPSIS

*pkgcatalog-scan* [*--max-document-bytes* _N_] \
                 *--collection* _NAME_=_ROOT_ [*--collection* _NAME_=_ROOT_ ...] \
                [*--external-revision* _NAME_=_REVISION_ ...]

# DESCRIPTION

*pkgcatalog-scan* is the reference frontend for *libpkgcatalog-acquire*. It
assigns contiguous collection precedence from explicit command-line order,
acquires the native collection layout, and prints the catalog identity,
collection revision identities, and every effective or shadowed candidate.

The output is diagnostic text rather than a persistent catalog serialization or
resolver protocol.

# OPTIONS

*--collection* _NAME_=_ROOT_
	Add one explicit collection. Repetition order defines precedence position
	starting at zero.

*--external-revision* _NAME_=_REVISION_
	Retain an optional external revision label as diagnostic provenance. The
	name must match exactly one configured collection.

*--max-document-bytes* _N_
	Set a positive canonical decimal byte limit for each YAML document. The
	default is 1048576.

*--version*
	Print the program version.

*--help*
	Print usage.

# EXIT STATUS

Zero indicates a successfully sealed catalog. Non-zero indicates option,
filesystem, YAML, source-authority, or catalog-authority failure.

# SEE ALSO

*pkgcatalog_acquire*(3), *pkgcatalog_collection_layout*(5),
*libpkgcatalog*(3)
