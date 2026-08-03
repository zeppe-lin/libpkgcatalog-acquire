// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file export.h
 *  \brief Shared-library visibility declarations for libpkgcatalog-acquire.
 */
#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(PKGCATALOG_ACQUIRE_BUILDING_LIBRARY)
#define PKGCATALOG_ACQUIRE_API __declspec(dllexport)
#else
#define PKGCATALOG_ACQUIRE_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define PKGCATALOG_ACQUIRE_API __attribute__((visibility("default")))
#else
#define PKGCATALOG_ACQUIRE_API
#endif
