// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include "pch.h"

namespace AppInstaller::Filesystem
{
    // Checks if the file system at path supports named streams/ADS
    bool SupportsNamedStreams(const std::filesystem::path& path);

    // Checks if the file system at path supports hard links
    bool SupportsHardLinks(const std::filesystem::path& path);

    // Checks if the file system at path support reparse points
    bool SupportsReparsePoints(const std::filesystem::path& path);

    // Checks that the path does not contain any reserved words.
    bool IsNonReservedFilename(const std::filesystem::path& path);
}