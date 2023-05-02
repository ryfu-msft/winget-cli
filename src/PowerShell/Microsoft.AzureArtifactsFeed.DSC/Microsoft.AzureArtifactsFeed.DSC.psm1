# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

enum Ensure
{
    Absent
    Present
}

#region DSCResources
[DSCResource()]
class AzureDevOpsFeed
{
    [DscProperty(Key)]
    [string] $RepositoryName

    [DscProperty(Mandatory)]
    [string] $SourceLocationUri

    [DscProperty()]
    [bool] $Trusted = $false

    [DscProperty()]
    [bool] $UseNuget = $false

    [DscProperty(Key)]
    [Ensure] $Ensure = [Ensure]::Present

    [AzureDevOpsFeed] Get()
    {
        $currentState = [AzureDevOpsFeed]::new()

        $currentState.RepositoryName = $this.RepositoryName
        $currentState.SourceLocationUri = $this.SourceLocationUri
        $currentState.Ensure = [Ensure]::Absent
        $currentState.Trusted = $this.Trusted

        $registeredRepository = Get-PSRepository -Name $this.RepositoryName
        if ($null -ne $registeredRepository)
        {
            if ($registeredRepository.SourceLocation -eq $currentState.SourceLocationUri -and $registeredRepository.Trusted -eq $currentState.Trusted)
            {
                $currentState.Ensure = [Ensure]::Present
            }
        }

        return $currentState
    }

    [bool] Test()
    {
        $currentState = $this.Get()
        return $currentState.Ensure -eq $this.Ensure
    }

    [void] Set()
    {
        $inDesiredState = $this.Test()
        if ($inDesiredState -eq $false)
        {
            if ($this.Ensure -eq [Ensure]::Present)
            {
                $registeredRepository = Get-PSRepository -Name $this.RepositoryName
                if ($null -eq $registeredRepository)
                {
                    Unregister-PSRepository -Name $this.RepositoryName
                }             

                Assert-AzureCredentialProvider -UseNuget $this.UseNuget
                Invoke-AzureCredentialProvider -PackageSourceUri $this.SourceLocationUri -UseNuget $this.UseNuget

                if ($this.Trusted)
                {
                    Register-PSRepository -Name $this.RepositoryName -SourceLocation $this.SourceLocationUri -InstallationPolicy Trusted
                }
                else
                {
                    Register-PSRepository -Name $this.RepositoryName -SourceLocation $this.SourceLocationUri -InstallationPolicy Untrusted
                }
            }
            else
            {
                Unregister-PSRepository -Name $this.RepositoryName
            }
        }
    }
}

#endregion DSCResources

#region Functions
$AzureCredentialProviderNetCore = "$env:USERPROFILE\.nuget\plugins\netcore\CredentialProvider.Microsoft\CredentialProvider.Microsoft.exe"
$AzureCredentialProviderNetFx = "$env:USERPROFILE\.nuget\plugins\netfx\CredentialProvider.Microsoft\CredentialProvider.Microsoft.exe"

function Assert-AzureCredentialProvider
{
    param (
        [Parameter(Mandatory)]
        [bool]$UseNuget
    )

    if ($UseNuget)
    {
        if (-not (Test-Path -Path $AzureCredentialProviderNetFx))
        {
            Invoke-Expression "& { $(Invoke-RestMethod https://aka.ms/install-artifacts-credprovider.ps1) } -AddNetfx"
        }
    }
    else
    {
        if (-not (Test-Path -Path $AzureCredentialProviderNetCore))
        {
            Invoke-Expression "& { $(Invoke-RestMethod https://aka.ms/install-artifacts-credprovider.ps1) }"
        }
    }
}

function Invoke-AzureCredentialProvider
{
    param (
        [Parameter(Mandatory)]
        [string]$PackageSourceUri,

        [Parameter(Mandatory)]
        [bool]$UseNuget
    )

    if ($UseNuget)
    {
        Start-Process -FilePath $AzureCredentialProviderNetFx -ArgumentList "-C -U $PackageSourceUri -IsRetry"
    }
    else
    {
        Start-Process -FilePath $AzureCredentialProviderNetCore -ArgumentList "-C -U $PackageSourceUri -IsRetry"
    }
}
#endregion Functions