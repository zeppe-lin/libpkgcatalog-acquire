// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/parser.h>

#include "pkgcatalog_scan_config.h"

#include <libpkgsource/error.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <libpkgcatalog-acquire/acquire.h>
#include <libpkgcatalog/error.h>

namespace {

struct raw_collection final {
  std::string name;
  std::string root;
  std::optional<std::string> revision;
};

[[noreturn]] void usage_error(const std::string& message)
{
  throw std::runtime_error(message);
}

std::pair<std::string, std::string> assignment(const std::string& value,
                                               const std::string& option)
{
  const std::size_t equals = value.find('=');
  if (equals == std::string::npos || equals == 0 ||
      equals + 1 == value.size()) {
    usage_error(option + " requires NAME=VALUE");
  }
  return {value.substr(0, equals), value.substr(equals + 1)};
}

std::uint64_t decimal(const std::string& value)
{
  if (value.empty()) {
    usage_error("empty byte limit");
  }
  if (value.size() > 1 && value.front() == '0') {
    usage_error("byte limit is not canonical decimal");
  }
  std::uint64_t result = 0;
  for (const char character : value) {
    if (character < '0' || character > '9') {
      usage_error("byte limit is not canonical decimal");
    }
    const std::uint64_t digit = static_cast<unsigned>(character - '0');
    if (result > (std::numeric_limits<std::uint64_t>::max() - digit) / 10) {
      usage_error("byte limit is too large");
    }
    result = result * 10 + digit;
  }
  if (result == 0) {
    usage_error("byte limit must be greater than zero");
  }
  return result;
}

void print_help()
{
  std::cout << "usage: pkgcatalog-scan [--max-document-bytes N]\n"
               "       --collection NAME=ROOT [--collection NAME=ROOT ...]\n"
               "       [--external-revision NAME=REVISION ...]\n";
}

} // namespace

int main(int argc, char** argv)
{
  try {
    std::vector<raw_collection> raw;
    std::map<std::string, std::string> revisions;
    std::uint64_t max_document_bytes = 1024U * 1024U;
    for (int index = 1; index < argc; ++index) {
      const std::string argument(argv[index]);
      if (argument == "--help") {
        print_help();
        return 0;
      }
      if (argument == "--version") {
        std::cout << "pkgcatalog-scan " PKGCATALOG_SCAN_VERSION "\n";
        return 0;
      }
      if (argument == "--collection") {
        if (++index == argc) {
          usage_error("--collection requires NAME=ROOT");
        }
        auto [name, root] = assignment(argv[index], "--collection");
        raw.push_back(
            raw_collection{std::move(name), std::move(root), std::nullopt});
        continue;
      }
      if (argument == "--external-revision") {
        if (++index == argc) {
          usage_error("--external-revision requires NAME=REVISION");
        }
        auto [name, revision] = assignment(argv[index], "--external-revision");
        if (!revisions.emplace(std::move(name), std::move(revision)).second) {
          usage_error("duplicate external revision for one collection");
        }
        continue;
      }
      if (argument == "--max-document-bytes") {
        if (++index == argc) {
          usage_error("--max-document-bytes requires N");
        }
        max_document_bytes = decimal(argv[index]);
        continue;
      }
      usage_error("unknown option: " + argument);
    }
    if (raw.empty()) {
      usage_error("at least one --collection is required");
    }

    for (raw_collection& collection : raw) {
      const auto revision = revisions.find(collection.name);
      if (revision != revisions.end()) {
        collection.revision = revision->second;
        revisions.erase(revision);
      }
    }
    if (!revisions.empty()) {
      usage_error("external revision names an unknown collection: " +
                  revisions.begin()->first);
    }

    if (raw.size() > std::numeric_limits<std::uint32_t>::max()) {
      usage_error("too many collections");
    }
    std::vector<pkgcatalog::acquire::collection_specification> specifications;
    specifications.reserve(raw.size());
    for (std::size_t index = 0; index < raw.size(); ++index) {
      specifications.emplace_back(
          static_cast<std::uint32_t>(index),
          pkgcatalog::collection_reference(raw[index].name),
          raw[index].root,
          raw[index].revision,
          pkgsource::declaration_provenance(
              "<command-line>",
              "collections[" + std::to_string(index) + "]",
              1,
              static_cast<std::uint32_t>(index + 1)));
    }

    const pkgcatalog::catalog_snapshot snapshot =
        pkgcatalog::acquire::acquire_catalog(
            std::move(specifications),
            pkgcatalog::acquire::limits(max_document_bytes));
    std::cout << "catalog " << snapshot.identity().hex() << '\n';
    for (const pkgcatalog::catalog_collection& collection :
         snapshot.collections()) {
      std::cout << "collection " << collection.precedence() << ' '
                << collection.collection().name().name() << ' '
                << collection.collection().revision_identity().hex() << '\n';
    }
    for (const pkgcatalog::catalog_candidate& candidate :
         snapshot.candidates()) {
      std::cout << pkgcatalog::to_string(candidate.status()) << ' '
                << candidate.package().name() << ' '
                << candidate.release().version_release() << ' '
                << candidate.collection().name() << ' '
                << candidate.identity().hex();
      if (candidate.shadowed_by()) {
        std::cout << " shadowed-by " << candidate.shadowed_by()->hex();
      }
      std::cout << '\n';
    }
    return 0;
  } catch (const pkgcatalog::acquire::error& value) {
    std::cerr << "pkgcatalog-scan: "
              << pkgcatalog::acquire::to_string(value.code()) << ": "
              << value.what() << '\n';
  } catch (const pkgsource::yaml::yaml_error& value) {
    std::cerr << "pkgcatalog-scan: yaml: " << value.document() << ':'
              << value.line() << ':' << value.column() << ": " << value.path()
              << ": " << value.what() << '\n';
  } catch (const pkgsource::error& value) {
    std::cerr << "pkgcatalog-scan: source authority: " << value.what() << '\n';
  } catch (const pkgcatalog::error& value) {
    std::cerr << "pkgcatalog-scan: catalog authority: " << value.what() << '\n';
  } catch (const std::exception& value) {
    std::cerr << "pkgcatalog-scan: " << value.what() << '\n';
  }
  return 1;
}
