// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

namespace AppInstaller::Archive
{
    HRESULT ExtractArchive(const std::filesystem::path& archivePath, const std::filesystem::path& destPath)
    {
        IFileOperation* pfo;
        HRESULT hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));

        if (SUCCEEDED(hr))
        {
            // Turn off all UI from being shown to the user during the operation.
            pfo->SetOperationFlags(FOF_NO_UI);

            // Create IShellItem from destination path
            IShellItem* psiTo = NULL;
            hr = SHCreateItemFromParsingName(destPath.c_str(), NULL, IID_PPV_ARGS(&psiTo));

            if (SUCCEEDED(hr))
            {
                LPITEMIDLIST pidl; // pointer to an item id list
                IShellItem* psiFrom;
                IShellFolder* psf;
                IEnumIDList* pEnum;
                HRESULT result = SHParseDisplayName(archivePath.c_str(), NULL, &pidl, 0, NULL);
                result = SHBindToObject(NULL, pidl, NULL, IID_PPV_ARGS(&psf));
                result = psf->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum);

                LPITEMIDLIST pidlItem = NULL;
                ULONG nFetched;
                while (pEnum->Next(1, &pidlItem, &nFetched) == S_OK && nFetched == 1)
                {
                    STRRET strFolderName;
                    TCHAR szFolderName[MAX_PATH];
                    if ((psf->GetDisplayNameOf(pidlItem, SHGDN_INFOLDER, &strFolderName) == S_OK) &&
                        (StrRetToBuf(&strFolderName, pidlItem, szFolderName, MAX_PATH) == S_OK))
                    {
                        hr = SHCreateItemWithParent(NULL, psf, pidlItem, IID_PPV_ARGS(&psiFrom));
                        pfo->CopyItem(psiFrom, psiTo, NULL, NULL);
                    }
                    ILFree(pidlItem);
                }

                hr = pfo->PerformOperations();
                psiTo->Release();
                pfo->Release();
            }

            CoUninitialize();
        }

        return hr;
    }
}