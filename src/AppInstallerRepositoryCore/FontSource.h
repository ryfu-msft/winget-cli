// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include "ISource.h"

namespace AppInstaller::Repository::Microsoft
{
    struct FontSource : public std::enable_shared_from_this<FontSource>, public ISource
    {
        static constexpr ISourceType SourceType = ISourceType::FontSource;

        FontSource(const SourceDetails& details, SourceInformation information);

        FontSource(const FontSource&) = delete;
        FontSource& operator=(const FontSource&) = delete;

        FontSource(FontSource&&) = default;
        FontSource& operator=(FontSource&&) = default;

        ~FontSource() = default;

        // Gets the source's identifier; a unique identifier independent of the name
        // that will not change between a remove/add or between additional adds.
        // Must be suitable for filesystem names.
        const std::string& GetIdentifier() const override;

        // Get the source's details.
        const SourceDetails& GetDetails() const override;

        SourceInformation GetInformation() const override;

        // Execute a search on the source.
        SearchResult Search(const SearchRequest& request) const override;

        // Determines if the other source refers to the same as this.
        bool IsSame(const FontSource* other) const;

        void* CastTo(ISourceType type);

    private:
        std::shared_ptr<FontSource> NonConstSharedFromThis() const;

        SourceDetails m_details;
        SourceInformation m_information;
    };
}
