# ====================================================================
# FILE:    dev/build_m2_verify.ps1
# OWNER:   Member 2
# PURPOSE:
#   Builds the Member 2 verification harness against the merged renderer
#   and the GLM/GLAD headers supplied with the OpenGL template.
#
# USAGE:
#   powershell -ExecutionPolicy Bypass -File dev/build_m2_verify.ps1 `
#     -TemplateRoot "C:\path\to\OpenGL Template for VS Code (Other)"
#   then run: .\dev\m2_verify.exe from the project root
#
# The harness writes dev/out/*.ppm images - open them with any viewer.
# ====================================================================

param([string]$TemplateRoot = $env:OPENGL_TEMPLATE_ROOT)

$ErrorActionPreference = 'Stop'

# Project root = the folder that contains this script's parent.
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

$GXX = 'd:\graphics-opengl\graphics\mingw\mingw64\bin\g++.exe'
if (-not (Test-Path $GXX)) {
    $GXX = 'g++'   # fall back to whatever is on PATH
}

$GlmInclude = ''
$GladInclude = ''
if ($TemplateRoot) {
    $GlmInclude = Join-Path $TemplateRoot 'external\glm'
    $GladInclude = Join-Path $TemplateRoot 'external\glad\include'
}
if (-not (Test-Path (Join-Path $GlmInclude 'glm\glm.hpp'))) {
    throw 'GLM headers were not found. Pass -TemplateRoot or set OPENGL_TEMPLATE_ROOT.'
}
if (-not (Test-Path (Join-Path $GladInclude 'glad\gl.h'))) {
    throw 'GLAD 2 headers were not found under the provided template.'
}

$Sources = @(
    'dev/m2_verify.cpp',
    'src/core/Renderer.cpp',
    'src/core/Shader.cpp',
    'src/core/glad.cpp',
    'src/entities/HierarchicalModel.cpp',
    'src/entities/CharacterModels.cpp',
    'src/entities/Animation.cpp',
    'src/entities/Projectile.cpp',
    'src/entities/Player.cpp',
    'src/entities/Enemy.cpp'
)

Write-Host "Building Member 2 verification harness with $GXX ..."
& $GXX -std=c++17 -O2 -Wall -Wextra -I src -I $GlmInclude -I $GladInclude -o dev/m2_verify.exe $Sources

if ($LASTEXITCODE -ne 0) {
    Write-Host "BUILD FAILED (exit $LASTEXITCODE)" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "BUILD OK -> dev/m2_verify.exe" -ForegroundColor Green
Write-Host "Run it from the project root:  .\dev\m2_verify.exe"
