// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <cassert>
#include <cstdint>
#include <optional>
#include <string>

#include <libpkgcatalog-acquire/acquire.h>

namespace {

template <class Function>
void expect_invalid_request(Function function)
{
  try {
    function();
  } catch (const pkgcatalog::acquire::error& value) {
    assert(value.code() == pkgcatalog::acquire::error_code::invalid_request);
    return;
  }
  assert(false);
}

pkgsource::declaration_provenance provenance()
{
  return pkgsource::declaration_provenance(
      "catalog.conf", "collections[0]", 1, 1);
}

void test_request_values()
{
  pkgcatalog::acquire::collection_specification specification(
      7,
      pkgcatalog::collection_reference("core"),
      "/tmp/core",
      std::optional<std::string>("revision"),
      provenance());
  assert(specification.precedence() == 7);
  assert(specification.name().name() == "core");
  assert(specification.root() == "/tmp/core");
  assert(specification.external_revision() == "revision");
  assert(specification.declaration() == provenance());

  const pkgcatalog::acquire::limits limits(4096);
  assert(limits.max_document_bytes() == 4096);
}

void test_invalid_request_values()
{
  expect_invalid_request([&] {
    (void)pkgcatalog::acquire::collection_specification(
        0,
        pkgcatalog::collection_reference("core"),
        {},
        std::nullopt,
        provenance());
  });
  expect_invalid_request([&] { (void)pkgcatalog::acquire::limits(0); });

  for (const std::string& revision : {
           std::string(),
           std::string("bad\nrevision"),
           std::string("bad\rrevision"),
           std::string("bad\0revision", 12),
       }) {
    expect_invalid_request([&] {
      (void)pkgcatalog::acquire::collection_specification(
          0,
          pkgcatalog::collection_reference("core"),
          "/tmp/core",
          std::optional<std::string>(revision),
          provenance());
    });
  }
}

void test_error_names()
{
  using pkgcatalog::acquire::error_code;
  assert(pkgcatalog::acquire::to_string(error_code::invalid_request) ==
         "invalid-request");
  assert(pkgcatalog::acquire::to_string(error_code::invalid_root) ==
         "invalid-root");
  assert(pkgcatalog::acquire::to_string(error_code::filesystem_failure) ==
         "filesystem-failure");
  assert(pkgcatalog::acquire::to_string(error_code::unsupported_entry) ==
         "unsupported-entry");
  assert(pkgcatalog::acquire::to_string(error_code::document_too_large) ==
         "document-too-large");
  assert(pkgcatalog::acquire::to_string(error_code::document_read_failed) ==
         "document-read-failed");
  assert(pkgcatalog::acquire::to_string(static_cast<error_code>(255)) ==
         "unknown");
}

} // namespace

int main()
{
  test_request_values();
  test_invalid_request_values();
  test_error_names();
}
