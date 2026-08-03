#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
from __future__ import annotations

import argparse
import html
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


PROJECT = "libpkgcatalog-acquire"
DOCUMENTS = (
    ("README.md", "index.html", "libpkgcatalog-acquire"),
    ("HISTORY.md", "history.html", "Project history"),
    ("CONTRIBUTING.md", "contributing.html", "Contributing"),
    ("MAINTAINING.md", "maintaining.html", "Maintaining"),
    ("docs/architecture.md", "architecture.html", "Architecture"),
    ("docs/acquisition.md", "acquisition.html", "Acquisition pipeline"),
    ("docs/integration.md", "integration.html", "Ecosystem integration"),
    ("docs/abi.md", "abi.html", "ELF ABI policy"),
    ("docs/code-style.md", "code-style.html", "Code style"),
    ("docs/testing.md", "testing.html", "Testing"),
    ("docs/manpage-markdown.md", "manpage-markdown.html", "Manual-page Markdown"),
    ("docs/html.md", "html.html", "HTML documentation"),
    ("docs/protocols/collection-layout-v1.md", "protocols/collection-layout-v1.html", "Collection layout version 1"),
    ("docs/history/catalog-2.1-extraction.md", "history/catalog-2.1-extraction.html", "Catalog 2.1 extraction"),
    ("docs/man/pkgcatalog-scan.1.md", "man/pkgcatalog-scan.1.html", "pkgcatalog-scan(1)"),
    ("docs/man/pkgcatalog_acquire.3.md", "man/pkgcatalog_acquire.3.html", "pkgcatalog_acquire(3)"),
    ("docs/man/pkgcatalog_collection_layout.5.md", "man/pkgcatalog_collection_layout.5.html", "pkgcatalog_collection_layout(5)"),
)


def fail(message: str) -> "NoReturn":
    raise SystemExit(f"build-html-docs: {message}")


def run(command: list[str], *, cwd: Path | None = None, stdin: str | None = None) -> None:
    completed = subprocess.run(
        command,
        cwd=cwd,
        input=stdin,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        fail(f"command failed ({completed.returncode}): {' '.join(command)}")


def pandoc_version(pandoc: str) -> str:
    completed = subprocess.run(
        [pandoc, "--version"], text=True, capture_output=True, check=False
    )
    if completed.returncode != 0:
        fail(f"cannot execute Pandoc: {pandoc}")
    first = completed.stdout.splitlines()[0] if completed.stdout else ""
    match = re.fullmatch(r"pandoc (\d+)\.(\d+)(?:\..*)?", first)
    if match is None:
        fail(f"cannot parse Pandoc version: {first}")
    major, minor = (int(value) for value in match.groups())
    if major != 3 or minor < 1:
        fail(f"Pandoc 3.1 through 3.x is required; found {first.removeprefix('pandoc ')}")
    return first.removeprefix("pandoc ")


def relative_link(page: Path, target: str) -> str:
    return os.path.relpath(target, page.parent.as_posix()).replace(os.sep, "/")


def navigation(page: Path, version: str) -> str:
    links = (
        ("Home", "index.html"),
        ("Architecture", "architecture.html"),
        ("ABI", "abi.html"),
        ("Manual", "man/pkgcatalog_acquire.3.html"),
        ("API", "api/index.html"),
        ("History", "history.html"),
    )
    items = "\n".join(
        f'<a href="{html.escape(relative_link(page, target))}">{html.escape(label)}</a>'
        for label, target in links
    )
    return (
        '<nav class="house-nav">\n'
        f'<a class="project" href="{html.escape(relative_link(page, "index.html"))}">'
        f"{PROJECT} {html.escape(version)}</a>\n"
        f"{items}\n"
        "</nav>\n"
    )


def footer(version: str) -> str:
    return (
        '<footer class="house-footer">'
        f"Generated from {PROJECT} {html.escape(version)} authoritative sources."
        "</footer>\n"
    )


def render_markdown(
    pandoc: str, source_root: Path, output_root: Path, version: str
) -> None:
    with tempfile.TemporaryDirectory(prefix="libpkgcatalog-acquire-html-") as temp_name:
        temp = Path(temp_name)
        for source_name, output_name, title in DOCUMENTS:
            source = source_root / source_name
            if not source.is_file():
                fail(f"missing Markdown source: {source_name}")
            output = output_root / output_name
            output.parent.mkdir(parents=True, exist_ok=True)
            nav = temp / "nav.html"
            page = Path(output_name)
            nav.write_text(navigation(page, version), encoding="utf-8", newline="\n")
            tail = temp / "footer.html"
            tail.write_text(footer(version), encoding="utf-8", newline="\n")
            css = relative_link(page, "assets/house.css")
            run(
                [
                    pandoc,
                    "--from=markdown-smart",
                    "--to=html5",
                    "--standalone",
                    "--fail-if-warnings",
                    "--eol=lf",
                    "--wrap=none",
                    "--no-highlight",
                    f"--metadata=pagetitle:{title}",
                    f"--css={css}",
                    f"--include-before-body={nav}",
                    f"--include-after-body={tail}",
                    str(source),
                    "--output",
                    str(output),
                ]
            )


def render_doxygen(doxygen: str, source_root: Path, output_root: Path, version: str) -> None:
    base = source_root / "Doxyfile"
    if not base.is_file():
        fail("missing Doxyfile")
    configuration = base.read_text(encoding="utf-8") + "\n" + "\n".join(
        [
            f"PROJECT_NUMBER = {version}",
            f"OUTPUT_DIRECTORY = {output_root}",
            f"INPUT = {source_root / 'include'}",
            "FULL_PATH_NAMES = NO",
            f"STRIP_FROM_PATH = {source_root}",
            "GENERATE_HTML = YES",
            "HTML_OUTPUT = api",
            "GENERATE_LATEX = NO",
            f"HTML_EXTRA_STYLESHEET = {source_root / 'docs/assets/doxygen-extra.css'}",
        ]
    ) + "\n"
    run([doxygen, "-"], cwd=source_root, stdin=configuration)
    if not (output_root / "api/index.html").is_file():
        fail("Doxygen did not produce api/index.html")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--project-version", required=True)
    parser.add_argument("--pandoc", required=True)
    parser.add_argument("--doxygen", required=True)
    parser.add_argument("--checker", type=Path, required=True)
    parser.add_argument("--stamp", type=Path, required=True)
    args = parser.parse_args()

    source_root = args.source_root.resolve()
    output_dir = args.output_dir.resolve()
    stamp = args.stamp.resolve()
    pandoc_version(args.pandoc)

    output_dir.parent.mkdir(parents=True, exist_ok=True)
    temporary = Path(
        tempfile.mkdtemp(prefix=f".{PROJECT}-{args.project_version}-", dir=output_dir.parent)
    )
    try:
        assets = temporary / "assets"
        assets.mkdir(parents=True)
        shutil.copy2(source_root / "docs/assets/house.css", assets / "house.css")
        shutil.copy2(
            source_root / "docs/assets/doxygen-extra.css",
            assets / "doxygen-extra.css",
        )
        legal = temporary / "legal"
        legal.mkdir()
        shutil.copy2(source_root / "COPYING", legal / "COPYING")
        shutil.copy2(source_root / "COPYRIGHT", legal / "COPYRIGHT")

        render_markdown(args.pandoc, source_root, temporary, args.project_version)
        render_doxygen(args.doxygen, source_root, temporary, args.project_version)
        run(
            [
                sys.executable,
                str(args.checker),
                str(temporary),
                "--forbid-path",
                str(source_root),
                "--forbid-path",
                str(output_dir.parent),
            ]
        )

        if output_dir.exists():
            shutil.rmtree(output_dir)
        temporary.rename(output_dir)
        stamp.parent.mkdir(parents=True, exist_ok=True)
        stamp.write_text(f"{PROJECT} {args.project_version}\n", encoding="utf-8")
    except BaseException:
        shutil.rmtree(temporary, ignore_errors=True)
        raise

    return 0


if __name__ == "__main__":
    sys.exit(main())
