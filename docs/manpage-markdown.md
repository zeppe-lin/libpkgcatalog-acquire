# Manual-page Markdown

## Purpose

Manual pages are authored in a restricted Markdown profile and converted to
`man(7)` roff with Pandoc. Markdown is canonical. Generated roff is committed as
derived release material so ordinary builds do not require Pandoc.

## Canonical and generated files

Canonical sources live in `docs/man/*.md`. Generated pages live in
`docs/man/generated/` and retain their installed section suffix.

Do not edit generated roff directly. Use:

```sh
tools/update-man-pages.sh --write
tools/update-man-pages.sh --check
```

Configured Meson trees expose equivalent targets when Pandoc is available.

## Document header

Each source starts with one Pandoc title block:

```text
% PAGE_NAME(SECTION) libpkgcatalog-acquire VERSION | libpkgcatalog-acquire
```

The `NAME` section carries the lowercase installed name and one concise purpose
statement separated by an ASCII hyphen.

## Allowed Markdown

The profile permits paragraphs, lists, definition lists, fenced code blocks,
inline code, and semantic emphasis.

Use inline code for identifiers, values, paths, header names, libraries, and
manual references. Use fenced blocks for exact declarations, shell commands,
and protocol examples.

## Forbidden Markdown

Manual sources must not contain raw roff, raw HTML, Setext headings, horizontal
rules, tables, footnotes, citations, images, task lists, automatic or reference
links, tabs, or trailing whitespace.

## Conversion contract

The generator uses Pandoc 3.1 through 3.x with:

```text
--from=markdown-smart
--to=man
--standalone
--fail-if-warnings
--eol=lf
--wrap=none
--syntax-highlighting=none  # Pandoc versions that provide it
--no-highlight               # older supported Pandoc 3.x versions
```

The generator selects the non-deprecated highlighting option supported by the
installed Pandoc 3.x release. Project-owned canonicalization removes
writer-added font escapes inside exact examples and normalizes equivalent
two-character roff special-character spellings, including bullets, quotes, and
`@`. A new Pandoc major version requires explicit compatibility review.

Generated roff is compared byte-for-byte after canonicalization and must pass
`mandoc -Tlint`.

## Review rules

Review Markdown and generated roff in the same commit. Generated files are
reproducible release artifacts, not independent documentation authority.
