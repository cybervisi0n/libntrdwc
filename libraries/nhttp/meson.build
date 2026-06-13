if(build_target == 'arm')
    nitrodwc_prebuilt += fs.copyfile('prebuilt' / 'libdwcnhttp.a')
else
    subdir('src')
    libdwcnhttp = library('dwcnhttp',
        dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
        include_directories: [nitrodwc_includes],
        pic: false,
        sources: libdwcnhttp_srcs,
        native: true
    )

    nitrodwc_libs += libdwcnhttp
endif
