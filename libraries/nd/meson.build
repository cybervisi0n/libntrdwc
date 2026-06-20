if(build_target == 'arm')
    nitrodwc_prebuilt += fs.copyfile('prebuilt' / 'libdwcnd.a')
else
    subdir('src')
    libdwcnd = library('dwcnd',
        dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
        include_directories: [nitrodwc_includes],
        pic: build_target == 'nx',
        sources: libdwcnd_srcs,
        native: (build_target == 'linux' or build_target == 'win64')
    )

    nitrodwc_libs += libdwcnd
endif
