// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <cassert>
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>

#include <libpkgcatalog-acquire/libpkgcatalog-acquire.h>

int main()
{
  const auto stamp =
      std::chrono::steady_clock::now().time_since_epoch().count();
  const std::filesystem::path root =
      std::filesystem::temp_directory_path() /
      ("libpkgcatalog-acquire-installed-" + std::to_string(stamp));
  std::filesystem::create_directories(root);

  try {
    const pkgcatalog::catalog_snapshot snapshot =
        pkgcatalog::acquire::acquire_catalog({
            pkgcatalog::acquire::collection_specification(
                0,
                pkgcatalog::collection_reference("core"),
                root,
                std::nullopt,
                pkgsource::declaration_provenance(
                    "installed-consumer", "collections[0]", 1, 1)),
        });
    assert(snapshot.collections().size() == 1);
    assert(snapshot.candidates().empty());
  } catch (...) {
    std::filesystem::remove_all(root);
    throw;
  }
  std::filesystem::remove_all(root);
}
