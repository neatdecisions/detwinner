testsrc = [
    'test.cpp',
    'settings/SearchSettingsManagerTest.cpp'
]

gmock_dep = dependency('gmock')
gtest_dep = dependency('gtest')

# extract object files from the executable
ssm_cpp = exe.extract_objects('settings/SearchSettingsManager.cpp')

test_detwinner_exe = executable(
    'test_detwinner_exe',
    testsrc,
    include_directories : incdir,
    dependencies: [gtkmm_dep, gtest_dep, gmock_dep, libLogic_dep],
    cpp_args: ['-Wno-ignored-attributes','-std=c++17'],
    link_with : libLogic,
    objects: ssm_cpp)

# test execution
test('Detwinner - presentation layer unit tests', test_detwinner_exe,
    workdir: meson.current_source_dir())
