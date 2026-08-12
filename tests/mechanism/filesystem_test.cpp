// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "support/acquisition_fixture.h"

#include <cassert>
#include <csignal>
#include <filesystem>

#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <libpkgcatalog-acquire/acquire.h>

namespace {

using test_support::recipe;
using test_support::specification;
using test_support::temporary_tree;
using test_support::write;

template <class Function>
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

template <class Function>
void expect_fast_acquire(pkgcatalog::acquire::error_code code, Function function)
{
  const pid_t child = ::fork();
  assert(child >= 0);
  if (child == 0) {
    ::alarm(2);
    try {
      function();
    } catch (const pkgcatalog::acquire::error& value) {
      ::_exit(value.code() == code ? 0 : 2);
    } catch (...) {
      ::_exit(3);
    }
    ::_exit(4);
  }

  int status = 0;
  assert(::waitpid(child, &status, 0) == child);
  assert(WIFEXITED(status));
  assert(WEXITSTATUS(status) == 0);
}

void populate(const std::filesystem::path& root)
{
  write(root / "alpha" / "recipe.yml", recipe("alpha", "1.0"));
}

void test_request_and_layout_refusal()
{
  temporary_tree tree;
  const auto core = tree.root() / "core";
  populate(core);

  expect_acquire(pkgcatalog::acquire::error_code::invalid_request, [&] {
    (void)pkgcatalog::acquire::acquire_catalog({});
  });
  expect_acquire(pkgcatalog::acquire::error_code::invalid_request, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(1, "core", core)});
  });
  expect_acquire(pkgcatalog::acquire::error_code::invalid_request, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "core", core), specification(1, "other", core)});
  });

  const auto other = tree.root() / "other";
  populate(other);
  expect_acquire(pkgcatalog::acquire::error_code::invalid_request, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "core", core), specification(1, "core", other)});
  });

  std::filesystem::create_directories(core / "orphan");
  expect_acquire(pkgcatalog::acquire::error_code::unsupported_entry, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "core", core)});
  });
  std::filesystem::remove_all(core / "orphan");

  expect_acquire(pkgcatalog::acquire::error_code::document_too_large, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "core", core)}, pkgcatalog::acquire::limits(8));
  });
}

void test_symlink_refusal()
{
  temporary_tree tree;
  const auto root = tree.root() / "core";
  write(root / "real.yml", recipe("alpha", "1.0"));
  std::filesystem::create_directories(root / "alpha");

  std::error_code ec;
  std::filesystem::create_symlink(
      root / "real.yml", root / "alpha" / "recipe.yml", ec);
  if (!ec) {
    expect_acquire(pkgcatalog::acquire::error_code::unsupported_entry, [&] {
      (void)pkgcatalog::acquire::acquire_catalog(
          {specification(0, "core", root)});
    });
  }

  const auto actual = tree.root() / "actual";
  std::filesystem::create_directories(actual);
  const auto linked = tree.root() / "linked";
  ec.clear();
  std::filesystem::create_directory_symlink(actual, linked, ec);
  if (!ec) {
    expect_acquire(pkgcatalog::acquire::error_code::invalid_root, [&] {
      (void)pkgcatalog::acquire::acquire_catalog(
          {specification(0, "linked", linked)});
    });
  }
}

void test_special_document_refusal_is_nonblocking()
{
  temporary_tree tree;

  const auto profiles_root = tree.root() / "profiles";
  std::filesystem::create_directories(profiles_root);
  const auto profiles = profiles_root / "profiles.yml";
  assert(::mkfifo(profiles.c_str(), 0600) == 0);
  expect_fast_acquire(pkgcatalog::acquire::error_code::unsupported_entry, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "profiles", profiles_root)});
  });

  const auto recipe_root = tree.root() / "recipe";
  std::filesystem::create_directories(recipe_root / "alpha");
  const auto recipe_path = recipe_root / "alpha" / "recipe.yml";
  assert(::mkfifo(recipe_path.c_str(), 0600) == 0);
  expect_fast_acquire(pkgcatalog::acquire::error_code::unsupported_entry, [&] {
    (void)pkgcatalog::acquire::acquire_catalog(
        {specification(0, "recipe", recipe_root)});
  });
}

} // namespace

int main()
{
  test_request_and_layout_refusal();
  test_symlink_refusal();
  test_special_document_refusal_is_nonblocking();
}
