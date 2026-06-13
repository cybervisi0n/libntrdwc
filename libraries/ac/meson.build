if(build_target == 'arm')
    nitrodwc_prebuilt += fs.copyfile('prebuilt' / 'libdwcac.a')
else
    subdir('src')
    libdwcac_includes = include_directories('include')
    libdwcac = library('dwcac',
        dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
        include_directories: [nitrodwc_includes, libdwcac_includes],
        pic: false,
        sources: libdwcac_srcs,
        native: true
    )

    nitrodwc_libs += libdwcac
endif
