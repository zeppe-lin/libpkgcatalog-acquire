// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <utility>

#include <libpkgcatalog-acquire/acquire.h>

namespace test_support {

class temporary_tree final {
public:
  temporary_tree()
  {
    const auto stamp =
        std::chrono::steady_clock::now().time_since_epoch().count();
    root_ = std::filesystem::temp_directory_path() /
            ("libpkgcatalog-acquire-" + std::to_string(stamp));
    std::filesystem::create_directories(root_);
  }

  ~temporary_tree()
  {
    std::error_code ignored;
    std::filesystem::remove_all(root_, ignored);
  }

  temporary_tree(const temporary_tree&) = delete;
  temporary_tree& operator=(const temporary_tree&) = delete;

  [[nodiscard]] const std::filesystem::path& root() const noexcept
  {
    return root_;
  }

private:
  std::filesystem::path root_;
};

inline void write(const std::filesystem::path& path, const std::string& bytes)
{
  std::filesystem::create_directories(path.parent_path());
  std::ofstream stream(path, std::ios::binary);
  assert(stream);
  stream << bytes;
  assert(stream.good());
}

inline std::string recipe(const std::string& name,
                          const std::string& version,
                          const std::string& build_requirement = {},
                          const std::string& build_script = "true")
{
  return "format: zeppe-lin.recipe/1\n"
         "package:\n"
         "  name: " +
         name +
         "\n"
         "  version: " +
         version +
         "\n"
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
         "    " +
         build_script + "\n";
}

inline pkgsource::declaration_provenance provenance(std::size_t index)
{
  return pkgsource::declaration_provenance(
      "catalog.conf",
      "collections[" + std::to_string(index) + "]",
      1,
      static_cast<std::uint32_t>(index + 1));
}

inline pkgcatalog::acquire::collection_specification
specification(std::uint32_t precedence,
              const std::string& name,
              const std::filesystem::path& root,
              std::optional<std::string> revision = std::nullopt)
{
  return pkgcatalog::acquire::collection_specification(
      precedence,
      pkgcatalog::collection_reference(name),
      root,
      std::move(revision),
      provenance(precedence));
}

} // namespace test_support
