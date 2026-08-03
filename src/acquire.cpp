// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgcatalog-acquire/acquire.h>

#include <libpkgcatalog/error.h>
#include <libpkgsource-yaml/parser.h>

#include <algorithm>
#include <fstream>
#include <limits>
#include <set>
#include <system_error>
#include <utility>

namespace pkgcatalog::acquire {
namespace {

std::string path_text(const std::filesystem::path& value)
{
  return value.generic_string();
}

bool line_safe(const std::string& value)
{
  return !value.empty() && value.find('\n') == std::string::npos &&
         value.find('\r') == std::string::npos &&
         value.find('\0') == std::string::npos;
}

[[noreturn]] void fail(error_code code, const std::filesystem::path& path,
                       const std::string& message)
{
  throw error(code, path, message + ": " + path_text(path));
}

std::filesystem::file_status status_of(const std::filesystem::path& path)
{
  std::error_code ec;
  const std::filesystem::file_status result =
      std::filesystem::symlink_status(path, ec);
  if (ec == std::errc::no_such_file_or_directory)
    return std::filesystem::file_status(
        std::filesystem::file_type::not_found);
  if (ec)
    fail(error_code::filesystem_failure, path, ec.message());
  return result;
}

bool exists_without_error(const std::filesystem::path& path)
{
  const std::filesystem::file_status status = status_of(path);
  return status.type() != std::filesystem::file_type::not_found;
}

std::filesystem::path normalized_root(const std::filesystem::path& root)
{
  if (root.empty())
    fail(error_code::invalid_root, root, "empty collection root");
  std::error_code ec;
  std::filesystem::path result = std::filesystem::absolute(root, ec);
  if (ec)
    fail(error_code::filesystem_failure, root, ec.message());
  result = result.lexically_normal();
  if (!line_safe(path_text(result)))
    fail(error_code::invalid_root, result,
         "collection root is not line-safe");
  const std::filesystem::path canonical = std::filesystem::canonical(result, ec);
  if (ec)
    fail(error_code::filesystem_failure, result, ec.message());
  if (canonical != result)
    fail(error_code::invalid_root, result,
         "collection root contains a symbolic-link component");
  const std::filesystem::file_status status = status_of(result);
  if (std::filesystem::is_symlink(status))
    fail(error_code::invalid_root, result,
         "collection root must not be a symbolic link");
  if (!std::filesystem::is_directory(status))
    fail(error_code::invalid_root, result,
         "collection root is not a directory");
  return result;
}

std::string read_document(const std::filesystem::path& path,
                          const limits& resource_limits)
{
  const std::filesystem::file_status status = status_of(path);
  if (std::filesystem::is_symlink(status))
    fail(error_code::unsupported_entry, path,
         "document must not be a symbolic link");
  if (!std::filesystem::is_regular_file(status))
    fail(error_code::unsupported_entry, path,
         "document is not a regular file");

  std::error_code ec;
  const std::uintmax_t size = std::filesystem::file_size(path, ec);
  if (ec)
    fail(error_code::filesystem_failure, path, ec.message());
  if (size > resource_limits.max_document_bytes())
    fail(error_code::document_too_large, path,
         "document exceeds the configured byte limit");
  if (size > static_cast<std::uintmax_t>(
                 std::numeric_limits<std::size_t>::max()) ||
      size > static_cast<std::uintmax_t>(
                 std::numeric_limits<std::streamsize>::max()))
    fail(error_code::document_too_large, path,
         "document cannot be represented in memory");

  std::ifstream stream(path, std::ios::binary);
  if (!stream)
    fail(error_code::document_read_failed, path,
         "cannot open document");
  std::string bytes(static_cast<std::size_t>(size), '\0');
  if (!bytes.empty())
    stream.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
  if ((!bytes.empty() && stream.gcount() !=
                           static_cast<std::streamsize>(bytes.size())) ||
      stream.bad())
    fail(error_code::document_read_failed, path,
         "short or failed document read");
  if (stream.peek() != std::char_traits<char>::eof())
    fail(error_code::document_read_failed, path,
         "document changed while it was read");
  return bytes;
}

bool hidden_name(const std::filesystem::path& path)
{
  const std::string name = path.filename().string();
  return !name.empty() && name.front() == '.';
}

std::vector<std::filesystem::path>
package_directories(const std::filesystem::path& root)
{
  std::error_code ec;
  std::filesystem::directory_iterator iterator(root, ec);
  if (ec)
    fail(error_code::filesystem_failure, root, ec.message());

  std::vector<std::filesystem::path> result;
  for (const std::filesystem::directory_entry& entry : iterator) {
    const std::filesystem::path path = entry.path();
    if (!line_safe(path_text(path)))
      fail(error_code::unsupported_entry, path,
           "collection entry path is not line-safe");
    if (hidden_name(path))
      continue;
    const std::filesystem::file_status status = status_of(path);
    if (std::filesystem::is_symlink(status))
      fail(error_code::unsupported_entry, path,
           "collection entries must not be symbolic links");
    if (std::filesystem::is_directory(status)) {
      const std::filesystem::path recipe = path / "recipe.yml";
      if (!exists_without_error(recipe))
        fail(error_code::unsupported_entry, path,
             "non-hidden package directory has no recipe.yml");
      result.push_back(path);
      continue;
    }
    if (!std::filesystem::is_regular_file(status))
      fail(error_code::unsupported_entry, path,
           "unsupported non-hidden collection entry");
  }
  std::sort(result.begin(), result.end(),
            [](const std::filesystem::path& lhs,
               const std::filesystem::path& rhs) {
              return lhs.filename().generic_string() <
                     rhs.filename().generic_string();
            });
  return result;
}

struct observed_collection final {
  collection_specification specification;
  std::filesystem::path root;
};

std::vector<observed_collection> normalize_specifications(
    std::vector<collection_specification> collections)
{
  if (collections.empty())
    throw error(error_code::invalid_request, {},
                "at least one collection is required");
  std::sort(collections.begin(), collections.end(),
            [](const collection_specification& lhs,
               const collection_specification& rhs) {
              return lhs.precedence() < rhs.precedence();
            });

  std::set<collection_reference> names;
  std::set<std::string> roots;
  std::vector<observed_collection> result;
  result.reserve(collections.size());
  for (std::size_t index = 0; index < collections.size(); ++index) {
    collection_specification& specification = collections[index];
    if (specification.precedence() != index)
      throw error(error_code::invalid_request, specification.root(),
                  "collection precedence must be contiguous from zero");
    if (!names.insert(specification.name()).second)
      throw error(error_code::invalid_request, specification.root(),
                  "duplicate collection name: " +
                      specification.name().name());
    std::filesystem::path root = normalized_root(specification.root());
    if (!roots.insert(path_text(root)).second)
      throw error(error_code::invalid_request, root,
                  "duplicate normalized collection root");
    result.push_back(observed_collection{
        std::move(specification), std::move(root)});
  }
  return result;
}

} // namespace

std::string_view to_string(error_code value) noexcept
{
  switch (value) {
  case error_code::invalid_request: return "invalid-request";
  case error_code::invalid_root: return "invalid-root";
  case error_code::filesystem_failure: return "filesystem-failure";
  case error_code::unsupported_entry: return "unsupported-entry";
  case error_code::document_too_large: return "document-too-large";
  case error_code::document_read_failed: return "document-read-failed";
  }
  return "unknown";
}

error::error(error_code code, std::filesystem::path path, std::string message)
    : std::runtime_error(std::move(message)),
      code_(code),
      path_(std::move(path))
{
}
error_code error::code() const noexcept { return code_; }
const std::filesystem::path& error::path() const noexcept { return path_; }

collection_specification::collection_specification(
    std::uint32_t precedence,
    collection_reference name,
    std::filesystem::path root,
    std::optional<std::string> external_revision,
    pkgsource::declaration_provenance declaration)
    : precedence_(precedence),
      name_(std::move(name)),
      root_(std::move(root)),
      external_revision_(std::move(external_revision)),
      declaration_(std::move(declaration))
{
  if (root_.empty())
    throw error(error_code::invalid_request, root_,
                "collection root is empty");
  if (external_revision_ &&
      (external_revision_->empty() ||
       external_revision_->find('\n') != std::string::npos ||
       external_revision_->find('\r') != std::string::npos))
    throw error(error_code::invalid_request, root_,
                "invalid external revision provenance");
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
  if (max_document_bytes_ == 0)
    throw error(error_code::invalid_request, {},
                "max_document_bytes must be greater than zero");
}
std::uint64_t limits::max_document_bytes() const noexcept
{
  return max_document_bytes_;
}

catalog_snapshot acquire_catalog(
    std::vector<collection_specification> collections,
    limits resource_limits)
{
  std::vector<observed_collection> observed =
      normalize_specifications(std::move(collections));

  std::vector<pkgsource::profile_declaration> profile_declarations;
  for (const observed_collection& collection : observed) {
    const std::filesystem::path document = collection.root / "profiles.yml";
    if (!exists_without_error(document))
      continue;
    pkgsource::yaml_adapter::parsed_profile_document parsed =
        pkgsource::yaml_adapter::parse_profiles_yaml_v1(
            read_document(document, resource_limits),
            pkgsource::source_origin(path_text(document)));
    profile_declarations.insert(profile_declarations.end(),
                                parsed.declarations().begin(),
                                parsed.declarations().end());
  }
  pkgsource::profile_catalog profiles =
      pkgsource::profile_catalog::seal(std::move(profile_declarations));

  std::vector<catalog_collection> sealed_collections;
  sealed_collections.reserve(observed.size());
  for (const observed_collection& collection : observed) {
    std::vector<pkgsource::source_snapshot> sources;
    for (const std::filesystem::path& package_directory :
         package_directories(collection.root)) {
      const std::filesystem::path recipe = package_directory / "recipe.yml";
      sources.push_back(pkgsource::yaml_adapter::seal_recipe_yaml_v1(
          read_document(recipe, resource_limits),
          pkgsource::source_origin(path_text(recipe)), profiles));
    }
    collection_provenance provenance(
        path_text(collection.root),
        collection.specification.external_revision(),
        collection.specification.declaration());
    sealed_collections.emplace_back(
        collection.specification.precedence(),
        seal_collection(collection_declaration(
            collection.specification.name(), std::move(provenance),
            std::move(sources))));
  }
  return catalog_snapshot::seal(std::move(profiles),
                                std::move(sealed_collections));
}

} // namespace pkgcatalog::acquire
