// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "support/acquisition_fixture.h"

#include <libpkgsource/error.h>

#include <cassert>
#include <filesystem>
#include <string>

#include <libpkgcatalog-acquire/acquire.h>
#include <libpkgcatalog/error.h>

namespace {

using test_support::recipe;
using test_support::specification;
using test_support::temporary_tree;
using test_support::write;

template <class Function>
void expect_source(pkgsource::error_code code, Function function)
{
  try {
    function();
  } catch (const pkgsource::error& value) {
    assert(value.code() == code);
    return;
  }
  assert(false);
}

template <class Function>
void expect_catalog(pkgcatalog::error_code code, Function function)
{
  try {
    function();
  } catch (const pkgcatalog::error& value) {
    assert(value.code() == code);
    return;
  }
  assert(false);
}

void populate_valid(const std::filesystem::path& parent)
{
  const std::filesystem::path core = parent / "core";
  const std::filesystem::path system = parent / "system";
  write(core / "profiles.yml",
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  compiler:\n"
        "    members:\n"
        "      - package: gcc\n");
  write(system / "profiles.yml",
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  toolchain:\n"
        "    members:\n"
        "      - profile: \"@compiler\"\n"
        "      - package: binutils\n");
  write(core / "not-the-package-name" / "recipe.yml",
        recipe("alpha", "1.0", "@toolchain"));
  write(system / "alpha-new" / "recipe.yml", recipe("alpha", "2.0"));
  write(system / "beta" / "recipe.yml", recipe("beta", "1.0"));
  std::filesystem::create_directories(core / ".git");
  write(core / "README.md", "diagnostic repository material\n");
}

void test_two_pass_acquisition()
{
  temporary_tree tree;
  populate_valid(tree.root());
  const auto first = pkgcatalog::acquire::acquire_catalog({
      specification(1, "system", tree.root() / "system", "system-rev"),
      specification(0, "core", tree.root() / "core", "core-rev"),
  });
  assert(first.collections().size() == 2);
  assert(first.profiles().profiles().size() == 2);
  assert(first.require(pkgsource::package_reference("alpha"))
             .release()
             .version() == "1.0");
  const auto alpha =
      first.candidates_for(pkgsource::package_reference("alpha"));
  assert(alpha.size() == 2);
  assert(alpha[0].status() == pkgcatalog::candidate_status::effective);
  assert(alpha[1].status() == pkgcatalog::candidate_status::shadowed);
  assert(alpha[1].shadowed_by() == alpha[0].identity());
  assert(
      first.require(pkgsource::package_reference("beta")).collection().name() ==
      "system");
  assert(first.require(pkgsource::package_reference("alpha"))
             .source()
             .origin()
             .document()
             .find("not-the-package-name") != std::string::npos);

  temporary_tree mirror;
  populate_valid(mirror.root());
  const auto second = pkgcatalog::acquire::acquire_catalog({
      specification(0, "core", mirror.root() / "core", "different"),
      specification(1, "system", mirror.root() / "system", std::nullopt),
  });
  assert(first.identity() == second.identity());
  assert(first.collections()[0].collection().revision_identity() ==
         second.collections()[0].collection().revision_identity());
}

void test_authority_failures()
{
  temporary_tree tree;
  const auto first = tree.root() / "first";
  const auto second = tree.root() / "second";
  write(first / "profiles.yml",
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  compiler:\n"
        "    members: [{package: gcc}]\n");
  write(second / "profiles.yml",
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  compiler:\n"
        "    members: [{package: clang}]\n");
  expect_source(pkgsource::error_code::duplicate_declaration, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "first", first), specification(1, "second", second)});
  });

  std::filesystem::remove(second / "profiles.yml");
  write(first / "a" / "recipe.yml", recipe("duplicate", "1.0"));
  write(first / "b" / "recipe.yml", recipe("duplicate", "2.0"));
  expect_catalog(pkgcatalog::error_code::duplicate_candidate, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "first", first)});
  });
}

void test_source_semantics_reach_catalog_identity()
{
  temporary_tree tree;
  const auto root = tree.root() / "core";
  const auto document = root / "alpha" / "recipe.yml";
  write(document, recipe("alpha", "1.0", {}, "true"));

  const auto first = pkgcatalog::acquire::acquire_catalog(
      {specification(0, "core", root, "diagnostic")});
  const auto& first_candidate =
      first.require(pkgsource::package_reference("alpha"));

  write(document, recipe("alpha", "1.0", {}, "printf changed"));
  const auto second = pkgcatalog::acquire::acquire_catalog(
      {specification(0, "core", root, "diagnostic")});
  const auto& second_candidate =
      second.require(pkgsource::package_reference("alpha"));

  assert(first_candidate.release().identity() ==
         second_candidate.release().identity());
  assert(first_candidate.source().identity() !=
         second_candidate.source().identity());
  assert(first_candidate.identity() != second_candidate.identity());
  assert(first.collections()[0].collection().revision_identity() !=
         second.collections()[0].collection().revision_identity());
  assert(first.identity() != second.identity());
}

} // namespace

int main()
{
  test_two_pass_acquisition();
  test_authority_failures();
  test_source_semantics_reach_catalog_identity();
}
