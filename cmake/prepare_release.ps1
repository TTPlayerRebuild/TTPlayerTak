[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Version,
    [string]$PreviousVersion,
    [Parameter(Mandatory = $true)][string]$Repository,
    [Parameter(Mandatory = $true)][string]$Commit,
    [string]$ArtifactDirectory = 'artifact',
    [string]$ServerUrl = 'https://github.com'
)
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'version.ps1')
$version = (Get-TakBuildVersion $Version).Name
if ($PreviousVersion -and -not (Read-TakVersionTag $PreviousVersion)) { throw 'Invalid previous release version.' }
if ($Repository -notmatch '^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$' -or $Commit -notmatch '^[0-9a-fA-F]{40}$') {
    throw 'Invalid repository or commit.'
}
$archiveName = "ttp_tak-$version.zip"
$sourceName = "ttp_tak-$version-source.zip"
$hash = (Get-FileHash -LiteralPath (Join-Path $ArtifactDirectory $archiveName) -Algorithm SHA256).Hash.ToLowerInvariant()
$sourceHash = (Get-FileHash -LiteralPath (Join-Path $ArtifactDirectory $sourceName) -Algorithm SHA256).Hash.ToLowerInvariant()
$expected = (Get-Content -LiteralPath (Join-Path $ArtifactDirectory 'SHA256SUMS.txt') -Encoding UTF8 -Raw).Trim().Replace("`r`n","`n")
if ($expected -cne "$hash  $archiveName`n$sourceHash  $sourceName") { throw 'TAK package SHA-256 verification failed.' }
$repoUrl = "$ServerUrl/$Repository"
$logUrl = if ($PreviousVersion) { "$repoUrl/compare/$PreviousVersion...$version" } else { "$repoUrl/commits/$version" }
$notes = @"
**更新记录**: $logUrl

解压 ttp_tak-$version.zip，关闭播放器后，将 AddIn/ttp_tak.dll 放入安装目录。
独立 TAK Reader，使用固定 FFmpeg 最小解码器，保留 8/16/24-bit 整数 PCM；标签和封面复用宿主 APEv2 服务。
同一 x86 DLL 面向 XP SP3、Win7 和新系统，CPU 需要 SSE2。
运行包仅包含 AddIn/ttp_tak.dll 和 SHA256SUMS.txt。对应源码附件含完整 FFmpeg 源码、修改和构建说明，许可证保留在源码中，不嵌入 DLL。

[源码与验证说明]($repoUrl/tree/$Commit)
[第三方版本、来源和修改记录]($repoUrl/blob/$Commit/docs/DEPENDENCIES.md)
[完整许可证]($repoUrl/tree/$Commit/LICENSES)
"@
$notes | Set-Content -LiteralPath (Join-Path $ArtifactDirectory 'release-notes.md') -Encoding UTF8

