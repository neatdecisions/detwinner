rm -rf Debug
rm -rf Release
mkdir Debug
mkdir Release
cd Debug
#CC=clang CXX=clang++ meson .. --buildtype=debug
meson .. --buildtype=debug
cd ../Release
#CC=clang CXX=clang++ meson .. --buildtype=release
meson .. --buildtype=release
