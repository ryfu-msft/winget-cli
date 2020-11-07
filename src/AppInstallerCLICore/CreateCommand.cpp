// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CreateCommand.h"
#include "Workflows/WorkflowBase.h"
#include "Resources.h"
#include <urlmon.h>

namespace AppInstaller::CLI
{
    using namespace std::string_view_literals;
    using namespace Utility::literals;

    std::vector<Argument> CreateCommand::GetArguments() const
    {
        return {
            Argument::ForType(Execution::Args::Type::InstallerURL),
        };
    }

    Resource::LocString CreateCommand::ShortDescription() const
    {
        return { Resource::String::CreateCommandShortDescription };
    }

    Resource::LocString CreateCommand::LongDescription() const
    {
        return { Resource::String::CreateCommandLongDescription };
    }

    std::string CreateCommand::HelpLink() const
    {
        return "https://aka.ms/winget-command-hash";
    }

    void CreateCommand::ExecuteInternal(Execution::Context& context) const
    {
        context <<
            Workflow::VerifyInstallerURL(Execution::Args::Type::InstallerURL) <<
            [](Execution::Context& context)
        {
            std::string_view installerArgUTF8 = context.Args.GetArg(Execution::Args::Type::InstallerURL);
            std::wstring installerArgUTF16 = Utility::ConvertToUTF16(installerArgUTF8);
            LPCWSTR installerURL = installerArgUTF16.c_str();
            
            // Obtain installer URL
            std::wstring tempPathUTF16 = std::filesystem::temp_directory_path().wstring();
            //LPCWSTR tempPath = tempPathUTF16.c_str();

            // Download URL to an installer file
            HRESULT result = URLDownloadToFile(NULL, installerURL, L"testspotify", BINDF_GETNEWESTVERSION, NULL);

            if (result == S_OK) {
                std::cout << "Download successful" << std::endl;
            }
            else if (result == E_OUTOFMEMORY) 
            {
                std::cout << "Insufficient memory to complete operation" << std::endl;
            }
            else if (result == INET_E_DOWNLOAD_FAILURE) 
            {
                std::cout << "Download failure" << std::endl;
            }
            // Hash Installer
            // Extract Metadata from URL
            // Save metadata information to fields to be converted to a manifest
            // Launch Wizard Mode
            std::cout << "Launching Manifest Generator Wizard" << std::endl;
        };
    }
}
