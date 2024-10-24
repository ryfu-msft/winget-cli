// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include "ISource.h"
#include "SourceFactory.h"

#include <string_view>

namespace AppInstaller::Repository::Microsoft
{
    using namespace std::string_view_literals;

    struct FontSourceFactory
    {
        // Get the type string for this source.
        static constexpr std::string_view Type()
        {
            using namespace std::string_view_literals;
            return "Microsoft.Fonts"sv;
        }

        // Creates a source factory for this type.
        static std::unique_ptr<ISourceFactory> Create();
    };
}
