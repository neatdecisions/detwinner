rm -rf Debug
rm -rf Release
rm -rf DebugOptimized
mkdir Debug
mkdir DebugOptimized
mkdir Release
cd Debug
meson .. --buildtype=debug
cd ../Release
meson .. --buildtype=release
cd ../DebugOptimized
meson .. --buildtype=debugoptimized
