if(build_target == 'arm')
    nitrodwc_prebuilt += fs.copyfile('prebuilt' / 'libdwcac.a')
else
    subdir('src')
    libdwcac_includes = include_directories('include')
    libdwcac = library('dwcac',
        dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
        include_directories: [nitrodwc_includes, libdwcac_includes],
        pic: true,
        sources: libdwcac_srcs,
        native: (build_target == 'linux' or build_target == 'win64')
    )

    nitrodwc_libs += libdwcac
endif
