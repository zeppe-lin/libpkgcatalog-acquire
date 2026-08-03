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

#include <libpkgcatalog-acquire/export.h>
#include <libpkgcatalog/catalog.h>

namespace pkgcatalog::acquire {

/*! \brief Stable acquisition failure categories. */
enum class error_code {
  /*! The caller supplied no collections, duplicate authority, or invalid
   * limits. */
  invalid_request,
  /*! A collection root is empty, non-canonical, linked, or not a directory. */
  invalid_root,
  /*! Filesystem metadata inspection failed. */
  filesystem_failure,
  /*! A visible collection entry violates the native layout contract. */
  unsupported_entry,
  /*! A profiles or recipe document exceeds the configured byte ceiling. */
  document_too_large,
  /*! A regular document could not be read as one stable byte sequence. */
  document_read_failed,
};

/*! \brief Return the stable diagnostic spelling of an acquisition error code.
 *  \param value Error category to name.
 *  \return A process-lifetime string view.
 */
[[nodiscard]] PKGCATALOG_ACQUIRE_API std::string_view
to_string(error_code value) noexcept;

/*! \brief Filesystem acquisition failure with the exact observed path.
 *
 *  This exception reports only request, filesystem, layout, and document-read
 *  failures owned by this adapter. YAML syntax, source sealing, and catalog
 *  sealing retain their native exception domains.
 */
class PKGCATALOG_ACQUIRE_API error final : public std::runtime_error {
public:
  /*! \brief Construct an acquisition failure.
   *  \param code Stable failure category.
   *  \param path Exact diagnostic path associated with the observation.
   *  \param message Human-readable diagnostic text.
   */
  error(error_code code, std::filesystem::path path, std::string message);

  /*! \brief Destroy the polymorphic exception. */
  ~error() override;

  /*! \brief Return the stable failure category.
   *  \return The error code supplied at construction.
   */
  [[nodiscard]] error_code code() const noexcept;

  /*! \brief Return the exact diagnostic path.
   *  \return A reference valid for the lifetime of this exception.
   */
  [[nodiscard]] const std::filesystem::path& path() const noexcept;

private:
  error_code code_;
  std::filesystem::path path_;
};

/*! \brief One explicitly configured collection root.
 *
 *  The specification records caller authority. The adapter does not discover
 *  collections, infer names from paths, or assign precedence implicitly.
 */
class PKGCATALOG_ACQUIRE_API collection_specification final {
public:
  /*! \brief Construct one explicit collection request.
   *  \param precedence Contiguous precedence position beginning at zero.
   *  \param name Canonical catalog collection reference.
   *  \param root Filesystem root to observe.
   *  \param external_revision Optional diagnostic revision provenance.
   *  \param declaration Provenance of the configuration declaration.
   *  \throws error if the root or external revision is structurally invalid.
   */
  collection_specification(std::uint32_t precedence,
                           collection_reference name,
                           std::filesystem::path root,
                           std::optional<std::string> external_revision,
                           pkgsource::declaration_provenance declaration);

  /*! \brief Return the explicit precedence position.
   *  \return The caller-supplied position.
   */
  [[nodiscard]] std::uint32_t precedence() const noexcept;

  /*! \brief Return the canonical collection name.
   *  \return A reference valid for the lifetime of this specification.
   */
  [[nodiscard]] const collection_reference& name() const noexcept;

  /*! \brief Return the requested filesystem root.
   *  \return A reference valid for the lifetime of this specification.
   */
  [[nodiscard]] const std::filesystem::path& root() const noexcept;

  /*! \brief Return optional external revision provenance.
   *  \return A reference valid for the lifetime of this specification.
   */
  [[nodiscard]] const std::optional<std::string>&
  external_revision() const noexcept;

  /*! \brief Return provenance of the configuration declaration.
   *  \return A reference valid for the lifetime of this specification.
   */
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
class PKGCATALOG_ACQUIRE_API limits final {
public:
  /*! \brief Construct resource limits.
   *  \param max_document_bytes Inclusive byte ceiling for each YAML document.
   *  \throws error if the ceiling is zero.
   */
  explicit limits(std::uint64_t max_document_bytes = 1024U * 1024U);

  /*! \brief Return the per-document byte ceiling.
   *  \return A positive byte count.
   */
  [[nodiscard]] std::uint64_t max_document_bytes() const noexcept;

private:
  std::uint64_t max_document_bytes_;
};

/*! \brief Observe, parse, seal, and combine explicit native collections.
 *
 *  Acquisition is global and two-pass: every optional profiles document is
 *  parsed and sealed into one profile catalog before any recipe is parsed and
 *  sealed. The resulting source snapshots are then sealed into collection
 *  revisions and one catalog snapshot.
 *
 *  \param collections Explicit collection specifications. Positions must be
 *  contiguous from zero and names and normalized roots must be unique.
 *  \param resource_limits Per-document read and parser byte ceiling.
 *  \return One sealed catalog snapshot owned by libpkgcatalog.
 *  \throws error for request, filesystem, layout, and document-read failures.
 *  \throws pkgsource::yaml::yaml_error for YAML syntax and grammar failures.
 *  \throws pkgsource::error for source-authority refusal.
 *  \throws pkgcatalog::error for catalog-authority refusal.
 */
[[nodiscard]] PKGCATALOG_ACQUIRE_API catalog_snapshot
acquire_catalog(std::vector<collection_specification> collections,
                limits resource_limits = limits());

} // namespace pkgcatalog::acquire
