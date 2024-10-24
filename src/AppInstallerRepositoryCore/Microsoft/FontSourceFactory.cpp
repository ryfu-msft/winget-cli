// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "FontSourceFactory.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace AppInstaller::Repository::Microsoft
{
    namespace
    {
        struct FontSourceReference : public ISourceReference
        {
            FontSourceReference(const SourceDetails& details) : m_details(details) {}

            SourceDetails& GetDetails() override { return m_details; };

            std::string GetIdentifier() override { return m_details.Identifier; }

            std::shared_ptr<ISource> Open(IProgressCallback&) override
            {

            }

        private:
            SourceDetails m_details;
        };

        // The base class for data that comes from a rest based source.
        struct FontSourceFactoryImpl : public ISourceFactory
        {
            std::string_view TypeName() const override final
            {
                return FontSourceFactory::Type();
            }

            std::shared_ptr<ISourceReference> Create(const SourceDetails& details) override final
            {
                THROW_HR_IF(E_INVALIDARG, !Utility::CaseInsensitiveEquals(details.Type, FontSourceFactory::Type()));

                return std::make_shared<FontSourceReference>(details);
            }

            bool Add(SourceDetails& details, IProgressCallback&) override final
            {
                if (details.Type.empty())
                {
                    details.Type = FontSourceFactory::Type();
                }
                else
                {
                    THROW_HR_IF(E_INVALIDARG, !Utility::CaseInsensitiveEquals(details.Type, FontSourceFactory::Type()));
                }

                return true;
            }

            bool Update(const SourceDetails& details, IProgressCallback&) override final
            {
                THROW_HR_IF(E_INVALIDARG, !Utility::CaseInsensitiveEquals(details.Type, FontSourceFactory::Type()));
                return true;
            }

            bool Remove(const SourceDetails& details, IProgressCallback&) override final
            {
                THROW_HR_IF(E_INVALIDARG, !Utility::CaseInsensitiveEquals(details.Type, FontSourceFactory::Type()));
                return true;
            }
        };
    }

    std::unique_ptr<ISourceFactory> FontSourceFactory::Create()
    {
        return std::make_unique<FontSourceFactoryImpl>();
    }
}
