# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libfastfail.so", "./bmbfmod.json" -DestinationPath "./fastfail_v1.0.0.zip" -Update
}
