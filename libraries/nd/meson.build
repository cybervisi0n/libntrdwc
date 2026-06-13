if(build_target == 'arm')
    nitrodwc_prebuilt += fs.copyfile('prebuilt' / 'libdwcnd.a')
else
    subdir('src')
    libdwcnd = library('dwcnd',
        dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
        include_directories: [nitrodwc_includes],
        pic: false,
        sources: libdwcnd_srcs,
        native: true
    )

    nitrodwc_libs += libdwcnd
endif
