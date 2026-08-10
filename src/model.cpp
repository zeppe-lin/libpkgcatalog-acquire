// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <utility>

#include <libpkgcatalog-acquire/acquire.h>

namespace pkgcatalog::acquire {

collection_specification::collection_specification(
    std::uint32_t precedence,
    collection_reference name,
    std::filesystem::path root,
    std::optional<std::string> external_revision,
    pkgsource::declaration_provenance declaration)
    : precedence_(precedence), name_(std::move(name)), root_(std::move(root)),
      external_revision_(std::move(external_revision)),
      declaration_(std::move(declaration))
{
  if (root_.empty()) {
    throw error(error_code::invalid_request, root_, "collection root is empty");
  }
  if (external_revision_ &&
      (external_revision_->empty() ||
       external_revision_->find('\n') != std::string::npos ||
       external_revision_->find('\r') != std::string::npos ||
       external_revision_->find('\0') != std::string::npos)) {
    throw error(error_code::invalid_request,
                root_,
                "invalid external revision provenance");
  }
}
std::uint32_t collection_specification::precedence() const noexcept
{
  return precedence_;
}
const collection_reference& collection_specification::name() const noexcept
{
  return name_;
}
const std::filesystem::path& collection_specification::root() const noexcept
{
  return root_;
}
const std::optional<std::string>&
collection_specification::external_revision() const noexcept
{
  return external_revision_;
}
const pkgsource::declaration_provenance&
collection_specification::declaration() const noexcept
{
  return declaration_;
}

limits::limits(std::uint64_t max_document_bytes)
    : max_document_bytes_(max_document_bytes)
{
  if (max_document_bytes_ == 0) {
    throw error(error_code::invalid_request,
                {},
                "max_document_bytes must be greater than zero");
  }
}
std::uint64_t limits::max_document_bytes() const noexcept
{
  return max_document_bytes_;
}

} // namespace pkgcatalog::acquire
