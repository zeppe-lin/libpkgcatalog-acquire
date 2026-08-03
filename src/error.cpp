// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <utility>

#include <libpkgcatalog-acquire/acquire.h>

namespace pkgcatalog::acquire {

std::string_view to_string(error_code value) noexcept
{
  switch (value) {
  case error_code::invalid_request:
    return "invalid-request";
  case error_code::invalid_root:
    return "invalid-root";
  case error_code::filesystem_failure:
    return "filesystem-failure";
  case error_code::unsupported_entry:
    return "unsupported-entry";
  case error_code::document_too_large:
    return "document-too-large";
  case error_code::document_read_failed:
    return "document-read-failed";
  }
  return "unknown";
}

error::error(error_code code, std::filesystem::path path, std::string message)
    : std::runtime_error(std::move(message)), code_(code),
      path_(std::move(path))
{
}
error::~error() = default;
error_code error::code() const noexcept
{
  return code_;
}
const std::filesystem::path& error::path() const noexcept
{
  return path_;
}

} // namespace pkgcatalog::acquire
