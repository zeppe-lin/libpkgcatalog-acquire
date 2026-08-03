# Extraction from libpkgcatalog 2.1

`libpkgcatalog 2.1.0` shipped `libpkgcatalog-acquire` and `pkgcatalog-scan` in the
catalog repository. That topology made the semantic catalog owner carry
filesystem and YAML dependencies even though the acquisition body had its own
public library, headers, tests, manual pages, and failure domain.

The 3.0 authority reset extracted those bodies into this repository. The
adapter now depends on standalone `libpkgsource-yaml 1.0.0` and
`libpkgcatalog 3.0.0`. The native layout and two-pass behavior were retained;
source-dependent catalog identities remain owned by catalog/source 3.

No compatibility parser, record translator, collection discovery layer, or old
identity bridge was invented during extraction.
