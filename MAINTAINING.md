# Maintaining libpkgcatalog-acquire

## Dependency discipline

Release against signed compatible `libpkgcatalog` and `libpkgsource-yaml` tags.
Pkg-config must publish `libpkgcatalog` exactly once in `Requires` and retain
`libpkgsource-yaml` exactly once in `Requires.private`. Public headers must not
expose YAML-provider types.

## Layout discipline

The collection layout is an observation protocol, not catalog authority. A new
layout generation requires a complete protocol document, explicit selection
mechanism, compatibility policy, migration guidance, and accepted/rejected
fixtures. Do not infer a generation from path shape.

Filesystem enumeration order is never precedence authority. Collection names,
roots, precedence, and external revision provenance remain caller supplied.

## Generated documentation

Root project documents and Markdown under `docs/` are canonical. Regenerate
committed roff with
`tools/update-man-pages.sh --write`. HTML is a versioned derived artifact built
from canonical Markdown and installed public headers.

## Release checklist

1. Build shared and static closures with GCC and Clang.
2. Run all tests with warnings as errors.
3. Run GCC and Clang ASan/UBSan jobs.
4. Run clang-format 17, Doxygen, Pandoc regeneration, mandoc lint, and HTML link
   validation.
5. Inspect pkg-config, SONAME, `NEEDED`, and the exact ABI manifest.
6. Test installed shared and static consumers and `pkgcatalog-scan`.
7. Stage `doc`, `man`, and `html-docs` installation through `DESTDIR`.
8. Re-audit the boundary against exact catalog and YAML dependency tags.
9. Replay the release series independently and compare the final tree.
