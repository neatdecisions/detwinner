testsrc = [
    'test.cpp',
    'logic/DuplicateFilesFinderTest.cpp',
    'logic/DuplicateImageFinderTest.cpp',
    'logic/FileIndexerTest.cpp',
    'logic/MurmurHashTest.cpp',
    'logic/TestingHelpers.cpp',
    'logic/callbacks/ImageFinderCallbackTest.cpp',
    'logic/images/HistogramTTest.cpp',
    'logic/images/ImageFeaturesBuilderTest.cpp',
    'logic/images/ImageFeaturesTest.cpp',
    'logic/images/ImageFeaturesTestFactory.cpp',
    'logic/images/SimilarityCacheBuilderTest.cpp',
    'logic/images/SimilarImageFinderTest.cpp'
]

gmock_dep = dependency('gmock')
gtest_dep = dependency('gtest')

testexe = executable(
    'testexe',
    testsrc,
    include_directories : incdir,
    dependencies: [gtest_dep, gmock_dep, libLogic_dep],
    cpp_args: ['-Wno-ignored-attributes','-std=c++17'],
    link_with : libLogic)

# test execution
test('Image search test', testexe,
    workdir: meson.current_source_dir())
