// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include "pch.h"

namespace AppInstaller::Archive
{
    // Extracts the archive file to the specified path
    void ExtractArchive(const std::filesystem::path& archivePath, const std::filesystem::path& destPath);
}