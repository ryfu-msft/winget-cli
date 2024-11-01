// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "FontSource.h"

namespace AppInstaller::Repository::Microsoft
{
    namespace
    {
        // The source reference used by package objects.
        struct SourceReference
        {
            SourceReference(const std::shared_ptr<FontSource>& source) :
                m_source(source) {}

        protected:
            std::shared_ptr<FontSource> GetReferenceSource() const
            {
                std::shared_ptr<FontSource> source = m_source.lock();
                THROW_HR_IF(E_NOT_VALID_STATE, !source);
                return source;
            }

        private:
            std::weak_ptr<FontSource> m_source;
        };

        struct FontFaceVersionInfo
        {
            std::string Name;
            AppInstaller::Utility::OpenTypeFontVersion Version;

            FontFaceVersionInfo(std::string faceName, AppInstaller::Utility::OpenTypeFontVersion version)
                : Name(std::move(faceName)), Version(std::move(version)) {}
        };

        struct FontFamilyPackage
        {
            std::string Identifier;
            std::string Name;
            std::vector<FontFaceVersionInfo> Faces; // Faces are considered versions.

            FontFamilyPackage(std::string identifier, std::string name, std::vector<FontFaceVersionInfo> faces)
                : Identifier(std::move(identifier)), Name(std::move(name)), Faces(std::move(faces)) {}
        };

        struct FontPackage : public std::enable_shared_from_this<FontPackage>, public SourceReference, public IPackage
        {
            static constexpr IPackageType PackageType = IPackageType::FontPackage;

            FontPackage(const std::shared_ptr<FontSource>& source, const FontFamilyPackage& m_fontFamilyPackage
            ) :
                SourceReference(source), m_fontFamily(std::move(m_fontFamilyPackage))
            {
            }

            Utility::LocIndString GetProperty(PackageProperty property) const override
            {
                switch (property)
                {
                case PackageProperty::Id:
                    return Utility::LocIndString{ m_fontFamily.Identifier };
                case PackageProperty::Name:
                    return Utility::LocIndString{ m_fontFamily.Name };
                default:
                    THROW_HR(E_UNEXPECTED);
                }
            }

            Source GetSource() const override
            {
                return Source{ GetReferenceSource() };
            }

        private:
            FontFamilyPackage m_fontFamily;
        };

        struct FontPackageVersion : public SourceReference, public IPackageVersion
        {
            FontPackageVersion(
                const std::shared_ptr<FontSource>& source, std::shared_ptr<FontPackage>&& package, FontFaceVersionInfo versionInfo)
                : SourceReference(source), m_package(std::move(package)), m_versionInfo(std::move(versionInfo)) {}

            Utility::LocIndString GetProperty(PackageVersionProperty property) const override
            {
                switch (property)
                {
                case PackageVersionProperty::SourceIdentifier:
                    return Utility::LocIndString{ GetReferenceSource()->GetIdentifier() };
                case PackageVersionProperty::SourceName:
                    return Utility::LocIndString{ GetReferenceSource()->GetDetails().Name };
                case PackageVersionProperty::Id:
                    return Utility::LocIndString{ m_package->GetProperty(PackageProperty::Id) };
                case PackageVersionProperty::Name:
                    return Utility::LocIndString{ m_package->GetProperty(PackageProperty::Name) };
                case PackageVersionProperty::Version:
                    return Utility::LocIndString{ m_versionInfo.Name };
                case PackageVersionProperty::Channel:
                    return Utility::LocIndString{ m_versionInfo.Version.ToString() };
                default:
                    return {};
                }
            }

        private:
            std::shared_ptr<FontPackage> m_package;
            FontFaceVersionInfo m_versionInfo;
        };
    }

    FontSource::FontSource(const SourceDetails& details, SourceInformation information)
        : m_details(details), m_information(std::move(information))
    {
    }

    const std::string& FontSource::GetIdentifier() const
    {
        return m_details.Identifier;
    }

    const SourceDetails& FontSource::GetDetails() const
    {
        return m_details;
    }

    SourceInformation FontSource::GetInformation() const
    {
        return m_information;
    }

    SearchResult FontSource::Search(const SearchRequest& request) const
    {
        UNREFERENCED_PARAMETER(request);

        // TODO: implement search.
    }

    bool FontSource::IsSame(const FontSource* other) const
    {
        return (other && GetIdentifier() == other->GetIdentifier());
    }

    void* FontSource::CastTo(ISourceType type)
    {
        if (type == SourceType)
        {
            return this;
        }

        return nullptr;
    }

    std::shared_ptr<FontSource> FontSource::NonConstSharedFromThis() const
    {
        return const_cast<FontSource*>(this)->shared_from_this();
    }
}
