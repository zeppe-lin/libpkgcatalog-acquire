// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file acquire.h
 *  \brief Explicit filesystem acquisition for native catalog declarations.
 */
#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <libpkgcatalog/catalog.h>

namespace pkgcatalog::acquire {

/*! \brief Stable acquisition failure categories. */
enum class error_code {
  invalid_request,
  invalid_root,
  filesystem_failure,
  unsupported_entry,
  document_too_large,
  document_read_failed,
};
[[nodiscard]] std::string_view to_string(error_code value) noexcept;

/*! \brief Filesystem acquisition failure with the exact observed path. */
class error final : public std::runtime_error {
public:
  error(error_code code, std::filesystem::path path, std::string message);
  [[nodiscard]] error_code code() const noexcept;
  [[nodiscard]] const std::filesystem::path& path() const noexcept;
private:
  error_code code_;
  std::filesystem::path path_;
};

/*! \brief One explicitly configured collection root. */
class collection_specification final {
public:
  collection_specification(
      std::uint32_t precedence,
      collection_reference name,
      std::filesystem::path root,
      std::optional<std::string> external_revision,
      pkgsource::declaration_provenance declaration);
  [[nodiscard]] std::uint32_t precedence() const noexcept;
  [[nodiscard]] const collection_reference& name() const noexcept;
  [[nodiscard]] const std::filesystem::path& root() const noexcept;
  [[nodiscard]] const std::optional<std::string>&
  external_revision() const noexcept;
  [[nodiscard]] const pkgsource::declaration_provenance&
  declaration() const noexcept;
private:
  std::uint32_t precedence_;
  collection_reference name_;
  std::filesystem::path root_;
  std::optional<std::string> external_revision_;
  pkgsource::declaration_provenance declaration_;
};

/*! \brief Explicit resource limits for acquisition document reads. */
class limits final {
public:
  explicit limits(std::uint64_t max_document_bytes = 1024U * 1024U);
  [[nodiscard]] std::uint64_t max_document_bytes() const noexcept;
private:
  std::uint64_t max_document_bytes_;
};

/*! \brief Acquire, parse, seal, and combine explicit native collections. */
[[nodiscard]] catalog_snapshot acquire_catalog(
    std::vector<collection_specification> collections,
    limits resource_limits = limits());

} // namespace pkgcatalog::acquire
