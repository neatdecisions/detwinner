magick_dep = dependency('GraphicsMagick++')
thread_dep = dependency('threads')

srcLogic = [
    'logic/callbacks/ImageFinderCallback.cpp',
    'logic/tools/StopTimer.cpp',
    'logic/DuplicateFilesFinder.cpp',
    'logic/DuplicateImageFinder.cpp',
    'logic/FileIndexer.cpp',
    'logic/Initializer.cpp',
    'logic/MurmurHash.cpp',
    'logic/images/ImageFeatures.cpp',
    'logic/images/ImageFeaturesBridge.cpp',
    'logic/images/ImageFeaturesBuilder.cpp',
    'logic/images/SimilarityCache.cpp',
    'logic/images/SimilarityCacheBuilder.cpp',
    'logic/images/SimilarImageFinder.cpp'
]


cc = meson.get_compiler('cpp')
fs_dep = cc.find_library('stdc++fs', required : false)


incdirLogic = include_directories('.', '../misc')

deps = [thread_dep, magick_dep, fs_dep]

libLogic = static_library(
    'detwinnerlogic',
    srcLogic,
    dependencies: deps,
    include_directories : incdirLogic,
    cpp_args: ['-Wno-ignored-attributes','-std=c++17'])

libLogic_dep = declare_dependency(
    dependencies: deps,
    include_directories : incdirLogic,
    link_with : libLogic)