# ====================================================================
# FILE:    dev/build_m2_verify.ps1
# OWNER:   Member 2
# PURPOSE:
#   Builds the headless Member 2 verification harness (dev/m2_verify.cpp)
#   together with every source file Member 2 owns, using the MinGW g++
#   that ships with the project.  No OpenGL, GLFW or window is needed.
#
# USAGE:
#   powershell -ExecutionPolicy Bypass -File dev/build_m2_verify.ps1
#   then run:  ./dev/m2_verify.exe     (from the project root)
#
# The harness writes dev/out/*.ppm images - open them with any viewer.
# ====================================================================

$ErrorActionPreference = 'Stop'

# Project root = the folder that contains this script's parent.
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

$GXX = 'd:\graphics-opengl\graphics\mingw\mingw64\bin\g++.exe'
if (-not (Test-Path $GXX)) {
    $GXX = 'g++'   # fall back to whatever is on PATH
}

$Sources = @(
    'dev/m2_verify.cpp',
    'src/entities/HierarchicalModel.cpp',
    'src/entities/CharacterModels.cpp',
    'src/entities/Animation.cpp',
    'src/entities/Projectile.cpp',
    'src/entities/Player.cpp',
    'src/entities/Enemy.cpp'
)

Write-Host "Building Member 2 verification harness with $GXX ..."
& $GXX -std=c++17 -O2 -Wall -Wextra -I src -o dev/m2_verify.exe $Sources

if ($LASTEXITCODE -ne 0) {
    Write-Host "BUILD FAILED (exit $LASTEXITCODE)" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "BUILD OK -> dev/m2_verify.exe" -ForegroundColor Green
Write-Host "Run it from the project root:  ./dev/m2_verify.exe"
