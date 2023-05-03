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
    [bool] $Trusted = $true

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

        $match = Find-PSRepository -Name $currentState.RepositoryName
        if ($null -ne $match)
        {
            if (($match.SourceLocation -eq $currentState.SourceLocationUri) -and ($match.Trusted -eq $currentState.Trusted))
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
        if (-not $inDesiredState)
        {
            if ($this.Ensure -eq [Ensure]::Present)
            {
                Assert-DotNetVersion

                # Unregister repository if one with the same name already exists.
                $repositoryMatch = Find-PSRepository -Name $this.RepositoryName
                if ($null -ne $repositoryMatch)
                {
                    Unregister-PSRepository -Name $this.RepositoryName         
                }

                # Unregister package source if one with the same name already exists.
                $packageSourceMatch = Find-PackageSource -Name $this.RepositoryName
                if ($null -ne $packageSourceMatch)
                {
                    Unregister-PackageSource -Name $this.RepositoryName -ProviderName Nuget
                }

                # Set environment variable to cache ADAL session token.
                $env:NUGET_CREDENTIALPROVIDER_ADAL_FILECACHE_ENABLED = $true

                Assert-AzureCredentialProvider -UseNuget $this.UseNuget
                Invoke-AzureCredentialProvider -PackageSourceUri $this.SourceLocationUri -UseNuget $this.UseNuget

                if ($this.Trusted)
                {
                    Register-PSRepository -Name $this.RepositoryName -SourceLocation $this.SourceLocationUri -InstallationPolicy Trusted
                    Register-PackageSource -Name $this.RepositoryName -Location $this.SourceLocationUri -ProviderName NuGet -Trusted -SkipValidate
                }
                else
                {
                    Register-PSRepository -Name $this.RepositoryName -SourceLocation $this.SourceLocationUri
                    Register-PackageSource -Name $this.RepositoryName -Location $this.SourceLocationUri -ProviderName NuGet -SkipValidate
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

function Assert-DotNetVersion
{
    $dotnetVersion = $null
    try
    {
        $dotnetVersion = Invoke-Expression -Command 'dotnet --version'
    }
    catch
    {
        throw "dotnet SDK is not installed. Visit https://dotnet.microsoft.com/en-us/download to install the latest dotnet SDK."
    }

    if ([System.Version]$dotnetVersion -lt [System.Version]'6.0')
    {
        throw "dotnet SDK v6.0.x is required."
    }
}

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
        Start-Process -FilePath $AzureCredentialProviderNetFx -ArgumentList "-C -U $PackageSourceUri -IsRetry" -Wait
    }
    else
    {
        Start-Process -FilePath $AzureCredentialProviderNetCore -ArgumentList "-C -U $PackageSourceUri -IsRetry" -Wait
    }
}

function Find-PSRepository
{
    param (
        [Parameter(Mandatory)]
        [string]$Name
    )
    return Get-PSRepository | Where-Object {$_.Name -eq $Name}
}

function Find-PackageSource
{
    param (
        [Parameter(Mandatory)]
        [string]$Name
    )
    return Get-PackageSource | Where-Object {$_.Name -eq $Name}
}
#endregion Functions