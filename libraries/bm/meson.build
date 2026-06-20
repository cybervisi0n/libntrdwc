if(build_target == 'arm')
    nitrodwc_prebuilt += fs.copyfile('prebuilt' / 'libdwcbm.a')
else
    subdir('src')
    libdwcbm_includes = include_directories('include')
    libdwcbm = library('dwcbm',
        dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
        include_directories: [nitrodwc_includes, libdwcbm_includes],
        pic: true,
        sources: libdwcbm_srcs,
        native: (build_target == 'linux' or build_target == 'win64')
    )

    nitrodwc_libs += libdwcbm
endif
