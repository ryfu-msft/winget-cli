// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include <AppInstallerLogging.h>
#include <AppInstallerVersions.h>
#include <AppInstallerStrings.h>

#include "ConfigurationSetSerializer.h"
#include "ConfigurationSetSerializer_0_2.h"
#include "ConfigurationSetUtilities.h"

using namespace AppInstaller::YAML;
using namespace winrt::Windows::Foundation;

namespace winrt::Microsoft::Management::Configuration::implementation
{
    std::unique_ptr<ConfigurationSetSerializer> ConfigurationSetSerializer::CreateSerializer(hstring version)
    {
        // Create the parser based on the version selected
        AppInstaller::Utility::SemanticVersion schemaVersion(std::move(winrt::to_string(version)));

        // TODO: Consider having the version/uri/type information all together in the future
        if (schemaVersion.PartAt(0).Integer == 0 && schemaVersion.PartAt(1).Integer == 1)
        {
            throw E_NOTIMPL;
        }
        else if (schemaVersion.PartAt(0).Integer == 0 && schemaVersion.PartAt(1).Integer == 2)
        {
            return std::make_unique<ConfigurationSetSerializer_0_2>();
        }
        else if (schemaVersion.PartAt(0).Integer == 0 && schemaVersion.PartAt(1).Integer == 3)
        {
            throw E_NOTIMPL;
        }
        else
        {
            AICLI_LOG(Config, Error, << "Unknown configuration version: " << schemaVersion.ToString());
            throw E_UNEXPECTED;
        }
    }

    void ConfigurationSetSerializer::WriteYamlValueSet(AppInstaller::YAML::Emitter& emitter, const Windows::Foundation::Collections::ValueSet& valueSet)
    {
        emitter << BeginMap;

        for (const auto& [key, value] : valueSet)
        {
            IPropertyValue property = value.try_as<IPropertyValue>();
            std::string keyName = winrt::to_string(key);
            auto type = property.Type();

            if (type == PropertyType::Boolean)
            {
                emitter << AppInstaller::YAML::Key << keyName << AppInstaller::YAML::Value << property.GetBoolean();
            }
            else if (type == PropertyType::String)
            {
                emitter << AppInstaller::YAML::Key << keyName << AppInstaller::YAML::Value << AppInstaller::Utility::ConvertToUTF8(property.GetString());
            }
            else if (type == PropertyType::Int64)
            {
                emitter << AppInstaller::YAML::Key << keyName << AppInstaller::YAML::Value << property.GetInt64();
            }
        }

        emitter << EndMap;
    }

    void ConfigurationSetSerializer::WriteYamlConfigurationUnits(AppInstaller::YAML::Emitter& emitter, const std::vector<ConfigurationUnit>& units)
    {
        emitter << BeginSeq;

        for (auto unit : units)
        {
            // Resource
            emitter << BeginMap;
            emitter << Key << GetConfigurationFieldName(ConfigurationFieldName::Resource) << Value << AppInstaller::Utility::ConvertToUTF8(unit.Type());

            // Directives
            const auto& metadata = unit.Metadata();
            emitter << Key << GetConfigurationFieldName(ConfigurationFieldName::Directives);
            WriteYamlValueSet(emitter, metadata);

            // Settings
            auto settings = unit.Settings();
            emitter << Key << GetConfigurationFieldName(ConfigurationFieldName::Settings);
            WriteYamlValueSet(emitter, settings);

            emitter << EndMap;
        }

        emitter << EndSeq;
    }
}
