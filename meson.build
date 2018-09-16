project('detwinner', ['c', 'cpp'],
  version: '0.1.0',
  license: 'GPL3+',
  meson_version: '>=0.45.0',
	default_options : ['c_std=c11', 'cpp_std=c++17'])

add_project_arguments ('-DGTKMM_DISABLE_DEPRECATED', language:'cpp')
add_project_arguments('-Wall', '-Wextra', language: 'cpp')
i18n = import('i18n')

project_name = meson.project_name()
project_version = meson.project_version()

prefix = get_option('prefix')
bindir = get_option('bindir')
datadir = get_option('datadir')
localedir = get_option('localedir')

detwinner_conf = configuration_data()
detwinner_conf.set_quoted('PACKAGE_VERSION', project_version)
detwinner_conf.set_quoted('LOCALEDIR', join_paths(prefix, localedir))
detwinner_conf.set_quoted('GETTEXT_PACKAGE', project_name)
configure_file(output: 'config.h', configuration: detwinner_conf)

configuration_inc = include_directories('.')

subdir('data')
subdir('po')
subdir('resources')
subdir('src/detwinner-lib')
subdir('src/detwinner')
subdir('test')