// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgcatalog-acquire/acquire.h>
#include <libpkgcatalog/error.h>
#include <libpkgsource/error.h>

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

namespace {

class temporary_tree final {
public:
  temporary_tree()
  {
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    root_ = std::filesystem::temp_directory_path() /
            ("libpkgcatalog-acquire-" + std::to_string(stamp));
    std::filesystem::create_directories(root_);
  }
  ~temporary_tree()
  {
    std::error_code ignored;
    std::filesystem::remove_all(root_, ignored);
  }
  const std::filesystem::path& root() const noexcept { return root_; }
private:
  std::filesystem::path root_;
};

void write(const std::filesystem::path& path, const std::string& bytes)
{
  std::filesystem::create_directories(path.parent_path());
  std::ofstream stream(path, std::ios::binary);
  assert(stream);
  stream << bytes;
  assert(stream.good());
}

std::string recipe(const std::string& name, const std::string& version,
                   const std::string& build_requirement = {})
{
  return "format: zeppe-lin.recipe/1\n"
         "package:\n"
         "  name: " + name + "\n"
         "  version: " + version + "\n"
         "  release: 1\n"
         "  summary: Native package\n"
         "  licenses: [GPL-3.0-or-later]\n"
         "requirements:\n" +
         (build_requirement.empty()
              ? std::string("  build: []\n")
              : "  build:\n    - profile: \"" + build_requirement + "\"\n") +
         "sources: []\n"
         "build:\n"
         "  language: posix-shell\n"
         "  script: |\n"
         "    true\n";
}

pkgsource::declaration_provenance provenance(std::size_t index)
{
  return pkgsource::declaration_provenance(
      "catalog.conf", "collections[" + std::to_string(index) + "]", 1,
      static_cast<std::uint32_t>(index + 1));
}

pkgcatalog::acquire::collection_specification specification(
    std::uint32_t precedence, const std::string& name,
    const std::filesystem::path& root,
    std::optional<std::string> revision = std::nullopt)
{
  return pkgcatalog::acquire::collection_specification(
      precedence, pkgcatalog::collection_reference(name), root,
      std::move(revision), provenance(precedence));
}

template<class Function>
void expect_acquire(pkgcatalog::acquire::error_code code, Function function)
{
  try {
    function();
  } catch (const pkgcatalog::acquire::error& value) {
    assert(value.code() == code);
    return;
  }
  assert(false);
}

template<class Function>
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

template<class Function>
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
             .release().version() == "1.0");
  const auto alpha = first.candidates_for(pkgsource::package_reference("alpha"));
  assert(alpha.size() == 2);
  assert(alpha[0].status() == pkgcatalog::candidate_status::effective);
  assert(alpha[1].status() == pkgcatalog::candidate_status::shadowed);
  assert(alpha[1].shadowed_by() == alpha[0].identity());
  assert(first.require(pkgsource::package_reference("beta"))
             .collection().name() == "system");
  assert(first.require(pkgsource::package_reference("alpha"))
             .source().origin().document().find("not-the-package-name") !=
         std::string::npos);

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

void test_request_and_layout_failures()
{
  temporary_tree tree;
  populate_valid(tree.root());
  expect_acquire(pkgcatalog::acquire::error_code::invalid_request, [&] {
    (void)pkgcatalog::acquire::acquire_catalog({});
  });
  expect_acquire(pkgcatalog::acquire::error_code::invalid_request, [&] {
    (void)pkgcatalog::acquire::acquire_catalog({
        specification(1, "core", tree.root() / "core")});
  });
  expect_acquire(pkgcatalog::acquire::error_code::invalid_request, [&] {
    (void)pkgcatalog::acquire::acquire_catalog({
        specification(0, "core", tree.root() / "core"),
        specification(1, "other", tree.root() / "core")});
  });

  std::filesystem::create_directories(tree.root() / "core" / "orphan");
  expect_acquire(pkgcatalog::acquire::error_code::unsupported_entry, [&] {
    (void)pkgcatalog::acquire::acquire_catalog({
        specification(0, "core", tree.root() / "core")});
  });
  std::filesystem::remove_all(tree.root() / "core" / "orphan");

  expect_acquire(pkgcatalog::acquire::error_code::document_too_large, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "core", tree.root() / "core")},
        pkgcatalog::acquire::limits(8));
  });
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
    (void)pkgcatalog::acquire::acquire_catalog({
        specification(0, "first", first),
        specification(1, "second", second)});
  });

  std::filesystem::remove(second / "profiles.yml");
  write(first / "a" / "recipe.yml", recipe("duplicate", "1.0"));
  write(first / "b" / "recipe.yml", recipe("duplicate", "2.0"));
  expect_catalog(pkgcatalog::error_code::duplicate_candidate, [&] {
    (void)pkgcatalog::acquire::acquire_catalog({
        specification(0, "first", first)});
  });
}

void test_symlink_rejection()
{
  temporary_tree tree;
  const auto root = tree.root() / "core";
  write(root / "real.yml", recipe("alpha", "1.0"));
  std::filesystem::create_directories(root / "alpha");
  std::error_code ec;
  std::filesystem::create_symlink(root / "real.yml", root / "alpha" / "recipe.yml", ec);
  if (!ec) {
    expect_acquire(pkgcatalog::acquire::error_code::unsupported_entry, [&] {
      (void)pkgcatalog::acquire::acquire_catalog({
          specification(0, "core", root)});
    });
  }

  const auto actual = tree.root() / "actual";
  std::filesystem::create_directories(actual);
  const auto linked = tree.root() / "linked";
  ec.clear();
  std::filesystem::create_directory_symlink(actual, linked, ec);
  if (!ec) {
    expect_acquire(pkgcatalog::acquire::error_code::invalid_root, [&] {
      (void)pkgcatalog::acquire::acquire_catalog({
          specification(0, "linked", linked)});
    });
  }
}

} // namespace

int main()
{
  test_two_pass_acquisition();
  test_request_and_layout_failures();
  test_authority_failures();
  test_symlink_rejection();
}
