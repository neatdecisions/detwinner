subdir('icons')

po_dir = join_paths (meson.source_root (), 'po')

desktop = i18n.merge_file ('desktop',
                           input: 'com.neatdecisions.Detwinner.desktop.in',
                           output: 'com.neatdecisions.Detwinner.desktop',
                           install: true,
                           install_dir: join_paths (datadir, 'applications'),
                           po_dir: po_dir,
                           type: 'desktop')

appdata = i18n.merge_file ('appdata',
                           input: 'com.neatdecisions.Detwinner.appdata.xml.in',
                           output: 'com.neatdecisions.Detwinner.appdata.xml',
                           install: true,
                           install_dir: join_paths (datadir, 'metainfo'),
                           po_dir: po_dir)

desktop_file_validate = find_program ('desktop-file-validate', required: false)
if desktop_file_validate.found ()
    test ('validate-desktop', desktop_file_validate,
          args: [desktop.full_path ()])
endif

appdata_file_validate = find_program ('appstream-util', required: false)
if appdata_file_validate.found ()
    test ('validate-appdata', appdata_file_validate,
          args: ['validate-strict', '--nonet', appdata.full_path ()])
endif

install_man('detwinner.1')
