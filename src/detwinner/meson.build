gtkmm_dep = dependency('gtkmm-3.0', version:'>=3.22.0')

src = [
    'Detwinner.cpp',
    'DetwinnerApp.cpp',
    'callbacks/ControlCallback.cpp',
    'callbacks/SearchProgressCallback.cpp',
    'settings/SearchSettingsManager.cpp',
    'tools/BackupFileDeleter.cpp',
    'tools/PermanentFileDeleter.cpp',
    'tools/TrashFileDeleter.cpp',
    'tools/IconManager.cpp',
    'tools/MemoryMappedFile.cpp',
    'ui/AutosizedImage.cpp',
    'ui/DeferredActionProgressDialog.cpp',
    'ui/DetwinnerWindow.cpp',
    'ui/FilePreview.cpp',
    'ui/FileTreeView.cpp',
    'ui/RegexListbox.cpp',
    'ui/SearchOptionsPane.cpp',
    'ui/SearchProgressDialog.cpp',
    'ui/SearchResultsPane.cpp',
    'ui/DuplicatesTreeView.cpp',
    'ui/SearchSettingsDialog.cpp',
    detwinnerresources
]

incdir = include_directories('.', '../misc')

exe = executable(
    'detwinner',
    src,
    dependencies: [gtkmm_dep, libLogic_dep],
    include_directories : [incdir, configuration_inc],
    cpp_args: ['-Wno-ignored-attributes','-std=c++20'],
    c_args: ['-Wno-overlength-strings'],
    link_with: [libLogic],
    install: true)

